import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SmartParking

Page {
    id: root

    property var recommendationData: ({})

    signal backRequested()

    background: Rectangle {
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#f2f8f6" }
            GradientStop { position: 1.0; color: "#f7fbfc" }
        }
    }

    header: ToolBar {
        background: Rectangle { color: "#17313b" }

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 8
            anchors.rightMargin: 12

            ToolButton {
                text: "< Back"
                onClicked: root.backRequested()
            }

            Label {
                text: "Recommendation"
                color: "white"
                font.pixelSize: 22
                font.bold: true
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
            }

            Item {
                Layout.preferredWidth: 52
            }
        }
    }

    ScrollView {
        anchors.fill: parent

        ColumnLayout {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.margins: 20
            width: parent.width
            spacing: 18

            Rectangle {
                Layout.fillWidth: true
                radius: 22
                color: "#17313b"
                implicitHeight: bestColumn.implicitHeight + 28

                ColumnLayout {
                    id: bestColumn
                    anchors.fill: parent
                    anchors.margins: 18
                    spacing: 10

                    Label {
                        text: "Best lot"
                        color: "#c7ece6"
                        font.pixelSize: 18
                    }

                    Label {
                        text: recommendationData.bestLotName || "Unavailable"
                        color: "white"
                        font.pixelSize: 30
                        font.bold: true
                    }

                    Label {
                        text: (recommendationData.bestLotLocation || "") + " • "
                              + (recommendationData.bestLotAvailability || 0) + " spaces • "
                              + Math.round((recommendationData.bestLotConfidence || 0) * 100) + "% confidence"
                        wrapMode: Text.WordWrap
                        color: "#d9f0f0"
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                radius: 22
                color: "white"
                border.color: "#d8e5e6"
                implicitHeight: backupColumn.implicitHeight + 28

                ColumnLayout {
                    id: backupColumn
                    anchors.fill: parent
                    anchors.margins: 18
                    spacing: 10

                    Label {
                        text: "Backup lot"
                        color: "#40636d"
                        font.pixelSize: 18
                    }

                    Label {
                        text: recommendationData.backupLotName || "Unavailable"
                        color: "#17313b"
                        font.pixelSize: 26
                        font.bold: true
                    }

                    Label {
                        text: (recommendationData.backupLotLocation || "") + " • "
                              + (recommendationData.backupLotAvailability || 0) + " spaces • "
                              + Math.round((recommendationData.backupLotConfidence || 0) * 100) + "% confidence"
                        wrapMode: Text.WordWrap
                        color: "#557078"
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                radius: 20
                color: "#eaf4f6"
                implicitHeight: explanationLabel.implicitHeight + 24

                Label {
                    id: explanationLabel
                    anchors.fill: parent
                    anchors.margins: 12
                    wrapMode: Text.WordWrap
                    text: recommendationData.explanation || "Recommendation explanation unavailable."
                    color: "#345964"
                }
            }
        }
    }
}
