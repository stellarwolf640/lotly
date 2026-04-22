#pragma once

#include <QObject>
#include <QSettings>

// SettingsManager persists all user-configurable settings via QSettings.
// All values survive app restarts. Expose via context property "settingsManager".
//
// Integration notes for settings not yet wired into the rest of the app:
//   darkMode         — TODO: switch app palette at runtime
//   largeText        — TODO: scale font.pixelSize across components
//   highContrast     — TODO: swap colour tokens to accessible palette
//   nearbyLotUpdates — TODO: background polling / push notifications
//   locationEnabled  — TODO: gate all location/API calls behind this flag
//   useCurrentLocation — TODO: on startup, request GPS and call setSearchLocation()
//   largerTouchTargets — TODO: increase implicitHeight of interactive controls
//   simplifiedDisplay  — TODO: hide confidence/chart cards in lot detail view
class SettingsManager : public QObject
{
    Q_OBJECT

    // ── Display ───────────────────────────────────────────────────────────────
    Q_PROPERTY(bool darkMode     READ darkMode     WRITE setDarkMode     NOTIFY darkModeChanged)
    Q_PROPERTY(bool largeText    READ largeText    WRITE setLargeText    NOTIFY largeTextChanged)
    Q_PROPERTY(bool highContrast READ highContrast WRITE setHighContrast NOTIFY highContrastChanged)

    // ── Notifications ─────────────────────────────────────────────────────────
    Q_PROPERTY(bool notificationsEnabled READ notificationsEnabled WRITE setNotificationsEnabled NOTIFY notificationsEnabledChanged)
    Q_PROPERTY(bool reportReminders      READ reportReminders      WRITE setReportReminders      NOTIFY reportRemindersChanged)
    Q_PROPERTY(bool nearbyLotUpdates     READ nearbyLotUpdates     WRITE setNearbyLotUpdates     NOTIFY nearbyLotUpdatesChanged)

    // ── Location Services ─────────────────────────────────────────────────────
    Q_PROPERTY(bool locationEnabled    READ locationEnabled    WRITE setLocationEnabled    NOTIFY locationEnabledChanged)
    Q_PROPERTY(bool useCurrentLocation READ useCurrentLocation WRITE setUseCurrentLocation NOTIFY useCurrentLocationChanged)

    // ── Accessibility ─────────────────────────────────────────────────────────
    Q_PROPERTY(bool largerTouchTargets READ largerTouchTargets WRITE setLargerTouchTargets NOTIFY largerTouchTargetsChanged)
    Q_PROPERTY(bool simplifiedDisplay  READ simplifiedDisplay  WRITE setSimplifiedDisplay  NOTIFY simplifiedDisplayChanged)

public:
    explicit SettingsManager(QObject *parent = nullptr);

    bool darkMode()     const;
    bool largeText()    const;
    bool highContrast() const;

    bool notificationsEnabled() const;
    bool reportReminders()      const;
    bool nearbyLotUpdates()     const;

    bool locationEnabled()    const;
    bool useCurrentLocation() const;

    bool largerTouchTargets() const;
    bool simplifiedDisplay()  const;

public slots:
    void setDarkMode(bool v);
    void setLargeText(bool v);
    void setHighContrast(bool v);

    void setNotificationsEnabled(bool v);
    void setReportReminders(bool v);
    void setNearbyLotUpdates(bool v);

    void setLocationEnabled(bool v);
    void setUseCurrentLocation(bool v);

    void setLargerTouchTargets(bool v);
    void setSimplifiedDisplay(bool v);

signals:
    void darkModeChanged();
    void largeTextChanged();
    void highContrastChanged();

    void notificationsEnabledChanged();
    void reportRemindersChanged();
    void nearbyLotUpdatesChanged();

    void locationEnabledChanged();
    void useCurrentLocationChanged();

    void largerTouchTargetsChanged();
    void simplifiedDisplayChanged();

private:
    template<typename T>
    void writeSetting(const QString &key, const T &value, T &cache, void(SettingsManager::*signal)())
    {
        if (cache == value)
            return;
        cache = value;
        m_settings.setValue(key, value);
        emit (this->*signal)();
    }

    QSettings m_settings;

    bool m_darkMode             = true;
    bool m_largeText            = false;
    bool m_highContrast         = false;
    bool m_notificationsEnabled = true;
    bool m_reportReminders      = true;
    bool m_nearbyLotUpdates     = false;
    bool m_locationEnabled      = true;
    bool m_useCurrentLocation   = false;
    bool m_largerTouchTargets   = false;
    bool m_simplifiedDisplay    = false;
};
