import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../components" as Components

// Simple placeholder page for bottom navigation sections not implemented yet.
Page {
    id: root

    property string titleText: ""
    property string subtitleText: ""

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

            Column {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.margins: 20
                spacing: 4

                Label {
                    text: root.titleText
                    color: "#F5F3EE"
                    font.pixelSize: 30
                    font.bold: true
                }

                Label {
                    text: root.subtitleText
                    color: "#D5DEEF"
                    font.pixelSize: 18
                }
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: 18
            spacing: 16

            Components.SectionCard {
                Layout.fillWidth: true
                implicitHeight: 180

                Column {
                    anchors.fill: parent
                    anchors.margins: 18
                    spacing: 12

                    Label {
                        text: root.titleText
                        color: "#F4F1EA"
                        font.pixelSize: 24
                        font.bold: true
                    }

                    Label {
                        width: parent.width
                        wrapMode: Text.WordWrap
                        text: "This section is a styled placeholder for the project demo."
                        color: "#B3AEA4"
                        font.pixelSize: 16
                    }
                }
            }
        }
    }
}
