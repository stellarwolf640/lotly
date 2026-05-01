#include "appcontroller.h"

#include <QCoreApplication>
#include <QDate>
#include <QGeoPositionInfo>
#include <QGeoPositionInfoSource>
#include <QPermission>
#include <QSignalBlocker>
#include <QTimer>
#include <QtMath>
#include <cmath>

AppController::AppController(QObject *parent)
    : QObject(parent)
    , m_auth(new GoogleAuthService(this))
    , m_databaseManager(&m_firestore, this)
    , m_predictionEngine(&m_dataProcessor)
    , m_recommendationEngine(&m_databaseManager)
    , m_reportManager(&m_databaseManager)
    , m_mapsService(new MapsService(this))
    , m_geocoder(new GeocodingService(this))
    , m_autocomplete(new AutocompleteService(this))
    , m_simulation(new SimulationManager(&m_databaseManager, this))
    , m_debounceTimer(new QTimer(this))
    , m_settingsManager(this)
{
    // ── Autocomplete debounce ─────────────────────────────────────────────────
    m_debounceTimer->setSingleShot(true);
    m_debounceTimer->setInterval(350);
    connect(m_debounceTimer, &QTimer::timeout, this, [this]() {
        if (m_pendingQuery.trimmed().length() >= 2)
            m_autocomplete->fetchSuggestions(m_pendingQuery,
                                             m_hasSearchLocation ? m_searchLat : 0.0,
                                             m_hasSearchLocation ? m_searchLng : 0.0);
        else
            clearAutocomplete();
    });

    // ── Authentication callbacks ──────────────────────────────────────────────
    connect(m_auth, &GoogleAuthService::signInSuccess, this,
            &AppController::onSignInSuccess);

    connect(m_auth, &GoogleAuthService::signInFailed, this,
            [this](const QString &error) {
        qWarning() << "[AppController] Sign-in failed:" << error;
        emit signInFailed(error);   // forward to QML so the spinner can stop
    });

    connect(m_auth, &GoogleAuthService::signedOut,
            this, &AppController::onSignedOut);

    // ── Report-fetch completion → clear data-loading spinner ─────────────────
    connect(&m_databaseManager, &DatabaseManager::reportsReadyChanged, this, [this]() {
        m_dataLoading = false;
        emit dataLoadingChanged();
    });

    // ── Database → UI refresh chain ───────────────────────────────────────────
    connect(&m_databaseManager, &DatabaseManager::dataChanged, this, [this]() {
        refreshLots();
        if (m_selectedLot.contains(QStringLiteral("id"))) {
            const QString id = m_selectedLot.value(QStringLiteral("id")).toString();
            m_selectedLot = lotToVariantMap(m_databaseManager.parkingLotById(id));
            m_selectedLot.insert(QStringLiteral("reportPrompt"),
                                 m_notificationManager.buildReportPrompt(
                                     m_selectedLot.value(QStringLiteral("name")).toString()));
            emit selectedLotChanged();
        }
        refreshRecommendations();
        refreshUserReports();
    });

    // ── Maps API callbacks ────────────────────────────────────────────────────
    connect(m_mapsService, &MapsService::lotsReceived, this,
            [this](const QList<ParkingLot> &lots) {
        {
            const QSignalBlocker blocker(&m_databaseManager);
            m_databaseManager.replaceParkingLots(lots);
        }
        refreshLots();
        refreshRecommendations();
        m_locationLoading = false;
        m_locationStatus  = QStringLiteral("Found %1 parking lots nearby.").arg(lots.size());
        emit locationLoadingChanged();
        emit locationStatusChanged();

        // Kick off async fetch of all report data from Firestore.
        // When it completes: allReportsReceived → dataChanged → refreshLots()
        // will re-run with real report data, and reportsReadyChanged clears the spinner.
        if (!m_databaseManager.reportsLoaded()) {
            m_dataLoading = true;
            emit dataLoadingChanged();
            m_databaseManager.refreshReports();
        }
    });

    connect(m_mapsService, &MapsService::errorOccurred, this,
            [this](const QString &error) {
        m_locationLoading = false;
        m_locationStatus  = error;
        emit locationLoadingChanged();
        emit locationStatusChanged();
        m_lots.clear();
        emit lotsChanged();
    });

    // ── Geocoding callbacks ───────────────────────────────────────────────────
    connect(m_geocoder, &GeocodingService::locationFound, this,
            [this](double lat, double lng, const QString &formattedAddress) {
        m_searchLocationName = formattedAddress;
        emit searchLocationNameChanged();
        setSearchLocation(lat, lng);
    });

    connect(m_geocoder, &GeocodingService::errorOccurred, this,
            [this](const QString &error) {
        m_locationLoading = false;
        m_locationStatus  = QStringLiteral("Address not found: %1").arg(error);
        emit locationLoadingChanged();
        emit locationStatusChanged();
    });

    // ── Autocomplete callbacks ────────────────────────────────────────────────
    connect(m_autocomplete, &AutocompleteService::suggestionsReady, this,
            [this](const QVariantList &suggestions) {
        m_autocompleteSuggestions = suggestions;
        m_autocompleteActive      = !suggestions.isEmpty();
        emit autocompleteSuggestionsChanged();
        emit autocompleteActiveChanged();
    });

    connect(m_autocomplete, &AutocompleteService::errorOccurred, this,
            [this](const QString &) { clearAutocomplete(); });

    // ── If already signed in (persisted session), restore state ───────────────
    if (m_auth->isSignedIn()) {
        const User user = m_auth->currentUser();
        m_databaseManager.setActiveUser(user);
        qDebug() << "[AppController] Restored session — emitting isSignedInChanged (true)";
        emit isSignedInChanged();
        emit currentUserChanged();

        // Queue GPS after event loop starts.
        m_locationStatus = QStringLiteral("Getting your location...");
        emit locationStatusChanged();
        QTimer::singleShot(0, this, &AppController::requestLocationAndSearch);
    }
}

// ── Auth getters ──────────────────────────────────────────────────────────────

bool AppController::isSignedIn() const
{
    return m_auth->isSignedIn();
}

QVariantMap AppController::currentUser() const
{
    return m_auth->currentUserMap();
}

// ── Auth invokables ───────────────────────────────────────────────────────────

void AppController::signIn()
{
    m_auth->signIn();
}

void AppController::signOut()
{
    m_auth->signOut();
}

// ── Auth event handlers ───────────────────────────────────────────────────────

void AppController::onSignInSuccess(const User &user)
{
    m_databaseManager.setActiveUser(user);
    qDebug() << "[AppController] onSignInSuccess — emitting isSignedInChanged (true),"
             << "user:" << user.displayName() << user.email();
    emit isSignedInChanged();
    emit currentUserChanged();
    refreshUserReports();

    // Check if user exists in Firestore; create a document if not.
    connect(&m_firestore, &FirestoreService::userNotFound, this,
            [this, user](const QString &userId) {
        if (userId == user.id())
            m_firestore.storeUser(user);
    }, Qt::SingleShotConnection);

    m_firestore.fetchUser(user.id());

    // Start GPS search now that the user is authenticated.
    m_locationStatus = QStringLiteral("Getting your location...");
    emit locationStatusChanged();
    QTimer::singleShot(0, this, &AppController::requestLocationAndSearch);
}

void AppController::onSignedOut()
{
    // Clear all cached state.
    m_lots.clear();
    m_selectedLot.clear();
    m_recommendation.clear();
    m_userReports.clear();
    m_locationStatus = QString();
    m_locationLoading = false;
    emit lotsChanged();
    emit selectedLotChanged();
    emit recommendationChanged();
    emit userReportsChanged();
    emit locationStatusChanged();
    emit locationLoadingChanged();
    qDebug() << "[AppController] onSignedOut — emitting isSignedInChanged (false)";
    emit isSignedInChanged();
    emit currentUserChanged();
}

// ── Property getters ──────────────────────────────────────────────────────────

QVariantList AppController::lots() const        { return m_lots; }
ParkingLotModel *AppController::parkingLotModel() { return &m_parkingLotModel; }
QVariantMap AppController::selectedLot() const  { return m_selectedLot; }
QVariantMap AppController::recommendation() const { return m_recommendation; }
QString AppController::lastSubmissionMessage() const { return m_lastSubmissionMessage; }
QString AppController::reportError()            const { return m_reportError; }
bool    AppController::dataLoading()            const { return m_dataLoading; }
SimulationManager *AppController::simulationManager() { return m_simulation; }
SettingsManager   *AppController::settings()           { return &m_settingsManager; }
bool AppController::locationLoading() const     { return m_locationLoading; }
QString AppController::locationStatus() const   { return m_locationStatus; }
QString AppController::searchLocationName() const { return m_searchLocationName; }
double  AppController::searchLat()         const { return m_searchLat; }
double  AppController::searchLng()         const { return m_searchLng; }
bool    AppController::hasSearchLocation() const { return m_hasSearchLocation; }
QVariantList AppController::autocompleteSuggestions() const { return m_autocompleteSuggestions; }
bool AppController::autocompleteActive() const { return m_autocompleteActive; }
QVariantList AppController::userReports() const { return m_userReports; }

// ── Invokables ────────────────────────────────────────────────────────────────

void AppController::selectLot(const QString &lotId)
{
    m_selectedLot = lotToVariantMap(m_databaseManager.parkingLotById(lotId));
    m_selectedLot.insert(QStringLiteral("reportPrompt"),
                         m_notificationManager.buildReportPrompt(
                             m_selectedLot.value(QStringLiteral("name")).toString()));
    emit selectedLotChanged();
}

void AppController::submitReport(const QString &lotId, const QString &statusLabel)
{
    const User user = m_databaseManager.activeUser();
    LotReport report(lotId, user.id(), statusFromLabel(statusLabel),
                     user.reliabilityScore());

    QString error;
    if (m_reportManager.submitReport(report, &error)) {
        m_lastSubmissionMessage =
            QStringLiteral("Report submitted: %1 for %2.")
                .arg(report.statusLabel())
                .arg(m_databaseManager.parkingLotById(lotId).name());
        emit lastSubmissionMessageChanged();

        // Clear any previous rate-limit error on success.
        if (!m_reportError.isEmpty()) {
            m_reportError.clear();
            emit reportErrorChanged();
        }
        refreshUserReports();
    } else {
        // Surface the error (e.g. cooldown message) without touching the success banner.
        m_reportError = error;
        emit reportErrorChanged();
    }
}

void AppController::refreshRecommendations()
{
    m_recommendation = m_recommendationEngine.buildRecommendation();
    emit recommendationChanged();
}

void AppController::requestLocationAndSearch()
{
    m_locationLoading = true;
    m_locationStatus  = QStringLiteral("Requesting location access...");
    emit locationLoadingChanged();
    emit locationStatusChanged();

    QLocationPermission locationPermission;
    locationPermission.setAccuracy(QLocationPermission::Precise);

    qApp->requestPermission(locationPermission, this, [this](const QPermission &permission) {
        if (permission.status() == Qt::PermissionStatus::Granted) {
            startGpsSearch();
        } else {
            m_locationLoading = false;
            m_locationStatus  = QStringLiteral(
                "Location permission denied. Tap Search to enter an address manually.");
            emit locationLoadingChanged();
            emit locationStatusChanged();
            if (m_lots.isEmpty())
                emit lotsChanged();
        }
    });
}

void AppController::useDefaultLocation()
{
    requestLocationAndSearch();
}

void AppController::updateAutocompleteQuery(const QString &text)
{
    m_pendingQuery = text;
    m_debounceTimer->stop();

    if (text.trimmed().length() < 2) {
        clearAutocomplete();
        return;
    }
    m_debounceTimer->start();
}

void AppController::selectAutocompleteSuggestion(const QString &placeId,
                                                  const QString &displayText)
{
    clearAutocomplete();
    m_searchLocationName = displayText;
    emit searchLocationNameChanged();

    m_locationLoading = true;
    m_locationStatus  = QStringLiteral("Looking up \"%1\"...").arg(displayText);
    emit locationLoadingChanged();
    emit locationStatusChanged();
    m_geocoder->geocodePlaceId(placeId);
}

void AppController::clearAutocomplete()
{
    if (m_autocompleteSuggestions.isEmpty() && !m_autocompleteActive)
        return;
    m_autocompleteSuggestions.clear();
    m_autocompleteActive = false;
    emit autocompleteSuggestionsChanged();
    emit autocompleteActiveChanged();
}

void AppController::geocodeAddress(const QString &address)
{
    if (address.trimmed().isEmpty())
        return;
    m_locationLoading = true;
    m_locationStatus  = QStringLiteral("Looking up \"%1\"...").arg(address.trimmed());
    emit locationLoadingChanged();
    emit locationStatusChanged();
    m_geocoder->geocodeAddress(address.trimmed());
}

void AppController::setSearchLocation(double latitude, double longitude)
{
    m_searchLat         = latitude;
    m_searchLng         = longitude;
    m_hasSearchLocation = true;
    emit searchLocationChanged();
    m_locationLoading   = true;
    m_locationStatus    = QStringLiteral("Searching nearby parking...");
    emit locationLoadingChanged();
    emit locationStatusChanged();
    m_mapsService->searchNearbyParking(latitude, longitude);
}

void AppController::clearReportError()
{
    if (!m_reportError.isEmpty()) {
        m_reportError.clear();
        emit reportErrorChanged();
    }
}

void AppController::refreshLots()
{
    const QDateTime now  = QDateTime::currentDateTime();
    const int       hour = now.time().hour();

    QList<ParkingLot> updatedLots = m_databaseManager.parkingLots();

    for (ParkingLot &lot : updatedLots) {
        const QList<LotReport> reports    = m_databaseManager.reportsForLot(lot.id());
        const double           timeWeight = m_dataProcessor.totalTimeWeight(reports, hour, now);

        lot.setPredictedAvailableSpaces(m_predictionEngine.predictAvailableSpaces(lot, reports));
        lot.setConfidence(m_confidenceCalculator.calculateConfidence(timeWeight));
        lot.setExplanation(m_predictionEngine.buildExplanation(lot, reports));

        if (m_hasSearchLocation && (lot.latitude() != 0.0 || lot.longitude() != 0.0))
            lot.setDistanceMeters(haversineDistance(m_searchLat, m_searchLng,
                                                    lot.latitude(), lot.longitude()));
    }

    {
        const QSignalBlocker blocker(&m_databaseManager);
        m_databaseManager.replaceParkingLots(updatedLots);
    }

    m_parkingLotModel.setLots(updatedLots);

    const QVariantMap rec = m_recommendationEngine.buildRecommendation();
    const QString bestId  = rec.value(QStringLiteral("bestLotId")).toString();

    m_lots.clear();
    for (const ParkingLot &lot : updatedLots) {
        QVariantMap vm = lotToVariantMap(lot);
        vm.insert(QStringLiteral("bestPick"), lot.id() == bestId);
        m_lots.append(vm);
    }

    emit lotsChanged();
}

// ── Private helpers ───────────────────────────────────────────────────────────

void AppController::refreshUserReports()
{
    const QString userId = m_databaseManager.activeUser().id();
    if (userId.isEmpty()) {
        if (!m_userReports.isEmpty()) {
            m_userReports.clear();
            emit userReportsChanged();
        }
        return;
    }

    const QList<LotReport> reports = m_databaseManager.reportsForUser(userId);
    const QDate today = QDate::currentDate();
    QVariantList result;

    for (const LotReport &r : reports) {
        const QDateTime ts  = r.timestamp();
        const QDate     day = ts.date();

        QString tsStr;
        if (day == today)
            tsStr = QStringLiteral("Today, ") + ts.toString(QStringLiteral("h:mm AP"));
        else if (day == today.addDays(-1))
            tsStr = QStringLiteral("Yesterday, ") + ts.toString(QStringLiteral("h:mm AP"));
        else
            tsStr = ts.toString(QStringLiteral("MMM d, h:mm AP"));

        QString statusStr;
        switch (r.status()) {
            case LotReport::Status::Empty:      statusStr = QStringLiteral("Empty"); break;
            case LotReport::Status::Moderate:   statusStr = QStringLiteral("Half");  break;
            case LotReport::Status::NearlyFull: statusStr = QStringLiteral("Busy");  break;
            case LotReport::Status::Full:       statusStr = QStringLiteral("Full");  break;
        }

        result.append(QVariantMap{
            {QStringLiteral("lotId"),     r.lotId()},
            {QStringLiteral("lotName"),   m_databaseManager.parkingLotById(r.lotId()).name()},
            {QStringLiteral("timestamp"), tsStr},
            {QStringLiteral("status"),    statusStr},
        });
    }

    m_userReports = result;
    emit userReportsChanged();
}

void AppController::startGpsSearch()
{
    if (!m_gpsSource) {
        m_gpsSource = QGeoPositionInfoSource::createDefaultSource(this);

        if (!m_gpsSource) {
            m_locationLoading = false;
            m_locationStatus  = QStringLiteral(
                "GPS not available on this device. Tap Search to enter an address manually.");
            emit locationLoadingChanged();
            emit locationStatusChanged();
            if (m_lots.isEmpty())
                emit lotsChanged();
            return;
        }

        m_gpsSource->setPreferredPositioningMethods(
            QGeoPositionInfoSource::AllPositioningMethods);

        connect(m_gpsSource, &QGeoPositionInfoSource::positionUpdated, this,
                [this](const QGeoPositionInfo &info) {
            m_gpsSource->stopUpdates();
            setSearchLocation(info.coordinate().latitude(),
                              info.coordinate().longitude());
        });

        connect(m_gpsSource, &QGeoPositionInfoSource::errorOccurred, this,
                [this](QGeoPositionInfoSource::Error) {
            m_locationLoading = false;
            m_locationStatus  = QStringLiteral(
                "Could not get GPS location. Tap Search to enter an address manually.");
            emit locationLoadingChanged();
            emit locationStatusChanged();
            if (m_lots.isEmpty())
                emit lotsChanged();
        });
    }

    m_locationLoading = true;
    m_locationStatus  = QStringLiteral("Getting your location...");
    emit locationLoadingChanged();
    emit locationStatusChanged();

    m_gpsSource->requestUpdate(20000);
}

double AppController::haversineDistance(double lat1, double lon1,
                                        double lat2, double lon2)
{
    constexpr double R = 6371000.0;
    const double dLat = qDegreesToRadians(lat2 - lat1);
    const double dLon = qDegreesToRadians(lon2 - lon1);
    const double a = std::sin(dLat / 2) * std::sin(dLat / 2)
                   + std::cos(qDegreesToRadians(lat1)) * std::cos(qDegreesToRadians(lat2))
                   * std::sin(dLon / 2) * std::sin(dLon / 2);
    return R * 2.0 * std::atan2(std::sqrt(a), std::sqrt(1.0 - a));
}

LotReport::Status AppController::statusFromLabel(const QString &label) const
{
    if (label.compare(QStringLiteral("Empty"),       Qt::CaseInsensitive) == 0)
        return LotReport::Status::Empty;
    if (label.compare(QStringLiteral("Nearly Full"), Qt::CaseInsensitive) == 0)
        return LotReport::Status::NearlyFull;
    if (label.compare(QStringLiteral("Sparse"),      Qt::CaseInsensitive) == 0)
        return LotReport::Status::Empty;
    if (label.compare(QStringLiteral("Busy"),        Qt::CaseInsensitive) == 0)
        return LotReport::Status::NearlyFull;
    if (label.compare(QStringLiteral("Half"),        Qt::CaseInsensitive) == 0)
        return LotReport::Status::Moderate;
    if (label.compare(QStringLiteral("Full"),        Qt::CaseInsensitive) == 0)
        return LotReport::Status::Full;
    return LotReport::Status::Moderate;
}

QVariantMap AppController::lotToVariantMap(const ParkingLot &lot) const
{
    const int pct = lot.totalSpaces() > 0
        ? qRound(static_cast<double>(lot.predictedAvailableSpaces())
                 / lot.totalSpaces() * 100.0)
        : 0;

    QString confLabel;
    if (lot.confidence() >= 0.75)      confLabel = QStringLiteral("High");
    else if (lot.confidence() >= 0.55) confLabel = QStringLiteral("Medium");
    else                               confLabel = QStringLiteral("Low");

    QString statusColor;
    if (pct >= 55)      statusColor = QStringLiteral("#2D6DCC");
    else if (pct >= 30) statusColor = QStringLiteral("#D6A51D");
    else                statusColor = QStringLiteral("#D5523F");

    const QList<LotReport> reports      = m_databaseManager.reportsForLot(lot.id());
    const int              recentReports = reports.size();

    // 21 hourly availability values: hour 6 AM (index 0) through 2 AM (index 20).
    // Uses real time-weighted report data blended with the fallback curve.
    const QVariantList hourlyAvailability =
        m_predictionEngine.buildHourlyPredictions(lot, reports);

    return {
        {QStringLiteral("id"),                      lot.id()},
        {QStringLiteral("name"),                    lot.name()},
        {QStringLiteral("location"),                lot.location()},
        {QStringLiteral("address"),                 lot.address().isEmpty() ? lot.location() : lot.address()},
        {QStringLiteral("latitude"),                lot.latitude()},
        {QStringLiteral("longitude"),               lot.longitude()},
        {QStringLiteral("placeId"),                 lot.placeId()},
        {QStringLiteral("totalSpaces"),             lot.totalSpaces()},
        {QStringLiteral("predictedAvailableSpaces"),lot.predictedAvailableSpaces()},
        {QStringLiteral("percentOpen"),             pct},
        {QStringLiteral("confidence"),              lot.confidence()},
        {QStringLiteral("confidenceLabel"),         confLabel},
        {QStringLiteral("confidenceScore"),         lot.confidence()},
        {QStringLiteral("statusColor"),             statusColor},
        {QStringLiteral("explanation"),             lot.explanation()},
        {QStringLiteral("recentReports"),           recentReports},
        {QStringLiteral("distanceMeters"),          lot.distanceMeters()},
        {QStringLiteral("hourlyAvailability"),      hourlyAvailability},
        {QStringLiteral("bestPick"),                false},
        {QStringLiteral("reportPrompt"),            QString()}
    };
}
