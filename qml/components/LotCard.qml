import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "." as Components

// Parking lot card matching the concept list layout.
Components.SectionCard {
    id: root

    property var lotData: ({})
    property bool compact: false
    property bool showAction: true
    signal clicked()

    cardColor: "#343432"
    borderColor: lotData.bestPick ? "#2D6DCC" : "#55544F"
    radiusValue: 22

    // Auto-size from content so the button never overflows the card boundary
    implicitHeight: cardLayout.implicitHeight + 36

    ColumnLayout {
        id: cardLayout
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 18
        spacing: compact ? 10 : 14

        RowLayout {
            Layout.fillWidth: true
            spacing: 12

            Label {
                Layout.fillWidth: true
                text: lotData.name || "Lot"
                color: "#F3F1EA"
                font.pixelSize: compact ? 18 : 20
                font.bold: true
                elide: Text.ElideRight
            }

            Rectangle {
                visible: lotData.bestPick === true
                radius: 10
                color: "#EDF3FB"
                implicitWidth: bestPickLabel.implicitWidth + 18
                implicitHeight: 34

                Label {
                    id: bestPickLabel
                    anchors.centerIn: parent
                    text: "Best pick"
                    color: "#2D6DCC"
                    font.pixelSize: 16
                    font.bold: true
                }
            }
        }

        Components.AvailabilityBar {
            Layout.fillWidth: true
            value: (lotData.percentOpen || 0) / 100
            fillColor: lotData.statusColor || "#2D6DCC"
            trackColor: "#5D5B54"
            barHeight: compact ? 8 : 9
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 14

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 4

                Label {
                    text: (lotData.percentOpen || 0) + "% open"
                    color: lotData.statusColor || "#2D6DCC"
                    font.pixelSize: compact ? 15 : 17
                    font.bold: true
                }

                RowLayout {
                    spacing: 10

                    Label {
                        text: "Confidence: " + ({"High":"high","Medium":"med","Low":"low"}[lotData.confidenceLabel] || (lotData.confidenceLabel || "med").toLowerCase())
                        color: "#D2CDC3"
                        font.pixelSize: compact ? 14 : 15
                    }

                    Label {
                        visible: (lotData.distanceMeters || -1) >= 0
                        text: {
                            const m = lotData.distanceMeters || 0
                            const ft = m * 3.28084
                            if (ft < 1000) return Math.round(ft) + " ft"
                            return (m / 1609.34).toFixed(1) + " mi"
                        }
                        color: "#8A8780"
                        font.pixelSize: compact ? 13 : 14
                    }
                }
            }

            Button {
                id: actionButton
                visible: root.showAction
                text: "View\n->"
                implicitWidth: compact ? 90 : 102
                implicitHeight: compact ? 62 : 70
                onClicked: root.clicked()

                background: Rectangle {
                    radius: 18
                    color: "#343432"
                    border.width: 1
                    border.color: "#66645E"
                }

                contentItem: Label {
                    text: actionButton.text
                    color: "#F3F1EA"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: compact ? 14 : 16
                    font.bold: true
                }
            }
        }
    }
}
