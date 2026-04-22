import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../components" as Components

// Detail screen rebuilt to match the concept card hierarchy.
Page {
    id: root

    property var lotData: ({})

    signal backRequested()
    signal openReport()
    signal openRecommendations()

    background: Rectangle {
        color: "#151515"
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            color: "#2D64B3"
            implicitHeight: 126

            RowLayout {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.margins: 18
                spacing: 14

                ToolButton {
                    text: "<"
                    onClicked: root.backRequested()

                    background: Item {}
                    contentItem: Label {
                        text: "<"
                        color: "#F5F3EE"
                        font.pixelSize: 28
                        font.bold: true
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 4

                    Label {
                        text: root.lotData.name || "Lot detail"
                        color: "#F5F3EE"
                        font.pixelSize: 28
                        font.bold: true
                    }

                    RowLayout {
                        spacing: 8

                        Label {
                            text: "Predicted availability"
                            color: "#D6DFEF"
                            font.pixelSize: 18
                        }

                        Label {
                            visible: (root.lotData.distanceMeters || -1) >= 0
                            text: {
                                const m = root.lotData.distanceMeters || 0
                                const ft = m * 3.28084
                                if (ft < 1000) return "\u2022 " + Math.round(ft) + " ft"
                                return "\u2022 " + (m / 1609.34).toFixed(1) + " mi"
                            }
                            color: "#B8C8E0"
                            font.pixelSize: 16
                        }
                    }
                }
            }
        }

        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            contentWidth: -1

            ColumnLayout {
                spacing: 16
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 18

                Components.SectionCard {
                    Layout.fillWidth: true
                    implicitHeight: 170

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 18
                        spacing: 16

                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 4

                            Label {
                                text: (root.lotData.percentOpen || 0) + "%"
                                color: "#2D6DCC"
                                font.pixelSize: 60
                                font.bold: true
                            }

                            Label {
                                text: "estimated open"
                                color: "#D5D0C6"
                                font.pixelSize: 18
                            }
                        }

                        ColumnLayout {
                            spacing: 10

                            Label {
                                text: "Confidence"
                                color: "#D5D0C6"
                                font.pixelSize: 18
                                font.bold: true
                            }

                            Components.ConfidenceBadge {
                                labelText: root.lotData.confidenceLabel || "Medium"
                                reportCount: root.lotData.recentReports || 0
                            }
                        }
                    }
                }

                Components.SectionCard {
                    Layout.fillWidth: true
                    visible: (root.lotData.address || "").length > 0
                    implicitHeight: addrCol.implicitHeight + 32

                    ColumnLayout {
                        id: addrCol
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.top: parent.top
                        anchors.margins: 16
                        spacing: 4

                        Label {
                            text: "Address"
                            color: "#9B988F"
                            font.pixelSize: 13
                            font.bold: true
                        }

                        Label {
                            Layout.fillWidth: true
                            text: root.lotData.address || ""
                            color: "#D9D5CD"
                            font.pixelSize: 15
                            wrapMode: Text.WordWrap
                        }
                    }
                }

                Components.SectionCard {
                    id: hourlyCard
                    Layout.fillWidth: true
                    implicitHeight: 222

                    // 19 values: index 0 = 6 AM, index 18 = midnight
                    property var _hourlyData: root.lotData.hourlyAvailability
                                              || [88,82,70,60,55,58,62,68,70,72,68,65,62,68,74,80,85,88,92]
                    property real _maxVal: {
                        var m = 1
                        for (var i = 0; i < _hourlyData.length; i++)
                            if (_hourlyData[i] > m) m = _hourlyData[i]
                        return m
                    }
                    // Map current clock hour to chart index (6 AM = 0, midnight = 18)
                    property int _nowIndex: {
                        var h = new Date().getHours()
                        if (h === 0) return 18
                        if (h < 6)  return 0
                        return Math.min(18, h - 6)
                    }

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 18
                        spacing: 14

                        Label {
                            text: "Availability by hour"
                            color: "#D9D5CD"
                            font.pixelSize: 18
                            font.bold: true
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignBottom
                            spacing: 2

                            Repeater {
                                model: hourlyCard._hourlyData

                                delegate: ColumnLayout {
                                    required property int index
                                    required property int modelData

                                    Layout.fillWidth: true
                                    Layout.alignment: Qt.AlignBottom
                                    spacing: 4

                                    Rectangle {
                                        Layout.fillWidth: true
                                        Layout.preferredHeight: Math.max(6, (modelData / hourlyCard._maxVal) * 88)
                                        radius: 3
                                        color: index === hourlyCard._nowIndex ? "#2D6DCC" : "#A8C6EA"
                                    }

                                    Label {
                                        Layout.fillWidth: true
                                        // Label every 3rd hour; "now" overrides its slot
                                        text: {
                                            if (index === hourlyCard._nowIndex) return "now"
                                            var h = index + 6
                                            if (h % 3 !== 0) return ""
                                            if (h === 12) return "12p"
                                            if (h === 24) return "12a"
                                            if (h < 12)   return h + "a"
                                            return (h - 12) + "p"
                                        }
                                        color: index === hourlyCard._nowIndex ? "#2D6DCC" : "#B7B3AA"
                                        font.pixelSize: 11
                                        horizontalAlignment: Text.AlignHCenter
                                    }
                                }
                            }
                        }
                    }
                }

                Components.SectionCard {
                    Layout.fillWidth: true
                    cardColor: "#E6EEF8"
                    borderColor: "#D8E2EE"
                    radiusValue: 20
                    implicitHeight: reasonText.implicitHeight + 34

                    Label {
                        id: reasonText
                        anchors.fill: parent
                        anchors.margins: 16
                        wrapMode: Text.WordWrap
                        text: "Why recommended: " + (root.lotData.explanation || "")
                        color: "#335F9E"
                        font.pixelSize: 16
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 12

                    Button {
                        id: reportButton
                        Layout.fillWidth: true
                        text: "Submit report"
                        onClicked: root.openReport()

                        background: Rectangle {
                            radius: 18
                            color: "#343432"
                            border.width: 1
                            border.color: "#65635D"
                        }

                        contentItem: Label {
                            text: reportButton.text
                            color: "#F4F1EA"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            font.pixelSize: 17
                            font.bold: true
                        }
                    }

                    Button {
                        id: recommendationButton
                        Layout.fillWidth: true
                        text: "View recommendation"
                        onClicked: root.openRecommendations()

                        background: Rectangle {
                            radius: 18
                            color: "#2D6DCC"
                        }

                        contentItem: Label {
                            text: recommendationButton.text
                            color: "#F4F1EA"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            font.pixelSize: 17
                            font.bold: true
                        }
                    }
                }
            }
        }
    }
}
