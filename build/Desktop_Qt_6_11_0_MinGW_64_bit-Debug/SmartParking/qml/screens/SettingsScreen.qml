import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../components" as Components

Page {
    id: root

    background: Rectangle { color: "#151515" }

    // ── Inline component: a labelled toggle row ───────────────────────────────
    component SettingRow: Item {
        id: rowRoot

        property string  label:       ""
        property string  description: ""
        property bool    checked:     false
        property bool    enabled:     true

        signal toggled(bool value)

        implicitHeight: rowLayout.implicitHeight + 20

        RowLayout {
            id: rowLayout
            anchors.left:    parent.left
            anchors.right:   parent.right
            anchors.verticalCenter: parent.verticalCenter
            spacing: 12

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 2

                Label {
                    text:           rowRoot.label
                    color:          rowRoot.enabled ? "#E8E5DE" : "#6A6762"
                    font.pixelSize: 16
                    font.bold:      true
                }

                Label {
                    visible:        rowRoot.description.length > 0
                    text:           rowRoot.description
                    color:          rowRoot.enabled ? "#9B988F" : "#565451"
                    font.pixelSize: 13
                    wrapMode:       Text.WordWrap
                    Layout.fillWidth: true
                }
            }

            Switch {
                id: sw
                checked:  rowRoot.checked
                enabled:  rowRoot.enabled
                onToggled: rowRoot.toggled(checked)

                indicator: Rectangle {
                    implicitWidth:  50
                    implicitHeight: 28
                    radius:         14
                    color:          sw.checked ? "#2D6DCC" : "#4A4844"
                    border.color:   sw.checked ? "#2D6DCC" : "#65635D"
                    Behavior on color { ColorAnimation { duration: 120 } }

                    Rectangle {
                        x:      sw.checked ? parent.width - width - 3 : 3
                        y:      3
                        width:  22
                        height: 22
                        radius: 11
                        color:  sw.checked ? "#F5F3EE" : "#9B988F"
                        Behavior on x { NumberAnimation { duration: 120 } }
                    }
                }

                background: Item {}
                contentItem: Item {}
            }
        }

        // Subtle divider
        Rectangle {
            anchors.bottom: parent.bottom
            anchors.left:   parent.left
            anchors.right:  parent.right
            height:         1
            color:          "#2A2A28"
        }
    }

    // ── Section header helper ─────────────────────────────────────────────────
    component SectionHeader: Label {
        Layout.fillWidth:    true
        Layout.topMargin:    8
        Layout.bottomMargin: 4
        color:               "#6C9DD4"
        font.pixelSize:      13
        font.bold:           true
        font.letterSpacing:  0.8
        text:                ""   // set inline
    }

    // ─────────────────────────────────────────────────────────────────────────

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // ── Header ───────────────────────────────────────────────────────────
        Rectangle {
            Layout.fillWidth: true
            color: "#2D64B3"
            implicitHeight: 100

            Label {
                anchors.left:   parent.left
                anchors.bottom: parent.bottom
                anchors.margins: 20
                text:           "Settings"
                color:          "#F5F3EE"
                font.pixelSize: 28
                font.bold:      true
            }
        }

        // ── Scrollable body ───────────────────────────────────────────────────
        ScrollView {
            Layout.fillWidth:  true
            Layout.fillHeight: true
            clip:              true
            contentWidth:      -1

            ColumnLayout {
                anchors.left:    parent.left
                anchors.right:   parent.right
                anchors.margins: 16
                spacing:         12

                // ── DISPLAY ───────────────────────────────────────────────────
                SectionHeader { text: "DISPLAY" }

                Components.SectionCard {
                    Layout.fillWidth: true
                    implicitHeight:   displayCol.implicitHeight + 8

                    ColumnLayout {
                        id: displayCol
                        anchors.left:    parent.left
                        anchors.right:   parent.right
                        anchors.top:     parent.top
                        anchors.margins: 4
                        spacing: 0

                        SettingRow {
                            Layout.fillWidth: true
                            label:       "Dark mode"
                            description: "Use the dark colour scheme"
                            checked:     settingsManager.darkMode
                            onToggled:   settingsManager.darkMode = value
                            // TODO: connect to app-wide palette swap when implemented
                        }

                        SettingRow {
                            Layout.fillWidth: true
                            label:       "Large text"
                            description: "Increase font sizes throughout the app"
                            checked:     settingsManager.largeText
                            onToggled:   settingsManager.largeText = value
                            // TODO: scale font.pixelSize via a global font size multiplier
                        }

                        SettingRow {
                            Layout.fillWidth: true
                            label:       "High contrast"
                            description: "Boost colour contrast for easier reading"
                            checked:     settingsManager.highContrast
                            onToggled:   settingsManager.highContrast = value
                            // TODO: swap colour tokens to WCAG AA-compliant palette
                        }
                    }
                }

                // ── NOTIFICATIONS ─────────────────────────────────────────────
                SectionHeader { text: "NOTIFICATIONS" }

                Components.SectionCard {
                    Layout.fillWidth: true
                    implicitHeight:   notifCol.implicitHeight + 8

                    ColumnLayout {
                        id: notifCol
                        anchors.left:    parent.left
                        anchors.right:   parent.right
                        anchors.top:     parent.top
                        anchors.margins: 4
                        spacing: 0

                        SettingRow {
                            id: notifMasterRow
                            Layout.fillWidth: true
                            label:       "Enable notifications"
                            description: "Allow ParkSmart to send you alerts"
                            checked:     settingsManager.notificationsEnabled
                            onToggled:   settingsManager.notificationsEnabled = value
                        }

                        SettingRow {
                            Layout.fillWidth: true
                            label:       "Report reminders"
                            description: "Remind you to submit a report after parking"
                            checked:     settingsManager.reportReminders
                            enabled:     settingsManager.notificationsEnabled
                            onToggled:   settingsManager.reportReminders = value
                            // TODO: schedule notification via NotificationManager
                        }

                        SettingRow {
                            Layout.fillWidth: true
                            label:       "Nearby lot updates"
                            description: "Alert when a nearby lot frees up significantly"
                            checked:     settingsManager.nearbyLotUpdates
                            enabled:     settingsManager.notificationsEnabled
                            onToggled:   settingsManager.nearbyLotUpdates = value
                            // TODO: trigger background lot polling when enabled
                        }
                    }
                }

                // ── LOCATION SERVICES ─────────────────────────────────────────
                SectionHeader { text: "LOCATION SERVICES" }

                Components.SectionCard {
                    Layout.fillWidth: true
                    implicitHeight:   locCol.implicitHeight + 8

                    ColumnLayout {
                        id: locCol
                        anchors.left:    parent.left
                        anchors.right:   parent.right
                        anchors.top:     parent.top
                        anchors.margins: 4
                        spacing: 0

                        SettingRow {
                            Layout.fillWidth: true
                            label:       "Location services"
                            description: "Allow ParkSmart to access your location"
                            checked:     settingsManager.locationEnabled
                            onToggled:   settingsManager.locationEnabled = value
                            // TODO: request Android location permission when enabled
                        }

                        SettingRow {
                            Layout.fillWidth: true
                            label:       "Use current location by default"
                            description: "Auto-search nearby lots on app launch"
                            checked:     settingsManager.useCurrentLocation
                            enabled:     settingsManager.locationEnabled
                            onToggled:   settingsManager.useCurrentLocation = value
                            // TODO: on startup, if enabled, call GPS → appController.setSearchLocation()
                        }
                    }
                }

                // ── ACCESSIBILITY ─────────────────────────────────────────────
                SectionHeader { text: "ACCESSIBILITY" }

                Components.SectionCard {
                    Layout.fillWidth: true
                    implicitHeight:   a11yCol.implicitHeight + 8

                    ColumnLayout {
                        id: a11yCol
                        anchors.left:    parent.left
                        anchors.right:   parent.right
                        anchors.top:     parent.top
                        anchors.margins: 4
                        spacing: 0

                        SettingRow {
                            Layout.fillWidth: true
                            label:       "Larger touch targets"
                            description: "Increase tap area for buttons and controls"
                            checked:     settingsManager.largerTouchTargets
                            onToggled:   settingsManager.largerTouchTargets = value
                            // TODO: increase implicitHeight on interactive elements
                        }

                        SettingRow {
                            Layout.fillWidth: true
                            label:       "Simplified display"
                            description: "Hide charts and confidence details for a cleaner view"
                            checked:     settingsManager.simplifiedDisplay
                            onToggled:   settingsManager.simplifiedDisplay = value
                            // TODO: bind hourlyCard.visible and confidenceBadge.visible to this flag
                        }
                    }
                }

                // ── ABOUT ─────────────────────────────────────────────────────
                SectionHeader { text: "ABOUT" }

                Components.SectionCard {
                    Layout.fillWidth: true
                    implicitHeight:   aboutCol.implicitHeight + 24

                    ColumnLayout {
                        id: aboutCol
                        anchors.left:    parent.left
                        anchors.right:   parent.right
                        anchors.top:     parent.top
                        anchors.margins: 16
                        spacing: 6

                        Label {
                            text:           "ParkSmart"
                            color:          "#E8E5DE"
                            font.pixelSize: 17
                            font.bold:      true
                        }
                        Label {
                            text:           "Smart Parking Availability & Prediction System"
                            color:          "#9B988F"
                            font.pixelSize: 13
                            wrapMode:       Text.WordWrap
                            Layout.fillWidth: true
                        }
                        Label {
                            text:           "Version 1.0 — Cal Poly Demo"
                            color:          "#6A6762"
                            font.pixelSize: 12
                        }
                    }
                }

                // Bottom padding
                Item { Layout.preferredHeight: 20 }
            }
        }
    }
}
