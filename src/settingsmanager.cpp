#include "settingsmanager.h"

SettingsManager::SettingsManager(QObject *parent)
    : QObject(parent)
    , m_settings(QStringLiteral("Lotly"), QStringLiteral("Lotly"))
{
    // Load persisted values, falling back to the defaults declared in the header.
    m_darkMode             = m_settings.value(QStringLiteral("display/darkMode"),             true).toBool();
    m_largeText            = m_settings.value(QStringLiteral("display/largeText"),            false).toBool();
    m_highContrast         = m_settings.value(QStringLiteral("display/highContrast"),         false).toBool();

    m_notificationsEnabled = m_settings.value(QStringLiteral("notifications/enabled"),        true).toBool();
    m_reportReminders      = m_settings.value(QStringLiteral("notifications/reportReminders"),true).toBool();
    m_nearbyLotUpdates     = m_settings.value(QStringLiteral("notifications/nearbyLotUpdates"),false).toBool();

    m_locationEnabled      = m_settings.value(QStringLiteral("location/enabled"),             true).toBool();
    m_useCurrentLocation   = m_settings.value(QStringLiteral("location/useCurrent"),          false).toBool();

    m_largerTouchTargets   = m_settings.value(QStringLiteral("accessibility/largerTargets"),  false).toBool();
    m_simplifiedDisplay    = m_settings.value(QStringLiteral("accessibility/simplified"),     false).toBool();
}

// ── Display ───────────────────────────────────────────────────────────────────

bool SettingsManager::darkMode()     const { return m_darkMode; }
bool SettingsManager::largeText()    const { return m_largeText; }
bool SettingsManager::highContrast() const { return m_highContrast; }

void SettingsManager::setDarkMode(bool v) {
    writeSetting(QStringLiteral("display/darkMode"), v, m_darkMode, &SettingsManager::darkModeChanged);
}
void SettingsManager::setLargeText(bool v) {
    writeSetting(QStringLiteral("display/largeText"), v, m_largeText, &SettingsManager::largeTextChanged);
    emit baseFontSizeChanged();   // baseFontSize is derived from m_largeText
}
void SettingsManager::setHighContrast(bool v) {
    writeSetting(QStringLiteral("display/highContrast"), v, m_highContrast, &SettingsManager::highContrastChanged);
}

// ── Notifications ─────────────────────────────────────────────────────────────

bool SettingsManager::notificationsEnabled() const { return m_notificationsEnabled; }
bool SettingsManager::reportReminders()      const { return m_reportReminders; }
bool SettingsManager::nearbyLotUpdates()     const { return m_nearbyLotUpdates; }

void SettingsManager::setNotificationsEnabled(bool v) {
    writeSetting(QStringLiteral("notifications/enabled"), v, m_notificationsEnabled, &SettingsManager::notificationsEnabledChanged);
}
void SettingsManager::setReportReminders(bool v) {
    writeSetting(QStringLiteral("notifications/reportReminders"), v, m_reportReminders, &SettingsManager::reportRemindersChanged);
}
void SettingsManager::setNearbyLotUpdates(bool v) {
    writeSetting(QStringLiteral("notifications/nearbyLotUpdates"), v, m_nearbyLotUpdates, &SettingsManager::nearbyLotUpdatesChanged);
}

// ── Location Services ─────────────────────────────────────────────────────────

bool SettingsManager::locationEnabled()    const { return m_locationEnabled; }
bool SettingsManager::useCurrentLocation() const { return m_useCurrentLocation; }

void SettingsManager::setLocationEnabled(bool v) {
    writeSetting(QStringLiteral("location/enabled"), v, m_locationEnabled, &SettingsManager::locationEnabledChanged);
}
void SettingsManager::setUseCurrentLocation(bool v) {
    writeSetting(QStringLiteral("location/useCurrent"), v, m_useCurrentLocation, &SettingsManager::useCurrentLocationChanged);
}

// ── Accessibility ─────────────────────────────────────────────────────────────

bool SettingsManager::largerTouchTargets() const { return m_largerTouchTargets; }
bool SettingsManager::simplifiedDisplay()  const { return m_simplifiedDisplay; }

// ── Derived ───────────────────────────────────────────────────────────────────

int SettingsManager::baseFontSize() const { return m_largeText ? 18 : 14; }

void SettingsManager::setLargerTouchTargets(bool v) {
    writeSetting(QStringLiteral("accessibility/largerTargets"), v, m_largerTouchTargets, &SettingsManager::largerTouchTargetsChanged);
}
void SettingsManager::setSimplifiedDisplay(bool v) {
    writeSetting(QStringLiteral("accessibility/simplified"), v, m_simplifiedDisplay, &SettingsManager::simplifiedDisplayChanged);
}
