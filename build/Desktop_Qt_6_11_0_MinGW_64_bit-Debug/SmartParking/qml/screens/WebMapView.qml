// WebMapView.qml — loaded by MapScreen only when Qt WebView is installed.
// This file intentionally imports QtWebView so the build doesn't break when
// the module is absent (MapScreen never loads this component in that case).
import QtQuick
import QtQuick.Controls
import QtWebView

Item {
    // Read coordinates from the parent MapScreen via the Loader's item chain.
    // The Loader is inside MapScreen which exposes latitude / longitude.
    readonly property double latitude:    MapScreen ? MapScreen.latitude    : 35.3050
    readonly property double longitude:   MapScreen ? MapScreen.longitude   : -120.6626
    readonly property bool   hasLocation: MapScreen ? MapScreen.hasLocation : false

    // Rebuild the URL whenever coordinates change
    readonly property string mapUrl:
        "https://www.google.com/maps/search/parking/@"
        + latitude + "," + longitude + ",15z"

    WebView {
        id: webView
        anchors.fill: parent
        url: parent.mapUrl

        // Loading indicator
        Rectangle {
            anchors.fill: parent
            color: "#151515"
            visible: webView.loading

            Column {
                anchors.centerIn: parent
                spacing: 14

                BusyIndicator {
                    anchors.horizontalCenter: parent.horizontalCenter
                    running: webView.loading
                }

                Label {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "Loading map…"
                    color: "#9B988F"
                    font.pixelSize: 15
                }
            }
        }
    }
}
