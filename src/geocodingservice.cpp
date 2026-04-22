#include "geocodingservice.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QUrlQuery>

#include "mapsservice.h"

GeocodingService::GeocodingService(QObject *parent)
    : QObject(parent)
    , m_network(new QNetworkAccessManager(this))
{
    connect(m_network, &QNetworkAccessManager::finished,
            this, &GeocodingService::onReplyFinished);
}

void GeocodingService::geocodeAddress(const QString &address)
{
    QUrl url(QStringLiteral("https://maps.googleapis.com/maps/api/geocode/json"));
    QUrlQuery q;
    q.addQueryItem(QStringLiteral("address"), address);
    q.addQueryItem(QStringLiteral("key"), MapsService::PLACEHOLDER_API_KEY);
    url.setQuery(q);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, QStringLiteral("LotlyApp/1.0"));
    m_network->get(request);
}

void GeocodingService::onReplyFinished(QNetworkReply *reply)
{
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        emit errorOccurred(
            QStringLiteral("Network error: %1").arg(reply->errorString()));
        return;
    }

    const QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    if (doc.isNull()) {
        emit errorOccurred(QStringLiteral("Invalid response from geocoding service."));
        return;
    }

    const QJsonObject root = doc.object();
    const QString status = root.value(QStringLiteral("status")).toString();

    if (status != QStringLiteral("OK")) {
        emit errorOccurred(
            QStringLiteral("Geocoding failed: %1").arg(status));
        return;
    }

    const QJsonArray results = root.value(QStringLiteral("results")).toArray();
    if (results.isEmpty()) {
        emit errorOccurred(QStringLiteral("No results found for that address."));
        return;
    }

    const QJsonObject first  = results.at(0).toObject();
    const QJsonObject geom   = first.value(QStringLiteral("geometry")).toObject();
    const QJsonObject loc    = geom.value(QStringLiteral("location")).toObject();

    const double lat              = loc.value(QStringLiteral("lat")).toDouble();
    const double lng              = loc.value(QStringLiteral("lng")).toDouble();
    const QString formattedAddress = first.value(QStringLiteral("formatted_address")).toString();

    emit locationFound(lat, lng, formattedAddress);
}

void GeocodingService::geocodePlaceId(const QString &placeId)
{
    QUrl url(QStringLiteral("https://maps.googleapis.com/maps/api/geocode/json"));
    QUrlQuery q;
    q.addQueryItem(QStringLiteral("place_id"), placeId);
    q.addQueryItem(QStringLiteral("key"), MapsService::PLACEHOLDER_API_KEY);
    url.setQuery(q);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, QStringLiteral("LotlyApp/1.0"));
    m_network->get(request);
    // Response handled by the same onReplyFinished slot — emits locationFound()
}
