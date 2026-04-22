import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../components" as Components

Page {
    id: root

    property var lotData: ({})
    property string submissionMessage: ""
    property string selectedStatus: "Half"
    property bool autoPromptOnExit: true

    signal backRequested()
    signal submitPressed(string statusLabel)

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
                    text: "Submit a report"
                    color: "#F5F3EE"
                    font.pixelSize: 30
                    font.bold: true
                }

                Label {
                    text: root.lotData.name || "Lot A -- North"
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
                    implicitHeight: 168

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 18
                        spacing: 16

                        Label {
                            text: "How full is this lot right now?"
                            color: "#D9D5CD"
                            font.pixelSize: 18
                            font.bold: true
                        }

                        GridLayout {
                            Layout.fillWidth: true
                            columns: 5
                            columnSpacing: 8
                            rowSpacing: 8

                            Repeater {
                                model: ["Empty", "Sparse", "Half", "Busy", "Full"]

                                delegate: Button {
                                    id: statusButton
                                    required property string modelData
                                    Layout.fillWidth: true
                                    text: modelData
                                    onClicked: root.selectedStatus = modelData

                                    background: Rectangle {
                                        radius: 14
                                        color: root.selectedStatus === statusButton.modelData ? "#2D6DCC" : "#343432"
                                        border.width: 1
                                        border.color: root.selectedStatus === statusButton.modelData ? "#2D6DCC" : "#67645D"
                                    }

                                    contentItem: Label {
                                        text: statusButton.text
                                        color: "#F5F3EE"
                                        horizontalAlignment: Text.AlignHCenter
                                        verticalAlignment: Text.AlignVCenter
                                        font.pixelSize: 14
                                        font.bold: root.selectedStatus === statusButton.modelData
                                    }
                                }
                            }
                        }
                    }
                }

                Components.SectionCard {
                    Layout.fillWidth: true
                    implicitHeight: 132

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 18
                        spacing: 14

                        Label {
                            text: "87"
                            color: "#2D6DCC"
                            font.pixelSize: 48
                            font.bold: true
                        }

                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 4

                            Label {
                                text: "Your reliability score"
                                color: "#D9D5CD"
                                font.pixelSize: 18
                                font.bold: true
                            }

                            Label {
                                text: "Trusted reporter"
                                color: "#F5F3EE"
                                font.pixelSize: 18
                                font.bold: true
                            }

                            Label {
                                text: "23 reports submitted"
                                color: "#B8B2A7"
                                font.pixelSize: 16
                            }
                        }
                    }
                }

                Components.SectionCard {
                    Layout.fillWidth: true
                    implicitHeight: 114

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 18
                        spacing: 14

                        Label {
                            text: "Post-exit prompt"
                            color: "#D9D5CD"
                            font.pixelSize: 18
                            font.bold: true
                        }

                        RowLayout {
                            Layout.fillWidth: true

                            Label {
                                Layout.fillWidth: true
                                text: "Auto-prompt on exit"
                                color: "#F5F3EE"
                                font.pixelSize: 16
                            }

                            Switch {
                                checked: root.autoPromptOnExit
                                onToggled: root.autoPromptOnExit = checked
                            }
                        }
                    }
                }

                Components.SectionCard {
                    Layout.fillWidth: true
                    visible: root.submissionMessage.length > 0
                    cardColor: "#E6EEF8"
                    borderColor: "#D8E2EE"
                    radiusValue: 18
                    implicitHeight: infoLabel.implicitHeight + 32

                    Label {
                        id: infoLabel
                        anchors.fill: parent
                        anchors.margins: 16
                        wrapMode: Text.WordWrap
                        text: root.submissionMessage
                        color: "#315E9D"
                        font.pixelSize: 15
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 12

                    Button {
                        id: cancelButton
                        Layout.fillWidth: true
                        text: "Cancel"
                        onClicked: root.backRequested()

                        background: Rectangle {
                            radius: 18
                            color: "#343432"
                            border.width: 1
                            border.color: "#67645D"
                        }

                        contentItem: Label {
                            text: cancelButton.text
                            color: "#D9D5CD"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            font.pixelSize: 18
                            font.bold: true
                        }
                    }

                    Button {
                        id: submitButton
                        Layout.fillWidth: true
                        text: "Submit report"
                        onClicked: root.submitPressed(root.selectedStatus)

                        background: Rectangle {
                            radius: 18
                            color: "#2D6DCC"
                        }

                        contentItem: Label {
                            text: submitButton.text
                            color: "#F5F3EE"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            font.pixelSize: 18
                            font.bold: true
                        }
                    }
                }
            }
        }
    }
}
