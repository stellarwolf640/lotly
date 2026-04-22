#pragma once

#include <QObject>
#include <QString>

class QNetworkAccessManager;
class QNetworkReply;

// GeocodingService converts a human-readable address string into geographic
// coordinates via the Google Geocoding API.
//
// Requires the same API key used for MapsService. Set it in mapsservice.cpp
// (PLACEHOLDER_API_KEY) — this service shares that key.
class GeocodingService : public QObject
{
    Q_OBJECT

public:
    explicit GeocodingService(QObject *parent = nullptr);

    // Asynchronously geocode a free-text address string.
    void geocodeAddress(const QString &address);

    // Resolve a Google place_id to coordinates (used after autocomplete selection).
    void geocodePlaceId(const QString &placeId);

signals:
    void locationFound(double latitude, double longitude, const QString &formattedAddress);
    void errorOccurred(const QString &message);

private slots:
    void onReplyFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager *m_network;
};
