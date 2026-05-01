#pragma once

#include <QList>
#include <QMap>
#include <QObject>

#include "lotreport.h"
#include "parkinglot.h"
#include "user.h"

class QNetworkAccessManager;
class QNetworkReply;
class QNetworkRequest;

// FirestoreService handles all communication with Cloud Firestore via the REST API.
//
// Base URL: https://firestore.googleapis.com/v1/projects/{project}/databases/(default)/documents
// All requests include ?key=REST_API_KEY as a query parameter.
//
// Firestore field format:
//   {"fields": {"fieldName": {"stringValue": "..."}, "count": {"integerValue": "5"}, ...}}
//
// All operations are asynchronous — connect to the signals to handle results.
class FirestoreService : public QObject
{
    Q_OBJECT

public:
    explicit FirestoreService(QObject *parent = nullptr);

    // ── Parking lots ──────────────────────────────────────────────────────────
    void fetchParkingLots();
    void storeParkingLot(const ParkingLot &lot);

    // ── Reports ───────────────────────────────────────────────────────────────
    // Fetches all lot_reports, ordered by timestamp DESC, limited to 500 docs.
    // Emits allReportsReceived() when complete.
    void fetchAllReports();

    // Fetches all reports for a single lot via a structured query.
    // Emits reportsReceived(lotId, reports) when complete.
    void fetchReportsForLot(const QString &lotId);
    void storeReport(const LotReport &report);

    // ── Users ─────────────────────────────────────────────────────────────────
    void fetchUser(const QString &userId);
    void storeUser(const User &user);
    void updateUser(const User &user);

    // ── Generic delete (used to remove simulated/cleared reports) ─────────────
    // collectionPath: e.g. "lot_reports", documentId: Firestore doc ID
    void deleteDocument(const QString &collectionPath, const QString &documentId);

signals:
    void parkingLotsReceived(const QList<ParkingLot> &lots);
    void parkingLotStored(const QString &lotId);

    // Emitted with the auto-generated Firestore document ID so callers can
    // track which document to delete later (e.g. for simulated report cleanup).
    void reportsReceived(const QString &lotId, const QList<LotReport> &reports);

    // Emitted by fetchAllReports() with the complete cross-lot result set.
    void allReportsReceived(const QList<LotReport> &reports);

    void reportStored(const QString &firestoreDocId, const LotReport &report);

    void userReceived(const User &user);
    void userNotFound(const QString &userId);
    void userStored(const QString &userId);

    void errorOccurred(const QString &operation, const QString &error);

private:
    // ── Internal reply routing ─────────────────────────────────────────────────
    enum class ReplyTag {
        FetchLots,
        StoreLot,
        FetchAllReports,   // global cross-lot fetch
        FetchReports,      // per-lot fetch
        StoreReport,
        FetchUser,
        StoreUser,
        UpdateUser,
        Delete
    };

    struct ReplyContext {
        ReplyTag   tag;
        QString    extra;      // lotId for FetchReports/StoreReport, userId for user ops
        LotReport  pendingReport; // kept so reportStored can echo it back
    };

    QNetworkRequest buildRequest(const QString &relPath) const;
    void get(const QString &relPath, ReplyTag tag, const QString &extra = {});
    void post(const QString &relPath, const QByteArray &body, ReplyTag tag,
              const QString &extra = {}, const LotReport &pending = {});
    void patch(const QString &relPath, const QByteArray &body, ReplyTag tag,
               const QString &extra = {});
    void del(const QString &relPath, ReplyTag tag, const QString &extra = {});

    void onReplyFinished(QNetworkReply *reply);

    // ── Response parsers ───────────────────────────────────────────────────────
    void handleLotsResponse(const QByteArray &data);
    void handleAllReportsResponse(const QByteArray &data);
    void handleReportsResponse(const QByteArray &data, const QString &lotId);
    void handleUserResponse(const QByteArray &data, const QString &userId);

    // ── Domain ↔ Firestore field converters ───────────────────────────────────
    static QJsonObject lotToFields(const ParkingLot &lot);
    static QJsonObject reportToFields(const LotReport &report);
    static QJsonObject userToFields(const User &user);
    static ParkingLot  fieldsToLot(const QJsonObject &fields, const QString &docName);
    static LotReport   fieldsToReport(const QJsonObject &fields, const QString &docName);
    static User        fieldsToUser(const QJsonObject &fields, const QString &docName);

    // ── Firestore field value helpers ──────────────────────────────────────────
    // Writers
    static QJsonObject sv(const QString &s);       // stringValue
    static QJsonObject iv(int i);                   // integerValue (stored as string per spec)
    static QJsonObject dv(double d);                // doubleValue
    static QJsonObject bv(bool b);                  // booleanValue
    static QJsonObject tv(const QDateTime &dt);     // timestampValue (UTC ISO-8601)
    // Readers
    static QString   rs(const QJsonObject &f, const QString &k); // read string
    static int       ri(const QJsonObject &f, const QString &k); // read integer
    static double    rd(const QJsonObject &f, const QString &k); // read double
    static bool      rb(const QJsonObject &f, const QString &k); // read bool
    static QDateTime rt(const QJsonObject &f, const QString &k); // read timestamp

    // Extracts the bare document ID from a Firestore document "name" path.
    // e.g. "projects/.../documents/lot_reports/abc123" → "abc123"
    static QString docIdFromName(const QString &name);

    QNetworkAccessManager          *m_network;
    QMap<QNetworkReply*, ReplyContext> m_pending;

    static const QString REST_KEY;
    static const QString BASE_URL;
};
