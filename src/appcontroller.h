#pragma once

#include <QObject>
#include <QTimer>
#include <QVariantList>
#include <QVariantMap>

#include "autocompleteservice.h"
#include "confidencecalculator.h"
#include "databasemanager.h"
#include "dataprocessor.h"
#include "firestoreservice.h"
#include "geocodingservice.h"
#include "googleauthservice.h"
#include "mapsservice.h"
#include "notificationmanager.h"
#include "parkinglotmodel.h"
#include "predictionengine.h"
#include "recommendationengine.h"
#include "reportmanager.h"
#include "settingsmanager.h"
#include "simulationmanager.h"

class QGeoPositionInfoSource;

class AppController : public QObject
{
    Q_OBJECT

    // ── Authentication ────────────────────────────────────────────────────────
    Q_PROPERTY(bool        isSignedIn  READ isSignedIn  NOTIFY isSignedInChanged)
    Q_PROPERTY(QVariantMap currentUser READ currentUser NOTIFY currentUserChanged)

    // ── Live lot list ─────────────────────────────────────────────────────────
    Q_PROPERTY(QVariantList lots READ lots NOTIFY lotsChanged)
    Q_PROPERTY(ParkingLotModel *parkingLotModel READ parkingLotModel CONSTANT)
    Q_PROPERTY(QVariantMap selectedLot READ selectedLot NOTIFY selectedLotChanged)
    Q_PROPERTY(QVariantMap recommendation READ recommendation NOTIFY recommendationChanged)
    Q_PROPERTY(QString lastSubmissionMessage READ lastSubmissionMessage
               NOTIFY lastSubmissionMessageChanged)
    Q_PROPERTY(QString reportError READ reportError NOTIFY reportErrorChanged)

    // ── Dev tools ─────────────────────────────────────────────────────────────
    Q_PROPERTY(SimulationManager *simulationManager READ simulationManager CONSTANT)

    // ── Settings ──────────────────────────────────────────────────────────────
    Q_PROPERTY(SettingsManager *settings READ settings CONSTANT)

    // ── Data loading state ────────────────────────────────────────────────────
    // True while the initial Firestore report fetch is in flight.
    // QML can use this to show a loading indicator on the chart.
    Q_PROPERTY(bool dataLoading READ dataLoading NOTIFY dataLoadingChanged)

    // ── Location state ────────────────────────────────────────────────────────
    Q_PROPERTY(bool   locationLoading    READ locationLoading    NOTIFY locationLoadingChanged)
    Q_PROPERTY(QString locationStatus    READ locationStatus     NOTIFY locationStatusChanged)
    Q_PROPERTY(QString searchLocationName READ searchLocationName
               NOTIFY searchLocationNameChanged)
    Q_PROPERTY(double  searchLat         READ searchLat          NOTIFY searchLocationChanged)
    Q_PROPERTY(double  searchLng         READ searchLng          NOTIFY searchLocationChanged)
    Q_PROPERTY(bool    hasSearchLocation READ hasSearchLocation  NOTIFY searchLocationChanged)

    // ── Autocomplete ──────────────────────────────────────────────────────────
    Q_PROPERTY(QVariantList autocompleteSuggestions READ autocompleteSuggestions
               NOTIFY autocompleteSuggestionsChanged)
    Q_PROPERTY(bool autocompleteActive READ autocompleteActive
               NOTIFY autocompleteActiveChanged)

    // ── User report history ───────────────────────────────────────────────────
    // Sorted newest-first. Each entry is a QVariantMap with keys:
    //   "lotId"     : QString
    //   "lotName"   : QString  (display name; empty if lot not in cache)
    //   "timestamp" : QString  (e.g. "Today, 9:14 AM" / "Yesterday, 3:22 PM")
    //   "status"    : QString  ("Empty" | "Half" | "Busy" | "Full")
    Q_PROPERTY(QVariantList userReports READ userReports NOTIFY userReportsChanged)

public:
    explicit AppController(QObject *parent = nullptr);

    // ── Auth getters ──────────────────────────────────────────────────────────
    bool        isSignedIn()  const;
    QVariantMap currentUser() const;

    // ── Existing getters ──────────────────────────────────────────────────────
    QVariantList lots() const;
    ParkingLotModel *parkingLotModel();
    QVariantMap selectedLot() const;
    QVariantMap recommendation() const;
    QString lastSubmissionMessage() const;
    QString reportError() const;
    bool    dataLoading() const;
    SimulationManager *simulationManager();
    SettingsManager   *settings();
    bool locationLoading() const;
    QString locationStatus() const;
    QString searchLocationName() const;
    double  searchLat()         const;
    double  searchLng()         const;
    bool    hasSearchLocation() const;
    QVariantList autocompleteSuggestions() const;
    bool autocompleteActive() const;
    QVariantList userReports() const;

    // ── Auth invokables ───────────────────────────────────────────────────────
    Q_INVOKABLE void signIn();
    Q_INVOKABLE void signOut();

    // ── Existing invokables ───────────────────────────────────────────────────
    Q_INVOKABLE void selectLot(const QString &lotId);
    Q_INVOKABLE void submitReport(const QString &lotId, const QString &statusLabel);
    Q_INVOKABLE void refreshRecommendations();
    Q_INVOKABLE void geocodeAddress(const QString &address);
    Q_INVOKABLE void updateAutocompleteQuery(const QString &text);
    Q_INVOKABLE void selectAutocompleteSuggestion(const QString &placeId,
                                                  const QString &displayText);
    Q_INVOKABLE void clearAutocomplete();
    Q_INVOKABLE void setSearchLocation(double latitude, double longitude);
    Q_INVOKABLE void requestLocationAndSearch();
    Q_INVOKABLE void useDefaultLocation();
    Q_INVOKABLE void refreshLots();
    Q_INVOKABLE void clearReportError();

signals:
    // Auth
    void isSignedInChanged();
    void currentUserChanged();
    void signInFailed(const QString &error);   // forwarded from GoogleAuthService

    // Existing
    void lotsChanged();
    void selectedLotChanged();
    void recommendationChanged();
    void lastSubmissionMessageChanged();
    void reportErrorChanged();
    void dataLoadingChanged();
    void locationLoadingChanged();
    void locationStatusChanged();
    void searchLocationNameChanged();
    void searchLocationChanged();
    void autocompleteSuggestionsChanged();
    void autocompleteActiveChanged();
    void userReportsChanged();

private:
    LotReport::Status statusFromLabel(const QString &label) const;
    QVariantMap lotToVariantMap(const ParkingLot &lot) const;
    void startGpsSearch();
    void refreshUserReports();
    static double haversineDistance(double lat1, double lon1, double lat2, double lon2);

    void onSignInSuccess(const User &user);
    void onSignedOut();

    // ── Owned services (order matters for init) ───────────────────────────────
    FirestoreService         m_firestore;
    GoogleAuthService       *m_auth           = nullptr;
    DatabaseManager          m_databaseManager;   // constructed after m_firestore
    DataProcessor            m_dataProcessor;
    PredictionEngine         m_predictionEngine;
    ConfidenceCalculator     m_confidenceCalculator;
    RecommendationEngine     m_recommendationEngine;
    NotificationManager      m_notificationManager;
    ReportManager            m_reportManager;
    MapsService             *m_mapsService    = nullptr;
    GeocodingService        *m_geocoder       = nullptr;
    AutocompleteService     *m_autocomplete   = nullptr;
    SimulationManager       *m_simulation     = nullptr;
    QGeoPositionInfoSource  *m_gpsSource      = nullptr;
    QTimer                  *m_debounceTimer  = nullptr;
    QString                  m_pendingQuery;
    ParkingLotModel          m_parkingLotModel;
    SettingsManager          m_settingsManager;

    // ── State ─────────────────────────────────────────────────────────────────
    QVariantList m_lots;
    QVariantMap  m_selectedLot;
    QVariantMap  m_recommendation;
    QString      m_lastSubmissionMessage;
    QString      m_reportError;
    bool         m_dataLoading        = false;
    bool         m_locationLoading    = false;
    QString      m_locationStatus;
    QString      m_searchLocationName;
    double       m_searchLat          = 0.0;
    double       m_searchLng          = 0.0;
    bool         m_hasSearchLocation  = false;
    QVariantList m_autocompleteSuggestions;
    bool         m_autocompleteActive = false;
    QVariantList m_userReports;
};
