import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../components" as Components

Page {
    id: root

    property var bestLot: ({})
    property var backupLot: ({})

    signal backRequested()

    background: Rectangle { color: "#151515" }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            color: "#2D64B3"
            implicitHeight: 126

            Column {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.margins: 20
                spacing: 4

                Label {
                    text: "Recommendation"
                    color: "#F5F3EE"
                    font.pixelSize: 30
                    font.bold: true
                }

                Label {
                    text: "Best option for right now"
                    color: "#D5DEEF"
                    font.pixelSize: 18
                }
            }
        }

        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            contentWidth: -1

            ColumnLayout {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 18
                spacing: 16

                Components.SectionCard {
                    Layout.fillWidth: true
                    borderColor: "#2D6DCC"
                    implicitHeight: 286

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 18
                        spacing: 14

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 12

                            Label {
                                Layout.fillWidth: true
                                text: root.bestLot.name || ""
                                color: "#F5F3EE"
                                font.pixelSize: 22
                                font.bold: true
                            }

                            Rectangle {
                                radius: 10
                                color: "#EDF3FB"
                                implicitWidth: 96
                                implicitHeight: 34

                                Label {
                                    anchors.centerIn: parent
                                    text: "Best pick"
                                    color: "#2D6DCC"
                                    font.pixelSize: 16
                                    font.bold: true
                                }
                            }
                        }

                        Label {
                            text: (root.bestLot.percentOpen || 0) + "% open"
                            color: "#2D6DCC"
                            font.pixelSize: 30
                            font.bold: true
                        }

                        Components.ConfidenceBadge {
                            labelText: root.bestLot.confidenceLabel || "High"
                            compact: true
                        }

                        Components.SectionCard {
                            Layout.fillWidth: true
                            cardColor: "#E6EEF8"
                            borderColor: "#D8E2EE"
                            radiusValue: 18
                            implicitHeight: explanationLabel.implicitHeight + 26

                            Label {
                                id: explanationLabel
                                anchors.fill: parent
                                anchors.margins: 14
                                wrapMode: Text.WordWrap
                                text: root.bestLot.explanation || ""
                                color: "#315E9D"
                                font.pixelSize: 16
                            }
                        }
                    }
                }

                Label {
                    text: "Backup option"
                    color: "#D9D5CD"
                    font.pixelSize: 18
                    font.bold: true
                }

                Components.SectionCard {
                    Layout.fillWidth: true
                    implicitHeight: 148

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 18
                        spacing: 10

                        RowLayout {
                            Layout.fillWidth: true

                            Label {
                                Layout.fillWidth: true
                                text: root.backupLot.name || ""
                                color: "#F5F3EE"
                                font.pixelSize: 20
                                font.bold: true
                                wrapMode: Text.WordWrap
                            }

                            Components.ConfidenceBadge {
                                labelText: root.backupLot.confidenceLabel || "Med"
                                compact: true
                            }
                        }

                        Components.AvailabilityBar {
                            Layout.fillWidth: true
                            value: (root.backupLot.percentOpen || 0) / 100
                            fillColor: root.backupLot.statusColor || "#2D6DCC"
                        }

                        RowLayout {
                            Layout.fillWidth: true

                            Label {
                                Layout.fillWidth: true
                                text: (root.backupLot.percentOpen || 0) + "% open"
                                color: root.backupLot.statusColor || "#2D6DCC"
                                font.pixelSize: 16
                                font.bold: true
                            }

                            Label {
                                text: (root.backupLot.recentReports || 0) + " recent reports"
                                color: "#B8B2A7"
                                font.pixelSize: 15
                            }
                        }
                    }
                }
            }
        }
    }
}
