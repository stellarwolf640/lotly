import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: root

    background: Rectangle { color: "#151515" }

    // ── AppController callbacks ───────────────────────────────────────────────
    Connections {
        target: appController

        function onIsSignedInChanged() {
            if (appController.isSignedIn) {
                // Main.qml's Loader will swap in the main app component.
                busyIndicator.running = false
                errorLabel.text = ""
            }
        }

        function onSignInFailed(error) {
            console.log("SignInScreen: signInFailed —", error)
            busyIndicator.running = false
            errorLabel.text = error.length > 0
                ? error
                : "Sign-in failed. Please try again."
        }
    }

    // ── Sign-in card ──────────────────────────────────────────────────────────
    ColumnLayout {
        anchors.centerIn: parent
        width: Math.min(parent.width - 64, 340)
        spacing: 0

        // App icon
        Rectangle {
            Layout.alignment: Qt.AlignHCenter
            width: 96; height: 96; radius: 24
            color: "#2D64B3"
            Label {
                anchors.centerIn: parent
                text: "L"
                color: "#F5F3EE"
                font.pixelSize: 52
                font.bold: true
            }
        }

        Item { Layout.preferredHeight: 28 }

        Label {
            Layout.alignment: Qt.AlignHCenter
            text: "Lotly"
            color: "#F5F3EE"
            font.pixelSize: 36
            font.bold: true
        }

        Item { Layout.preferredHeight: 8 }

        Label {
            Layout.alignment: Qt.AlignHCenter
            text: "Smarter parking, powered by you"
            color: "#8C887E"
            font.pixelSize: 14
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }

        Item { Layout.preferredHeight: 56 }

        // ── Sign-in button ────────────────────────────────────────────────────
        Button {
            id: signInBtn
            Layout.fillWidth: true
            implicitHeight: 54
            enabled: !busyIndicator.running

            onClicked: {
                errorLabel.text = ""
                busyIndicator.running = true
                // Opens the Google consent page in the system browser (Chrome).
                // The flow continues when Android routes the OAuth callback deep
                // link back to the app and main.cpp calls handleDeepLink().
                appController.signIn()
            }

            background: Rectangle {
                radius: 16
                color: signInBtn.enabled ? "#FFFFFF" : "#3A3A36"
                border.width: 1
                border.color: signInBtn.enabled ? "#E0DDD6" : "#4A4A46"
            }

            contentItem: RowLayout {
                spacing: 12
                anchors.centerIn: parent

                // Google "G" logo
                Canvas {
                    id: googleLogo
                    width: 24; height: 24

                    onPaint: {
                        var ctx = getContext("2d")
                        ctx.clearRect(0, 0, width, height)
                        var cx = width  / 2
                        var cy = height / 2
                        var r  = width  / 2
                        var ri = r * 0.40

                        function drawSlice(startDeg, endDeg, color) {
                            var s = startDeg * Math.PI / 180
                            var e = endDeg   * Math.PI / 180
                            ctx.beginPath()
                            ctx.moveTo(cx + ri * Math.cos(s), cy + ri * Math.sin(s))
                            ctx.arc(cx, cy, r,  s, e, false)
                            ctx.arc(cx, cy, ri, e, s, true)
                            ctx.closePath()
                            ctx.fillStyle = color
                            ctx.fill()
                        }

                        drawSlice(-45,  45, "#4285F4")
                        drawSlice( 45, 135, "#34A853")
                        drawSlice(135, 225, "#EA4335")
                        drawSlice(225, 315, "#FBBC05")

                        var barH = r * 0.35
                        ctx.fillStyle = "#4285F4"
                        ctx.fillRect(cx, cy - barH / 2, r, barH)

                        ctx.beginPath()
                        ctx.arc(cx, cy, ri, 0, Math.PI * 2)
                        ctx.fillStyle = "#FFFFFF"
                        ctx.fill()
                    }
                }

                Label {
                    text: "Sign in with Google"
                    color: "#3C3C3C"
                    font.pixelSize: 16
                    font.bold: true
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }

        Item { Layout.preferredHeight: 20 }

        // Loading spinner — visible while waiting for the browser callback
        BusyIndicator {
            id: busyIndicator
            Layout.alignment: Qt.AlignHCenter
            running: false
            visible: running
        }

        Label {
            id: errorLabel
            Layout.fillWidth: true
            text: ""
            visible: text.length > 0
            color: "#D5523F"
            font.pixelSize: 14
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
        }

        Item { Layout.preferredHeight: 48 }

        Label {
            Layout.alignment: Qt.AlignHCenter
            text: "Your account keeps reports synced\nacross all your devices."
            color: "#5C5850"
            font.pixelSize: 13
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }
    }
}
