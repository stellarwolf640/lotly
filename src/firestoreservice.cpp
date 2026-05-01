#include "firestoreservice.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QUrlQuery>

// ── Constants ─────────────────────────────────────────────────────────────────

const QString FirestoreService::REST_KEY =
    QStringLiteral("AIzaSyBBR5rj3rbqn0AZeh2uEHq2pgBgPy1WXOw");

const QString FirestoreService::BASE_URL =
    QStringLiteral("https://firestore.googleapis.com/v1/projects/"
                   "project-57ed75dd-e5b1-47d8-94b/databases/(default)/documents");

// ── Construction ──────────────────────────────────────────────────────────────

FirestoreService::FirestoreService(QObject *parent)
    : QObject(parent)
    , m_network(new QNetworkAccessManager(this))
{
}

// ── Public API ────────────────────────────────────────────────────────────────

void FirestoreService::fetchParkingLots()
{
    get(QStringLiteral("/parking_lots"), ReplyTag::FetchLots);
}

void FirestoreService::storeParkingLot(const ParkingLot &lot)
{
    // PATCH with the lot's placeId as the document ID (idempotent upsert).
    const QString relPath = QStringLiteral("/parking_lots/") + lot.id();
    const QByteArray body = QJsonDocument(
        QJsonObject{{QStringLiteral("fields"), lotToFields(lot)}}
    ).toJson(QJsonDocument::Compact);
    patch(relPath, body, ReplyTag::StoreLot, lot.id());
}

void FirestoreService::fetchAllReports()
{
    // Fetch the 500 most-recent reports across all lots via a structured query.
    // No where-clause means the entire lot_reports collection is scanned.
    // The timestamp DESC order + limit keeps the result set bounded.
    const QString relPath = QStringLiteral(":runQuery");

    QJsonObject from;
    from[QStringLiteral("collectionId")] = QStringLiteral("lot_reports");

    QJsonObject orderField;
    orderField[QStringLiteral("fieldPath")] = QStringLiteral("timestamp");
    QJsonObject orderBy;
    orderBy[QStringLiteral("field")]     = orderField;
    orderBy[QStringLiteral("direction")] = QStringLiteral("DESCENDING");

    QJsonObject query;
    query[QStringLiteral("from")]    = QJsonArray{from};
    query[QStringLiteral("orderBy")] = QJsonArray{orderBy};
    query[QStringLiteral("limit")]   = 500;

    const QByteArray body =
        QJsonDocument(QJsonObject{{QStringLiteral("structuredQuery"), query}})
            .toJson(QJsonDocument::Compact);

    post(relPath, body, ReplyTag::FetchAllReports);
}

void FirestoreService::fetchReportsForLot(const QString &lotId)
{
    // Firestore structured query via the runQuery endpoint.
    // Filters lot_reports collection where lotId == <lotId>.
    const QString relPath = QStringLiteral(":runQuery");

    QJsonObject fieldRef;
    fieldRef[QStringLiteral("fieldPath")] = QStringLiteral("lotId");

    QJsonObject fieldFilter;
    fieldFilter[QStringLiteral("field")]    = fieldRef;
    fieldFilter[QStringLiteral("op")]       = QStringLiteral("EQUAL");
    fieldFilter[QStringLiteral("value")]    = sv(lotId);

    QJsonObject filter;
    filter[QStringLiteral("fieldFilter")] = fieldFilter;

    QJsonObject from;
    from[QStringLiteral("collectionId")] = QStringLiteral("lot_reports");

    QJsonObject orderBy;
    QJsonObject orderField;
    orderField[QStringLiteral("fieldPath")] = QStringLiteral("timestamp");
    orderBy[QStringLiteral("field")]     = orderField;
    orderBy[QStringLiteral("direction")] = QStringLiteral("DESCENDING");

    QJsonObject query;
    query[QStringLiteral("from")]    = QJsonArray{from};
    query[QStringLiteral("where")]   = filter;
    query[QStringLiteral("orderBy")] = QJsonArray{orderBy};

    QJsonObject structuredQuery;
    structuredQuery[QStringLiteral("structuredQuery")] = query;

    post(relPath,
         QJsonDocument(structuredQuery).toJson(QJsonDocument::Compact),
         ReplyTag::FetchReports,
         lotId);
}

void FirestoreService::storeReport(const LotReport &report)
{
    // POST to lot_reports (no document ID) — Firestore auto-generates one.
    const QByteArray body = QJsonDocument(
        QJsonObject{{QStringLiteral("fields"), reportToFields(report)}}
    ).toJson(QJsonDocument::Compact);
    post(QStringLiteral("/lot_reports"), body, ReplyTag::StoreReport,
         report.lotId(), report);
}

void FirestoreService::fetchUser(const QString &userId)
{
    get(QStringLiteral("/users/") + userId, ReplyTag::FetchUser, userId);
}

void FirestoreService::storeUser(const User &user)
{
    // PATCH — creates or fully overwrites the user document.
    const QString relPath = QStringLiteral("/users/") + user.id();
    const QByteArray body = QJsonDocument(
        QJsonObject{{QStringLiteral("fields"), userToFields(user)}}
    ).toJson(QJsonDocument::Compact);
    patch(relPath, body, ReplyTag::StoreUser, user.id());
}

void FirestoreService::updateUser(const User &user)
{
    // Identical to storeUser — PATCH is an upsert in Firestore REST.
    storeUser(user);
}

void FirestoreService::deleteDocument(const QString &collectionPath,
                                      const QString &documentId)
{
    del(QStringLiteral("/") + collectionPath + QStringLiteral("/") + documentId,
        ReplyTag::Delete, documentId);
}

// ── Network helpers ───────────────────────────────────────────────────────────

QNetworkRequest FirestoreService::buildRequest(const QString &relPath) const
{
    QUrl url(BASE_URL + relPath);
    QUrlQuery q;
    q.addQueryItem(QStringLiteral("key"), REST_KEY);
    url.setQuery(q);

    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader,
                  QStringLiteral("application/json"));
    req.setHeader(QNetworkRequest::UserAgentHeader,
                  QStringLiteral("LotlyApp/1.0"));
    return req;
}

void FirestoreService::get(const QString &relPath, ReplyTag tag,
                            const QString &extra)
{
    QNetworkReply *reply = m_network->get(buildRequest(relPath));
    m_pending[reply] = {tag, extra, {}};
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onReplyFinished(reply);
    });
}

void FirestoreService::post(const QString &relPath, const QByteArray &body,
                             ReplyTag tag, const QString &extra,
                             const LotReport &pending)
{
    QNetworkReply *reply = m_network->post(buildRequest(relPath), body);
    m_pending[reply] = {tag, extra, pending};
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onReplyFinished(reply);
    });
}

void FirestoreService::patch(const QString &relPath, const QByteArray &body,
                              ReplyTag tag, const QString &extra)
{
    QNetworkReply *reply = m_network->sendCustomRequest(
        buildRequest(relPath), "PATCH", body);
    m_pending[reply] = {tag, extra, {}};
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onReplyFinished(reply);
    });
}

void FirestoreService::del(const QString &relPath, ReplyTag tag,
                            const QString &extra)
{
    QNetworkReply *reply = m_network->deleteResource(buildRequest(relPath));
    m_pending[reply] = {tag, extra, {}};
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onReplyFinished(reply);
    });
}

// ── Reply dispatch ────────────────────────────────────────────────────────────

void FirestoreService::onReplyFinished(QNetworkReply *reply)
{
    reply->deleteLater();

    const ReplyContext ctx = m_pending.take(reply);
    const QByteArray data  = reply->readAll();

    if (reply->error() != QNetworkReply::NoError) {
        const QString op = [&]() {
            switch (ctx.tag) {
            case ReplyTag::FetchLots:       return QStringLiteral("fetchParkingLots");
            case ReplyTag::StoreLot:        return QStringLiteral("storeParkingLot");
            case ReplyTag::FetchAllReports: return QStringLiteral("fetchAllReports");
            case ReplyTag::FetchReports:    return QStringLiteral("fetchReportsForLot");
            case ReplyTag::StoreReport:     return QStringLiteral("storeReport");
            case ReplyTag::FetchUser:       return QStringLiteral("fetchUser");
            case ReplyTag::StoreUser:
            case ReplyTag::UpdateUser:      return QStringLiteral("storeUser");
            case ReplyTag::Delete:          return QStringLiteral("deleteDocument");
            }
            return QStringLiteral("unknown");
        }();
        qWarning() << "[Firestore]" << op << "error:" << reply->errorString()
                   << "HTTP:" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()
                   << data;
        // For FetchUser, a 404 means the user simply doesn't exist yet — not an error.
        if (ctx.tag == ReplyTag::FetchUser &&
            reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 404) {
            emit userNotFound(ctx.extra);
        } else {
            emit errorOccurred(op, reply->errorString());
        }
        return;
    }

    switch (ctx.tag) {
    case ReplyTag::FetchLots:
        handleLotsResponse(data);
        break;

    case ReplyTag::StoreLot:
        emit parkingLotStored(ctx.extra);
        break;

    case ReplyTag::FetchAllReports:
        handleAllReportsResponse(data);
        break;

    case ReplyTag::FetchReports:
        handleReportsResponse(data, ctx.extra);
        break;

    case ReplyTag::StoreReport: {
        // The POST response body is the newly created document with its name.
        const QJsonObject doc = QJsonDocument::fromJson(data).object();
        const QString docId   = docIdFromName(doc.value(QStringLiteral("name")).toString());
        emit reportStored(docId, ctx.pendingReport);
        break;
    }

    case ReplyTag::FetchUser:
        handleUserResponse(data, ctx.extra);
        break;

    case ReplyTag::StoreUser:
    case ReplyTag::UpdateUser:
        emit userStored(ctx.extra);
        break;

    case ReplyTag::Delete:
        // No signal needed — deletion is fire-and-forget.
        qDebug() << "[Firestore] Deleted document:" << ctx.extra;
        break;
    }
}

// ── Response parsers ──────────────────────────────────────────────────────────

void FirestoreService::handleLotsResponse(const QByteArray &data)
{
    const QJsonObject root = QJsonDocument::fromJson(data).object();
    QList<ParkingLot> lots;

    // List response: {"documents": [...]}
    const QJsonArray docs = root.value(QStringLiteral("documents")).toArray();
    for (const QJsonValue &val : docs) {
        const QJsonObject doc    = val.toObject();
        const QJsonObject fields = doc.value(QStringLiteral("fields")).toObject();
        if (fields.isEmpty()) continue;
        lots.append(fieldsToLot(fields, doc.value(QStringLiteral("name")).toString()));
    }

    emit parkingLotsReceived(lots);
}

void FirestoreService::handleAllReportsResponse(const QByteArray &data)
{
    // Same array format as per-lot runQuery: [{document:{...}}, ...]
    const QJsonArray results = QJsonDocument::fromJson(data).array();
    QList<LotReport> reports;

    for (const QJsonValue &val : results) {
        const QJsonObject entry  = val.toObject();
        const QJsonObject doc    = entry.value(QStringLiteral("document")).toObject();
        if (doc.isEmpty()) continue;
        const QJsonObject fields = doc.value(QStringLiteral("fields")).toObject();
        if (fields.isEmpty()) continue;
        LotReport r = fieldsToReport(fields, doc.value(QStringLiteral("name")).toString());
        if (!r.lotId().isEmpty())
            reports.append(r);
    }

    qDebug() << "[Firestore] fetchAllReports returned" << reports.size() << "reports";
    emit allReportsReceived(reports);
}

void FirestoreService::handleReportsResponse(const QByteArray &data,
                                              const QString &lotId)
{
    // runQuery returns an array; each element is {"document": {...}} or {"skippedResults": N}
    const QJsonArray results = QJsonDocument::fromJson(data).array();
    QList<LotReport> reports;

    for (const QJsonValue &val : results) {
        const QJsonObject entry  = val.toObject();
        const QJsonObject doc    = entry.value(QStringLiteral("document")).toObject();
        if (doc.isEmpty()) continue;
        const QJsonObject fields = doc.value(QStringLiteral("fields")).toObject();
        if (fields.isEmpty()) continue;
        reports.append(fieldsToReport(fields,
                                      doc.value(QStringLiteral("name")).toString()));
    }

    emit reportsReceived(lotId, reports);
}

void FirestoreService::handleUserResponse(const QByteArray &data,
                                           const QString &userId)
{
    const QJsonObject doc    = QJsonDocument::fromJson(data).object();
    const QJsonObject fields = doc.value(QStringLiteral("fields")).toObject();
    if (fields.isEmpty()) {
        emit userNotFound(userId);
        return;
    }
    emit userReceived(fieldsToUser(fields, doc.value(QStringLiteral("name")).toString()));
}

// ── Domain ↔ Firestore converters ─────────────────────────────────────────────

QJsonObject FirestoreService::lotToFields(const ParkingLot &lot)
{
    return {
        {QStringLiteral("id"),                      sv(lot.id())},
        {QStringLiteral("name"),                    sv(lot.name())},
        {QStringLiteral("location"),                sv(lot.location())},
        {QStringLiteral("address"),                 sv(lot.address())},
        {QStringLiteral("latitude"),                dv(lot.latitude())},
        {QStringLiteral("longitude"),               dv(lot.longitude())},
        {QStringLiteral("placeId"),                 sv(lot.placeId())},
        {QStringLiteral("totalSpaces"),             iv(lot.totalSpaces())},
        {QStringLiteral("predictedAvailableSpaces"),iv(lot.predictedAvailableSpaces())},
        {QStringLiteral("confidence"),              dv(lot.confidence())},
        {QStringLiteral("explanation"),             sv(lot.explanation())},
    };
}

QJsonObject FirestoreService::reportToFields(const LotReport &report)
{
    return {
        {QStringLiteral("lotId"),       sv(report.lotId())},
        {QStringLiteral("userId"),      sv(report.userId())},
        {QStringLiteral("status"),      iv(static_cast<int>(report.status()))},
        {QStringLiteral("trustScore"),  dv(report.trustScore())},
        {QStringLiteral("timestamp"),   tv(report.timestamp())},
        // Tag simulated reports so we can filter/delete them efficiently.
        {QStringLiteral("isSimulated"), bv(report.userId().startsWith(
                                            QStringLiteral("sim-")))},
    };
}

QJsonObject FirestoreService::userToFields(const User &user)
{
    return {
        {QStringLiteral("id"),               sv(user.id())},
        {QStringLiteral("displayName"),      sv(user.displayName())},
        {QStringLiteral("email"),            sv(user.email())},
        {QStringLiteral("photoUrl"),         sv(user.photoUrl())},
        {QStringLiteral("reliabilityScore"), dv(user.reliabilityScore())},
    };
}

ParkingLot FirestoreService::fieldsToLot(const QJsonObject &f,
                                          const QString &docName)
{
    const QString id = rs(f, QStringLiteral("id")).isEmpty()
                       ? docIdFromName(docName)
                       : rs(f, QStringLiteral("id"));

    ParkingLot lot(
        id,
        rs(f, QStringLiteral("name")),
        rs(f, QStringLiteral("location")),
        rd(f, QStringLiteral("latitude")),
        rd(f, QStringLiteral("longitude")),
        rs(f, QStringLiteral("placeId")),
        ri(f, QStringLiteral("totalSpaces")),
        ri(f, QStringLiteral("predictedAvailableSpaces")),
        rd(f, QStringLiteral("confidence")),
        rs(f, QStringLiteral("explanation"))
    );
    lot.setAddress(rs(f, QStringLiteral("address")));
    return lot;
}

LotReport FirestoreService::fieldsToReport(const QJsonObject &f,
                                            const QString & /*docName*/)
{
    return LotReport(
        rs(f, QStringLiteral("lotId")),
        rs(f, QStringLiteral("userId")),
        static_cast<LotReport::Status>(ri(f, QStringLiteral("status"))),
        rd(f, QStringLiteral("trustScore")),
        rt(f, QStringLiteral("timestamp"))
    );
}

User FirestoreService::fieldsToUser(const QJsonObject &f,
                                     const QString &docName)
{
    const QString id = rs(f, QStringLiteral("id")).isEmpty()
                       ? docIdFromName(docName)
                       : rs(f, QStringLiteral("id"));
    return User(
        id,
        rs(f, QStringLiteral("displayName")),
        rs(f, QStringLiteral("email")),
        rs(f, QStringLiteral("photoUrl")),
        rd(f, QStringLiteral("reliabilityScore"))
    );
}

// ── Firestore field value helpers ─────────────────────────────────────────────

QJsonObject FirestoreService::sv(const QString &s)
{
    return {{QStringLiteral("stringValue"), s}};
}

QJsonObject FirestoreService::iv(int i)
{
    // Firestore integerValue is serialised as a JSON string.
    return {{QStringLiteral("integerValue"), QString::number(i)}};
}

QJsonObject FirestoreService::dv(double d)
{
    return {{QStringLiteral("doubleValue"), d}};
}

QJsonObject FirestoreService::bv(bool b)
{
    return {{QStringLiteral("booleanValue"), b}};
}

QJsonObject FirestoreService::tv(const QDateTime &dt)
{
    // RFC 3339 / ISO-8601 with milliseconds, UTC.
    return {{QStringLiteral("timestampValue"),
             dt.toUTC().toString(Qt::ISODateWithMs)}};
}

QString FirestoreService::rs(const QJsonObject &f, const QString &k)
{
    return f.value(k).toObject().value(QStringLiteral("stringValue")).toString();
}

int FirestoreService::ri(const QJsonObject &f, const QString &k)
{
    return f.value(k).toObject()
             .value(QStringLiteral("integerValue")).toString().toInt();
}

double FirestoreService::rd(const QJsonObject &f, const QString &k)
{
    return f.value(k).toObject()
             .value(QStringLiteral("doubleValue")).toDouble();
}

bool FirestoreService::rb(const QJsonObject &f, const QString &k)
{
    return f.value(k).toObject()
             .value(QStringLiteral("booleanValue")).toBool();
}

QDateTime FirestoreService::rt(const QJsonObject &f, const QString &k)
{
    const QString ts = f.value(k).toObject()
                        .value(QStringLiteral("timestampValue")).toString();
    // We store timestamps via tv() as dt.toUTC().toString(Qt::ISODateWithMs),
    // which in Qt 6 appends "Z" for UTC datetimes.
    // Qt::ISODateWithMs handles the "Z" suffix and marks the result Qt::UTC.
    // We convert to local time so that .time().hour() returns the user's LOCAL
    // hour — essential for hour-proximity weighting in DataProcessor.
    QDateTime dt = QDateTime::fromString(ts, Qt::ISODateWithMs);
    if (!dt.isValid()) {
        qWarning() << "[Firestore] Invalid timestamp value:" << ts;
        return QDateTime::currentDateTime();
    }
    // Already local? Leave it. UTC? Convert. Unknown spec? Treat as UTC.
    if (dt.timeSpec() == Qt::LocalTime)
        return dt;
    return dt.toLocalTime();
}

QString FirestoreService::docIdFromName(const QString &name)
{
    // name = "projects/.../documents/collection/DOC_ID"
    const int last = name.lastIndexOf(QLatin1Char('/'));
    return (last >= 0) ? name.mid(last + 1) : name;
}
