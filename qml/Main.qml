import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import "components" as Components
import "screens" as Screens

ApplicationWindow {
    id: window

    width: 420
    height: 900
    visible: true
    title: "Lotly"
    color: "#151515"

    // ── Dark / light theme ────────────────────────────────────────────────────
    // The app uses hardcoded dark colours throughout; setting Material.theme
    // ensures that any Controls that delegate to the theme (dialogs, menus, etc.)
    // also respect the user's dark-mode preference.
    Material.theme: appController.settings.darkMode ? Material.Dark : Material.Light

    property int currentTab: 0

    // Live lot list from the C++ backend
    property var lots: appController.lots

    // Selected lot as QVariantMap from AppController
    property var selectedLot: appController.selectedLot

    property string submissionMessage: appController.lastSubmissionMessage

    // Keep local properties in sync when backend signals fire
    Connections {
        target: appController

        function onLotsChanged() {
            window.lots = appController.lots
        }
        function onSelectedLotChanged() {
            window.selectedLot = appController.selectedLot
        }
        function onLastSubmissionMessageChanged() {
            window.submissionMessage = appController.lastSubmissionMessage
        }

        // When the user signs out, collapse the entire nav stack to SignIn.
        function onIsSignedInChanged() {
            console.log("isSignedIn changed to: " + appController.isSignedIn)
            if (!appController.isSignedIn) {
                window.currentTab = 0
                rootLoader.sourceComponent = signInComponent
            } else {
                rootLoader.sourceComponent = mainAppComponent
            }
        }
    }

    // ── Root switcher — SignIn vs Main App ────────────────────────────────────
    Loader {
        id: rootLoader
        anchors.fill: parent
        sourceComponent: appController.isSignedIn ? mainAppComponent : signInComponent
    }

    // ── Sign-in screen ────────────────────────────────────────────────────────
    Component {
        id: signInComponent
        Screens.SignInScreen {}
    }

    // ── Main app shell ────────────────────────────────────────────────────────
    Component {
        id: mainAppComponent

        Item {
            id: appShell

            // ── Navigation helpers ────────────────────────────────────────────
            function openLots() {
                window.currentTab = 0
                stackView.replace(homeComponent)
            }
            function openReport() {
                window.currentTab = 1
                stackView.replace(reportComponent)
            }
            function openProfile() {
                window.currentTab = 2
                stackView.replace(profileComponent)
            }
            function openSettings() {
                window.currentTab = 3
                stackView.replace(settingsComponent)
            }
            function openDevTools() {
                stackView.push(devToolsComponent)
            }

            StackView {
                id: stackView
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: bottomNav.top
                initialItem: homeComponent
            }

            // ── Screen components ─────────────────────────────────────────────

            Component {
                id: homeComponent
                Screens.HomeScreen {
                    lots: window.lots
                    onLotSelected: function(lotId) {
                        appController.selectLot(lotId)
                        window.currentTab = 0
                        stackView.push(detailComponent)
                    }
                    onDevToolsRequested: appShell.openDevTools()
                }
            }

            Component {
                id: detailComponent
                Screens.LotDetailScreen {
                    lotData: window.selectedLot
                    onBackRequested: stackView.pop()
                    onOpenReport: {
                        window.currentTab = 1
                        stackView.push(reportComponent)
                    }
                    onOpenRecommendations: stackView.push(recommendationComponent)
                }
            }

            Component {
                id: reportComponent
                Screens.ReportScreen {
                    lots: window.lots
                    lotData: window.selectedLot
                    submissionMessage: window.submissionMessage
                    onBackRequested: {
                        if (stackView.depth > 1) stackView.pop()
                        else appShell.openLots()
                    }
                    onSubmitPressed: function(lotId, statusLabel) {
                        appController.submitReport(lotId, statusLabel)
                    }
                }
            }

            Component {
                id: recommendationComponent
                Screens.RecommendationView {
                    bestLot: {
                        var rec = appController.recommendation
                        return {
                            name: rec.bestLotName || "",
                            percentOpen: rec.bestLotPercentOpen || 0,
                            confidenceLabel: rec.bestLotConfidence >= 0.75 ? "High"
                                           : rec.bestLotConfidence >= 0.55 ? "Medium" : "Low",
                            explanation: rec.explanation || "",
                            bestPick: true
                        }
                    }
                    backupLot: {
                        var rec = appController.recommendation
                        var pct = rec.backupLotPercentOpen || 0
                        return {
                            name: rec.backupLotName || "",
                            percentOpen: pct,
                            confidenceLabel: rec.backupLotConfidence >= 0.75 ? "High"
                                           : rec.backupLotConfidence >= 0.55 ? "Medium" : "Low",
                            statusColor: pct >= 55 ? "#2D6DCC" : pct >= 30 ? "#D6A51D" : "#D5523F",
                            recentReports: 0
                        }
                    }
                    onBackRequested: stackView.pop()
                }
            }

            Component {
                id: devToolsComponent
                Screens.DevToolsScreen {
                    lots: window.lots
                    onBackRequested: stackView.pop()
                }
            }

            Component {
                id: profileComponent
                Screens.ProfileScreen {}
            }

            Component {
                id: settingsComponent
                Screens.SettingsScreen {}
            }

            // ── Bottom navigation ─────────────────────────────────────────────
            Components.BottomNav {
                id: bottomNav
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                currentIndex: window.currentTab
                onTabSelected: function(index) {
                    if      (index === 0) appShell.openLots()
                    else if (index === 1) appShell.openReport()
                    else if (index === 2) appShell.openProfile()
                    else                  appShell.openSettings()
                }
            }
        }
    }
}
