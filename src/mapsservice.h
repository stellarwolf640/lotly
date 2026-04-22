#pragma once

#include <QList>
#include <QObject>

#include "parkinglot.h"

class QNetworkAccessManager;
class QNetworkReply;

// MapsService queries the Google Places Nearby Search API to find real parking
// locations near a given coordinate pair and converts them into ParkingLot objects.
//
// To enable live results:
//   1. Obtain an API key from https://console.cloud.google.com/
//   2. Enable "Places API" for the project.
//   3. Replace PLACEHOLDER_API_KEY with your key (or load it from a config file).
//
// Without a valid key the network request will return REQUEST_DENIED.
// The app degrades gracefully by keeping the default lots loaded.
class MapsService : public QObject
{
    Q_OBJECT

public:
    // Replace with a real key before shipping.
    // Never commit real keys to source control — load from env or secure storage.
    static const QString PLACEHOLDER_API_KEY;

    explicit MapsService(QObject *parent = nullptr);

    // Fires an async Nearby Search request using the Places API (New) v1.
    // Default radius is ~1 mile (1610 m); results are capped at 10 lots.
    void searchNearbyParking(double latitude, double longitude, int radiusMeters = 1610);

signals:
    void lotsReceived(const QList<ParkingLot> &lots);
    void errorOccurred(const QString &message);

private slots:
    void onReplyFinished(QNetworkReply *reply);

private:
    QList<ParkingLot> parsePlacesResponse(const QByteArray &data) const;

    QNetworkAccessManager *m_network;
};
