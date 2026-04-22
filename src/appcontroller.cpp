#include "appcontroller.h"

#include <QSignalBlocker>
#include <QTimer>
#include <QtMath>
#include <cmath>

AppController::AppController(QObject *parent)
    : QObject(parent)
    , m_predictionEngine(&m_dataProcessor)
    , m_recommendationEngine(&m_databaseManager)
    , m_reportManager(&m_databaseManager)
    , m_mapsService(new MapsService(this))
    , m_geocoder(new GeocodingService(this))
    , m_autocomplete(new AutocompleteService(this))
    , m_simulation(new SimulationManager(&m_databaseManager, this))
    , m_debounceTimer(new QTimer(this))
{
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
    // ── Database → UI refresh chain ──────────────────────────────────────────
    connect(&m_databaseManager, &DatabaseManager::dataChanged, this, [this]() {
        refreshLots();
        // Keep selected lot in sync if it exists
        if (m_selectedLot.contains(QStringLiteral("id"))) {
            const QString id = m_selectedLot.value(QStringLiteral("id")).toString();
            m_selectedLot = lotToVariantMap(m_databaseManager.parkingLotById(id));
            m_selectedLot.insert(QStringLiteral("reportPrompt"),
                                 m_notificationManager.buildReportPrompt(
                                     m_selectedLot.value(QStringLiteral("name")).toString()));
            emit selectedLotChanged();
        }
        refreshRecommendations();
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
        m_locationStatus  = QStringLiteral("Loaded %1 lots nearby.").arg(lots.size());
        emit locationLoadingChanged();
        emit locationStatusChanged();
    });

    connect(m_mapsService, &MapsService::errorOccurred, this,
            [this](const QString &error) {
        m_locationLoading = false;
        m_locationStatus  = QStringLiteral("Location error: %1").arg(error);
        emit locationLoadingChanged();
        emit locationStatusChanged();
    });

    // ── Geocoding callbacks ───────────────────────────────────────────────────
    connect(m_geocoder, &GeocodingService::locationFound, this,
            [this](double lat, double lng, const QString &formattedAddress) {
        m_searchLocationName = formattedAddress;
        emit searchLocationNameChanged();
        // Continue the flow: search for parking at the resolved coordinates
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
            [this](const QString &) {
        // Silently clear on error — don't block the user with an error message
        clearAutocomplete();
    });

    // ── Initial load ──────────────────────────────────────────────────────────
    loadDefaultLots();
    m_locationStatus = QStringLiteral("Default campus location loaded.");
    emit locationStatusChanged();
}

// ── Property getters ─────────────────────────────────────────────────────────

QVariantList AppController::lots() const        { return m_lots; }
ParkingLotModel *AppController::parkingLotModel() { return &m_parkingLotModel; }
QVariantMap AppController::selectedLot() const  { return m_selectedLot; }
QVariantMap AppController::recommendation() const { return m_recommendation; }
QString AppController::lastSubmissionMessage() const { return m_lastSubmissionMessage; }
SimulationManager *AppController::simulationManager() { return m_simulation; }
bool AppController::locationLoading() const     { return m_locationLoading; }
QString AppController::locationStatus() const   { return m_locationStatus; }
QString AppController::searchLocationName() const { return m_searchLocationName; }
double  AppController::searchLat()         const { return m_searchLat; }
double  AppController::searchLng()         const { return m_searchLng; }
bool    AppController::hasSearchLocation() const { return m_hasSearchLocation; }
QVariantList AppController::autocompleteSuggestions() const { return m_autocompleteSuggestions; }
bool AppController::autocompleteActive() const { return m_autocompleteActive; }

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
    } else {
        m_lastSubmissionMessage = error;
    }
    emit lastSubmissionMessageChanged();
}

void AppController::refreshRecommendations()
{
    m_recommendation = m_recommendationEngine.buildRecommendation();
    emit recommendationChanged();
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

    // Resolve place_id → coordinates, then trigger nearby search
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
    m_searchLat          = latitude;
    m_searchLng          = longitude;
    m_hasSearchLocation  = true;
    emit searchLocationChanged();
    m_locationLoading    = true;
    m_locationStatus     = QStringLiteral("Searching nearby parking...");
    emit locationLoadingChanged();
    emit locationStatusChanged();
    m_mapsService->searchNearbyParking(latitude, longitude);
}

void AppController::useDefaultLocation()
{
    m_hasSearchLocation  = false;
    m_searchLocationName = QString();
    emit searchLocationNameChanged();
    emit searchLocationChanged();
    loadDefaultLots();
    m_locationStatus = QStringLiteral("Default campus location loaded.");
    emit locationStatusChanged();
}

// ── Private helpers ───────────────────────────────────────────────────────────

double AppController::haversineDistance(double lat1, double lon1,
                                        double lat2, double lon2)
{
    constexpr double R = 6371000.0;
    const double dLat = qDegreesToRadians(lat2 - lat1);
    const double dLon = qDegreesToRadians(lon2 - lon1);
    const double a = std::sin(dLat / 2) * std::sin(dLat / 2)
                   + std::cos(qDegreesToRadians(lat1)) * std::cos(qDegreesToRadians(lat2))
                   * std::sin(dLon / 2) * std::sin(dLon / 2);
    const double c = 2.0 * std::atan2(std::sqrt(a), std::sqrt(1.0 - a));
    return R * c;
}

LotReport::Status AppController::statusFromLabel(const QString &label) const
{
    if (label.compare(QStringLiteral("Empty"),       Qt::CaseInsensitive) == 0)
        return LotReport::Status::Empty;
    if (label.compare(QStringLiteral("Nearly Full"), Qt::CaseInsensitive) == 0)
        return LotReport::Status::NearlyFull;
    if (label.compare(QStringLiteral("Sparse"),      Qt::CaseInsensitive) == 0)
        return LotReport::Status::Empty;   // map UI "Sparse" → closest enum
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
    // Compute derived display fields so QML cards don't need backend logic
    const int pct = lot.totalSpaces() > 0
        ? qRound(static_cast<double>(lot.predictedAvailableSpaces())
                 / lot.totalSpaces() * 100.0)
        : 0;

    QString confLabel;
    if (lot.confidence() >= 0.75)      confLabel = QStringLiteral("High");
    else if (lot.confidence() >= 0.55) confLabel = QStringLiteral("Medium");
    else                               confLabel = QStringLiteral("Low");

    QString statusColor;
    if (pct >= 55)      statusColor = QStringLiteral("#2D6DCC");  // blue  — high
    else if (pct >= 30) statusColor = QStringLiteral("#D6A51D");  // amber — medium
    else                statusColor = QStringLiteral("#D5523F");  // red   — low

    const int recentReports = m_databaseManager.reportsForLot(lot.id()).size();

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
        {QStringLiteral("bestPick"),                false},
        {QStringLiteral("reportPrompt"),            QString()}
    };
}

void AppController::refreshLots()
{
    QList<ParkingLot> updatedLots = m_databaseManager.parkingLots();

    for (ParkingLot &lot : updatedLots) {
        const QList<LotReport> reports = m_databaseManager.reportsForLot(lot.id());
        lot.setPredictedAvailableSpaces(m_predictionEngine.predictAvailableSpaces(lot, reports));
        lot.setConfidence(m_confidenceCalculator.calculateConfidence(reports));
        lot.setExplanation(m_predictionEngine.buildExplanation(lot, reports));

        if (m_hasSearchLocation && (lot.latitude() != 0.0 || lot.longitude() != 0.0))
            lot.setDistanceMeters(haversineDistance(m_searchLat, m_searchLng,
                                                    lot.latitude(), lot.longitude()));
    }

    // Persist the updated predictions back to the database
    {
        const QSignalBlocker blocker(&m_databaseManager);
        m_databaseManager.replaceParkingLots(updatedLots);
    }

    m_parkingLotModel.setLots(updatedLots);

    // Build QVariantList for QML, marking the best lot
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

void AppController::loadDefaultLots()
{
    // Only seed if the database is empty to avoid overwriting Maps API results
    // or previously stored user data on restart.
    if (!m_databaseManager.parkingLots().isEmpty()) {
        refreshLots();
        refreshRecommendations();
        return;
    }

    // Default lots represent a campus environment.
    // Replace lat/lng with your actual campus coordinates.
    const QList<ParkingLot> defaults = {
        ParkingLot(QStringLiteral("north-garage"),  QStringLiteral("North Garage"),
                   QStringLiteral("Engineering District"),
                   35.3050, -120.6626, QString(), 180, 90, 0.35, QString()),
        ParkingLot(QStringLiteral("library-lot"),   QStringLiteral("Library Lot"),
                   QStringLiteral("Central Campus"),
                   35.3055, -120.6618, QString(), 90,  45, 0.35, QString()),
        ParkingLot(QStringLiteral("stadium-lot"),   QStringLiteral("Stadium Lot"),
                   QStringLiteral("South Edge"),
                   35.2998, -120.6640, QString(), 220, 110, 0.35, QString()),
        ParkingLot(QStringLiteral("science-deck"),  QStringLiteral("Science Deck"),
                   QStringLiteral("Research Quad"),
                   35.3065, -120.6612, QString(), 140, 70, 0.35, QString())
    };

    {
        const QSignalBlocker blocker(&m_databaseManager);
        m_databaseManager.replaceParkingLots(defaults);
    }

    refreshLots();
    refreshRecommendations();
}
