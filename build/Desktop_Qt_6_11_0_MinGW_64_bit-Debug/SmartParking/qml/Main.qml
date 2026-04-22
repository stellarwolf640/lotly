import QtQuick
import QtQuick.Controls
import "data/MockParkingData.js" as MockData
import "components" as Components
import "screens" as Screens

ApplicationWindow {
    id: window

    width: 420
    height: 900
    visible: true
    title: "Smart Parking Availability & Prediction System"
    color: "#151515"

    property int currentTab: 0

    // Live lot list from the C++ backend (replaces static MockData.lots)
    property var lots: appController.lots

    // Current search location — forwarded to MapScreen
    property double mapLat: 35.3050
    property double mapLng: -120.6626
    property bool   mapHasLocation: false

    // Selected lot as QVariantMap from AppController
    property var selectedLot: appController.selectedLot

    property string submissionMessage: appController.lastSubmissionMessage

    // Keep selectedLot in sync when the backend emits selectedLotChanged
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
        function onSearchLocationChanged() {
            window.mapLat         = appController.searchLat
            window.mapLng         = appController.searchLng
            window.mapHasLocation = appController.hasSearchLocation
        }
    }

    // ── Navigation helpers ────────────────────────────────────────────────────

    function openLots() {
        currentTab = 0
        stackView.replace(homeComponent)
    }

    function openReport() {
        currentTab = 1
        stackView.replace(reportComponent)
    }

    function openHistory() {
        currentTab = 2
        stackView.replace(historyComponent)
    }

    function openSettings() {
        currentTab = 3
        stackView.replace(settingsComponent)
    }

    function openDevTools() {
        stackView.push(devToolsComponent)
    }

    // ── Stack ─────────────────────────────────────────────────────────────────

    StackView {
        id: stackView
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: bottomNav.top
        initialItem: homeComponent
    }

    // ── Screen components ─────────────────────────────────────────────────────

    Component {
        id: homeComponent
        Screens.HomeScreen {
            lots: window.lots
            onLotSelected: function(lotId) {
                appController.selectLot(lotId)
                window.currentTab = 0
                stackView.push(detailComponent)
            }
            onDevToolsRequested: window.openDevTools()
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
            lotData: window.selectedLot
            submissionMessage: window.submissionMessage
            onBackRequested: {
                if (stackView.depth > 1) stackView.pop()
                else window.openLots()
            }
            onSubmitPressed: function(statusLabel) {
                appController.submitReport(
                    window.selectedLot.id || "",
                    statusLabel)
            }
        }
    }

    Component {
        id: recommendationComponent
        Screens.RecommendationView {
            bestLot:   {
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
        id: historyComponent
        Screens.MapScreen {
            latitude:    window.mapLat
            longitude:   window.mapLng
            hasLocation: window.mapHasLocation
        }
    }

    Component {
        id: settingsComponent
        Screens.SettingsScreen {}
    }

    // ── Bottom navigation ─────────────────────────────────────────────────────

    Components.BottomNav {
        id: bottomNav
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        currentIndex: window.currentTab
        onTabSelected: function(index) {
            if (index === 0)      window.openLots()
            else if (index === 1) window.openReport()
            else if (index === 2) window.openHistory()
            else                  window.openSettings()
        }
    }
}
