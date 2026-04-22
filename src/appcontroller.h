#pragma once

#include <QObject>
#include <QTimer>
#include <QVariantList>
#include <QVariantMap>

#include "confidencecalculator.h"
#include "databasemanager.h"
#include "dataprocessor.h"
#include "autocompleteservice.h"
#include "geocodingservice.h"
#include "mapsservice.h"
#include "notificationmanager.h"
#include "parkinglotmodel.h"
#include "predictionengine.h"
#include "recommendationengine.h"
#include "reportmanager.h"
#include "simulationmanager.h"

class AppController : public QObject
{
    Q_OBJECT

    // Live lot list — QML binds to this instead of static mock JS data
    Q_PROPERTY(QVariantList lots READ lots NOTIFY lotsChanged)
    Q_PROPERTY(ParkingLotModel *parkingLotModel READ parkingLotModel CONSTANT)
    Q_PROPERTY(QVariantMap selectedLot READ selectedLot NOTIFY selectedLotChanged)
    Q_PROPERTY(QVariantMap recommendation READ recommendation NOTIFY recommendationChanged)
    Q_PROPERTY(QString lastSubmissionMessage READ lastSubmissionMessage NOTIFY lastSubmissionMessageChanged)

    // Dev tools
    Q_PROPERTY(SimulationManager *simulationManager READ simulationManager CONSTANT)

    // Location state
    Q_PROPERTY(bool locationLoading READ locationLoading NOTIFY locationLoadingChanged)
    Q_PROPERTY(QString locationStatus READ locationStatus NOTIFY locationStatusChanged)
    Q_PROPERTY(QString searchLocationName READ searchLocationName NOTIFY searchLocationNameChanged)
    Q_PROPERTY(double  searchLat          READ searchLat          NOTIFY searchLocationChanged)
    Q_PROPERTY(double  searchLng          READ searchLng          NOTIFY searchLocationChanged)
    Q_PROPERTY(bool    hasSearchLocation  READ hasSearchLocation  NOTIFY searchLocationChanged)

    // Autocomplete
    Q_PROPERTY(QVariantList autocompleteSuggestions READ autocompleteSuggestions NOTIFY autocompleteSuggestionsChanged)
    Q_PROPERTY(bool autocompleteActive READ autocompleteActive NOTIFY autocompleteActiveChanged)

public:
    explicit AppController(QObject *parent = nullptr);

    QVariantList lots() const;
    ParkingLotModel *parkingLotModel();
    QVariantMap selectedLot() const;
    QVariantMap recommendation() const;
    QString lastSubmissionMessage() const;
    SimulationManager *simulationManager();
    bool locationLoading() const;
    QString locationStatus() const;
    QString searchLocationName() const;
    double  searchLat()         const;
    double  searchLng()         const;
    bool    hasSearchLocation() const;
    QVariantList autocompleteSuggestions() const;
    bool autocompleteActive() const;

    Q_INVOKABLE void selectLot(const QString &lotId);
    Q_INVOKABLE void submitReport(const QString &lotId, const QString &statusLabel);
    Q_INVOKABLE void refreshRecommendations();

    // Geocode an address string and then search for nearby parking.
    Q_INVOKABLE void geocodeAddress(const QString &address);

    // Autocomplete: call on every keystroke (debounced internally, 350 ms).
    Q_INVOKABLE void updateAutocompleteQuery(const QString &text);

    // Called when the user taps a suggestion from the dropdown.
    Q_INVOKABLE void selectAutocompleteSuggestion(const QString &placeId,
                                                  const QString &displayText);

    // Clear the suggestion list (e.g. on popup close / cancel).
    Q_INVOKABLE void clearAutocomplete();

    // Fetch real parking lots from Google Places near the given coordinates.
    // Falls back to default lots on network/API error.
    Q_INVOKABLE void setSearchLocation(double latitude, double longitude);

    // Load the built-in default campus lots (no network required).
    Q_INVOKABLE void useDefaultLocation();

    // Force-refresh all lot predictions from the current report data.
    Q_INVOKABLE void refreshLots();

signals:
    void lotsChanged();
    void selectedLotChanged();
    void recommendationChanged();
    void lastSubmissionMessageChanged();
    void locationLoadingChanged();
    void locationStatusChanged();
    void searchLocationNameChanged();
    void searchLocationChanged();      // covers lat, lng, hasSearchLocation
    void autocompleteSuggestionsChanged();
    void autocompleteActiveChanged();

private:
    LotReport::Status statusFromLabel(const QString &label) const;
    QVariantMap lotToVariantMap(const ParkingLot &lot) const;
    void loadDefaultLots();
    static double haversineDistance(double lat1, double lon1, double lat2, double lon2);

    DatabaseManager      m_databaseManager;
    DataProcessor        m_dataProcessor;
    PredictionEngine     m_predictionEngine;
    ConfidenceCalculator m_confidenceCalculator;
    RecommendationEngine m_recommendationEngine;
    NotificationManager  m_notificationManager;
    ReportManager        m_reportManager;
    MapsService          *m_mapsService     = nullptr;
    GeocodingService     *m_geocoder        = nullptr;
    AutocompleteService  *m_autocomplete    = nullptr;
    SimulationManager    *m_simulation      = nullptr;
    QTimer               *m_debounceTimer   = nullptr;
    QString               m_pendingQuery;
    ParkingLotModel      m_parkingLotModel;

    QVariantList m_lots;
    QVariantMap  m_selectedLot;
    QVariantMap  m_recommendation;
    QString      m_lastSubmissionMessage;
    bool         m_locationLoading     = false;
    QString      m_locationStatus;
    QString      m_searchLocationName;
    double       m_searchLat           = 0.0;
    double       m_searchLng           = 0.0;
    bool         m_hasSearchLocation   = false;
    QVariantList m_autocompleteSuggestions;
    bool         m_autocompleteActive  = false;
};
