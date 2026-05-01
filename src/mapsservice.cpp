#include "mapsservice.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

const QString MapsService::PLACEHOLDER_API_KEY =
    QStringLiteral("AIzaSyDB0bD3_EOtFn21QQkDCtueCxfCqy1SpoI");

MapsService::MapsService(QObject *parent)
    : QObject(parent)
    , m_network(new QNetworkAccessManager(this))
{
    connect(m_network, &QNetworkAccessManager::finished,
            this, &MapsService::onReplyFinished);
}

void MapsService::searchNearbyParking(double latitude, double longitude, int radiusMeters)
{
    // Places API (New) v1 — Nearby Search
    // POST https://places.googleapis.com/v1/places:searchNearby
    // Docs: https://developers.google.com/maps/documentation/places/web-service/nearby-search
    QNetworkRequest request(
        QUrl(QStringLiteral("https://places.googleapis.com/v1/places:searchNearby")));

    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));
    request.setHeader(QNetworkRequest::UserAgentHeader,   QStringLiteral("LotlyApp/1.0"));
    request.setRawHeader("X-Goog-Api-Key",
                         PLACEHOLDER_API_KEY.toUtf8());
    // Request only the fields we actually use to minimise billing cost.
    request.setRawHeader("X-Goog-FieldMask",
                         "places.id,places.displayName,places.location,"
                         "places.formattedAddress,places.shortFormattedAddress");

    QJsonObject locationObj;
    QJsonObject center;
    center[QStringLiteral("latitude")]  = latitude;
    center[QStringLiteral("longitude")] = longitude;
    locationObj[QStringLiteral("circle")] = QJsonObject{
        {QStringLiteral("center"), center},
        {QStringLiteral("radius"), static_cast<double>(radiusMeters)}
    };

    QJsonObject body;
    body[QStringLiteral("includedTypes")]       = QJsonArray{QStringLiteral("parking")};
    body[QStringLiteral("maxResultCount")]       = 10;
    body[QStringLiteral("locationRestriction")] = locationObj;

    m_network->post(request, QJsonDocument(body).toJson(QJsonDocument::Compact));
}

void MapsService::onReplyFinished(QNetworkReply *reply)
{
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        emit errorOccurred(
            QStringLiteral("Network error: %1").arg(reply->errorString()));
        return;
    }

    const QList<ParkingLot> lots = parsePlacesResponse(reply->readAll());
    if (lots.isEmpty())
        emit errorOccurred(QStringLiteral("No parking lots found near this location."));
    else
        emit lotsReceived(lots);
}

QList<ParkingLot> MapsService::parsePlacesResponse(const QByteArray &data) const
{
    QList<ParkingLot> lots;

    const QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull())
        return lots;

    const QJsonObject root = doc.object();

    // Places API (New) returns an error object on failure instead of a status string.
    if (root.contains(QStringLiteral("error"))) {
        return lots;  // errorOccurred emitted by caller check on empty list
    }

    const QJsonArray places = root.value(QStringLiteral("places")).toArray();

    constexpr int DEFAULT_SPACES = 80;

    for (const QJsonValue &val : places) {
        if (lots.size() >= 10)
            break;

        const QJsonObject place = val.toObject();
        const QJsonObject loc   = place.value(QStringLiteral("location")).toObject();
        const QJsonObject displayName = place.value(QStringLiteral("displayName")).toObject();

        const QString placeId = place.value(QStringLiteral("id")).toString();
        const QString name    = displayName.value(QStringLiteral("text")).toString();
        const double  lat     = loc.value(QStringLiteral("latitude")).toDouble();
        const double  lng     = loc.value(QStringLiteral("longitude")).toDouble();

        // Prefer shortFormattedAddress for display; fall back to full address.
        const QString address = place.value(QStringLiteral("shortFormattedAddress")).toString().isEmpty()
            ? place.value(QStringLiteral("formattedAddress")).toString()
            : place.value(QStringLiteral("shortFormattedAddress")).toString();

        if (name.isEmpty() || placeId.isEmpty())
            continue;

        ParkingLot lot(
            placeId, name, address,
            lat, lng, placeId,
            DEFAULT_SPACES,
            static_cast<int>(DEFAULT_SPACES * 0.50),
            0.35,
            QStringLiteral("No reports yet — showing default availability estimate.")
        );
        lot.setAddress(address);

        lots.append(lot);
    }

    return lots;
}
