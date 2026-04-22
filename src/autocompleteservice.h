#pragma once

#include <QObject>
#include <QVariantList>

class QNetworkAccessManager;
class QNetworkReply;

// AutocompleteService queries the Places API (New) autocomplete endpoint as the
// user types. Each suggestion carries enough data to populate the search field
// and, after selection, to geocode to coordinates via GeocodingService.
//
// Uses the same API key as MapsService (MapsService::PLACEHOLDER_API_KEY).
// Requires "Places API (New)" to be enabled in Google Cloud Console.
class AutocompleteService : public QObject
{
    Q_OBJECT

public:
    explicit AutocompleteService(QObject *parent = nullptr);

    // Fires an async autocomplete request.
    // biasLat / biasLng bias results toward the current search area (pass 0,0 to skip).
    void fetchSuggestions(const QString &input,
                          double biasLat          = 0.0,
                          double biasLng          = 0.0,
                          double biasRadiusMeters = 50000.0);

signals:
    // Emitted when results arrive. Each QVariantMap in the list has keys:
    //   "placeId"       — Google place_id, used for geocoding on selection
    //   "mainText"      — primary display text  (e.g. "Cal Poly")
    //   "secondaryText" — secondary label       (e.g. "San Luis Obispo, CA")
    //   "fullText"      — combined text, use to fill the search field
    void suggestionsReady(const QVariantList &suggestions);
    void errorOccurred(const QString &message);

private slots:
    void onReplyFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager *m_network;
};
