import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../components" as Components

// ── ProfileScreen ────────────────────────────────────────────────────────────

Page {
    id: root

    background: Rectangle { color: "#151515" }

    // ── Helpers ───────────────────────────────────────────────────────────────

    // Returns the user's initials (up to two characters).
    function initials(name) {
        var words = (name || "").trim().split(" ")
        if (words.length >= 2 && words[0].length > 0 && words[1].length > 0)
            return words[0][0].toUpperCase() + words[1][0].toUpperCase()
        return words[0] && words[0].length > 0 ? words[0][0].toUpperCase() : "?"
    }

    // Returns badge fill/border color for a report status label.
    function statusBadgeColor(status) {
        switch (status) {
            case "Empty":  return "#2E7D32"   // green
            case "Sparse": return "#388E3C"   // green
            case "Half":   return "#1565C0"   // blue
            case "Busy":   return "#E65100"   // amber
            case "Full":   return "#C62828"   // red
            default:       return "#5A5850"
        }
    }

    property var reportList: appController.userReports

    // ── Header ────────────────────────────────────────────────────────────────
    Rectangle {
        id: profileHeader
        anchors.top:   parent.top
        anchors.left:  parent.left
        anchors.right: parent.right
        height: 90
        color: "#2D64B3"

        Label {
            anchors.left:    parent.left
            anchors.bottom:  parent.bottom
            anchors.margins: 20
            text:  "Profile"
            color: "#F5F3EE"
            font.pixelSize: 28
            font.bold: true
        }
    }

    // ── Scrollable content ────────────────────────────────────────────────────
    ScrollView {
        anchors.top:    profileHeader.bottom
        anchors.left:   parent.left
        anchors.right:  parent.right
        anchors.bottom: parent.bottom
        clip: true
        contentWidth: -1

        ColumnLayout {
            anchors.left:    parent.left
            anchors.right:   parent.right
            anchors.margins: 16
            spacing: 16

            // ── User card ─────────────────────────────────────────────────────
            Components.SectionCard {
                Layout.fillWidth: true
                implicitHeight:   userCardContent.implicitHeight + 40
                cardColor:        "#252523"

                ColumnLayout {
                    id: userCardContent
                    anchors.top:    parent.top
                    anchors.left:   parent.left
                    anchors.right:  parent.right
                    anchors.margins: 20
                    spacing: 16

                    // Profile picture or initials circle
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 16

                        Item {
                            width: 64; height: 64

                            // Initials circle (shown when no photo URL)
                            Rectangle {
                                id: initialsCircle
                                anchors.fill: parent
                                radius: 32
                                color:   "#2D64B3"
                                visible: !profilePhoto.visible

                                Label {
                                    anchors.centerIn: parent
                                    text: root.initials(appController.currentUser.displayName || "")
                                    color: "#FFFFFF"
                                    font.pixelSize: 22
                                    font.bold: true
                                }
                            }

                            // Profile photo (circle-clipped)
                            Rectangle {
                                id: photoClip
                                anchors.fill: parent
                                radius: 32
                                clip:   true
                                color:  "transparent"
                                visible: profilePhoto.visible

                                Image {
                                    id: profilePhoto
                                    anchors.fill: parent
                                    source:   appController.currentUser.photoUrl || ""
                                    fillMode: Image.PreserveAspectCrop
                                    visible:  status === Image.Ready
                                              && (appController.currentUser.photoUrl || "").length > 0
                                }
                            }
                        }

                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 4

                            Label {
                                Layout.fillWidth: true
                                text: appController.currentUser.displayName || "User"
                                color: "#F3F1EA"
                                font.pixelSize: 22
                                font.bold: true
                                elide: Text.ElideRight
                            }

                            Label {
                                Layout.fillWidth: true
                                text: appController.currentUser.email || ""
                                color: "#8A8780"
                                font.pixelSize: 14
                                elide: Text.ElideRight
                            }
                        }
                    }

                    // Reliability score bar
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 6

                        RowLayout {
                            Layout.fillWidth: true

                            Label {
                                text: "Reliability"
                                color: "#B8B2A7"
                                font.pixelSize: 14
                            }

                            Label {
                                Layout.fillWidth: true
                                text: "87"          // TODO: Math.round(appController.currentUser.reliabilityScore * 100)
                                color: "#34A853"
                                font.pixelSize: 14
                                font.bold: true
                                horizontalAlignment: Text.AlignRight
                            }
                        }

                        Components.ProgressBar {
                            Layout.fillWidth: true
                            value:     0.87          // TODO: appController.currentUser.reliabilityScore
                            fillColor: "#34A853"
                            trackColor: "#3A3A36"
                            barHeight: 8
                        }
                    }
                }
            }

            // ── Recent reports ────────────────────────────────────────────────
            Label {
                Layout.topMargin: 4
                text: "Recent reports"
                color: "#E8E5DE"
                font.pixelSize: 18
                font.bold: true
            }

            // Empty state
            Components.SectionCard {
                Layout.fillWidth: true
                implicitHeight:   emptyReportsLabel.implicitHeight + 48
                visible:          root.reportList.length === 0

                Label {
                    id: emptyReportsLabel
                    anchors.centerIn: parent
                    text: "No reports submitted yet"
                    color: "#6A6762"
                    font.pixelSize: 15
                    horizontalAlignment: Text.AlignHCenter
                }
            }

            // Report rows
            Components.SectionCard {
                Layout.fillWidth: true
                implicitHeight:   reportRows.implicitHeight + 8
                visible:          root.reportList.length > 0

                ColumnLayout {
                    id: reportRows
                    anchors.left:    parent.left
                    anchors.right:   parent.right
                    anchors.top:     parent.top
                    anchors.margins: 4
                    spacing: 0

                    Repeater {
                        model: root.reportList.length

                        delegate: Item {
                            required property int index
                            readonly property var entry: root.reportList[index]
                            Layout.fillWidth: true
                            implicitHeight:   reportRowContent.implicitHeight + 18

                            RowLayout {
                                id: reportRowContent
                                anchors.left:           parent.left
                                anchors.right:          parent.right
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.leftMargin:     14
                                anchors.rightMargin:    14
                                spacing: 10

                                ColumnLayout {
                                    Layout.fillWidth: true
                                    spacing: 2

                                    Label {
                                        Layout.fillWidth: true
                                        text: entry.lotName || ""
                                        color: "#E8E5DE"
                                        font.pixelSize: 15
                                        font.bold: true
                                        elide: Text.ElideRight
                                    }

                                    Label {
                                        text: entry.timestamp || ""
                                        color: "#8A8780"
                                        font.pixelSize: 12
                                    }
                                }

                                // Status badge
                                Rectangle {
                                    radius: 8
                                    color: statusBadgeColor(entry.status || "")
                                    implicitWidth:  badgeLabel.implicitWidth + 14
                                    implicitHeight: 24

                                    Label {
                                        id: badgeLabel
                                        anchors.centerIn: parent
                                        text: entry.status || ""
                                        color: "#FFFFFF"
                                        font.pixelSize: 12
                                        font.bold: true
                                    }
                                }
                            }

                            // Row divider
                            Rectangle {
                                anchors.bottom: parent.bottom
                                anchors.left:   parent.left
                                anchors.right:  parent.right
                                height:  1
                                color:   "#2A2A28"
                                visible: index < root.reportList.length - 1
                            }
                        }
                    }
                }
            }

            Item { Layout.preferredHeight: 20 }
        }
    }
}
