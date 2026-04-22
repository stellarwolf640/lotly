import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

// MapScreen — replaces the History placeholder tab.
//
// Behaviour:
//   • If Qt WebView is installed (hasWebView == true):
//       Loads an embedded Google Maps view centred on the search location.
//   • Otherwise:
//       Shows a clean placeholder with the current coordinates and install instructions.
//
// To enable the live map:
//   Qt Maintenance Tool → Qt 6.x → Additional Libraries → Qt WebView → Install
//   Then rebuild — CMake will detect it automatically.
//
// Future improvement — adding lot markers:
//   Switch the WebView source to a local HTML resource that uses the Maps JavaScript API
//   and inject JS via webView.runJavaScript("addMarker(lat, lng, 'Lot Name')").
Page {
    id: root

    property double latitude:    35.3050
    property double longitude:  -120.6626
    property bool   hasLocation: false

    background: Rectangle { color: "#151515" }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // ── Header ────────────────────────────────────────────────────────────
        Rectangle {
            Layout.fillWidth: true
            color: "#2D64B3"
            implicitHeight: 100

            ColumnLayout {
                anchors.left:    parent.left
                anchors.right:   parent.right
                anchors.bottom:  parent.bottom
                anchors.margins: 20
                spacing: 2

                Label {
                    text: "Nearby Map"
                    color: "#F5F3EE"
                    font.pixelSize: 26
                    font.bold: true
                }

                Label {
                    Layout.fillWidth: true
                    text: root.hasLocation
                          ? (appController.searchLocationName || "Selected location")
                          : "Default campus location"
                    color: "#D6DFEF"
                    font.pixelSize: 15
                    elide: Text.ElideRight
                }
            }
        }

        // ── Map area — live WebView or install prompt ─────────────────────────
        Loader {
            id: mapLoader
            Layout.fillWidth:  true
            Layout.fillHeight: true
            // hasWebView is set in main.cpp from the HAS_QTWEBVIEW compile definition
            sourceComponent: hasWebView ? webMapComponent : placeholderComponent
        }
    }

    // ── Live WebView map (only compiled/used when Qt WebView is installed) ────
    Component {
        id: webMapComponent

        Item {
            // Dynamic import — avoids a hard compile-time dependency on QtWebView.
            // If this Loader branch is ever reached, QtWebView is guaranteed present.
            Loader {
                anchors.fill: parent
                source: "WebMapView.qml"
            }
        }
    }

    // ── Placeholder — shown when Qt WebView is not installed ──────────────────
    Component {
        id: placeholderComponent

        Rectangle {
            color: "#151515"

            ColumnLayout {
                anchors.centerIn: parent
                width: parent.width * 0.78
                spacing: 20

                // Map icon placeholder
                Rectangle {
                    Layout.alignment: Qt.AlignHCenter
                    width: 72; height: 72; radius: 36
                    color: "#1E2A3A"
                    border.width: 2
                    border.color: "#2D5A9A"

                    Label {
                        anchors.centerIn: parent
                        text: "🗺"
                        font.pixelSize: 32
                    }
                }

                Label {
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    text: "Map not available"
                    color: "#D5D0C6"
                    font.pixelSize: 20
                    font.bold: true
                    wrapMode: Text.WordWrap
                }

                Label {
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    text: "Install the Qt WebView component to enable the embedded map."
                    color: "#8A8780"
                    font.pixelSize: 14
                    wrapMode: Text.WordWrap
                }

                // Install instructions card
                Rectangle {
                    Layout.fillWidth: true
                    radius: 14
                    color: "#1E1E1C"
                    border.width: 1
                    border.color: "#3A3A36"
                    implicitHeight: instrCol.implicitHeight + 24

                    ColumnLayout {
                        id: instrCol
                        anchors.left:    parent.left
                        anchors.right:   parent.right
                        anchors.top:     parent.top
                        anchors.margins: 16
                        spacing: 6

                        Label {
                            text: "How to enable:"
                            color: "#C8C3B8"
                            font.pixelSize: 13
                            font.bold: true
                        }
                        Label {
                            Layout.fillWidth: true
                            text: "1.  Open Qt Maintenance Tool\n" +
                                  "2.  Add or remove components\n" +
                                  "3.  Qt 6.x  →  Additional Libraries\n" +
                                  "4.  Check  Qt WebView  →  Install\n" +
                                  "5.  Rebuild the project"
                            color: "#8A8780"
                            font.pixelSize: 13
                            lineHeight: 1.5
                            wrapMode: Text.WordWrap
                        }
                    }
                }

                // Show current coordinates so the tab is still useful
                Rectangle {
                    visible: root.hasLocation
                    Layout.fillWidth: true
                    radius: 14
                    color: "#1A2A3A"
                    border.width: 1
                    border.color: "#2D5A9A"
                    implicitHeight: coordCol.implicitHeight + 20

                    ColumnLayout {
                        id: coordCol
                        anchors.left:    parent.left
                        anchors.right:   parent.right
                        anchors.top:     parent.top
                        anchors.margins: 14
                        spacing: 4

                        Label {
                            text: "Current search location"
                            color: "#6C9DD4"
                            font.pixelSize: 12
                            font.bold: true
                        }
                        Label {
                            Layout.fillWidth: true
                            text: appController.searchLocationName || "Custom location"
                            color: "#D5D0C6"
                            font.pixelSize: 14
                            font.bold: true
                            elide: Text.ElideRight
                        }
                        Label {
                            text: root.latitude.toFixed(5) + ", " + root.longitude.toFixed(5)
                            color: "#8A8780"
                            font.pixelSize: 12
                            font.family: "monospace"
                        }
                    }
                }
            }
        }
    }
}
