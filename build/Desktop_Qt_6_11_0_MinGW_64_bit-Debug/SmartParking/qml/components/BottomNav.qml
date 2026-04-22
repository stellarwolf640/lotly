import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

// Bottom navigation matching the concept structure.
Rectangle {
    id: root

    property int currentIndex: 0
    signal tabSelected(int index)

    color: "#30302D"
    border.width: 1
    border.color: "#4F4E48"
    implicitHeight: 92

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        spacing: 10

        Repeater {
            model: [
                { "label": "Lots" },
                { "label": "Report" },
                { "label": "History" },
                { "label": "Settings" }
            ]

            delegate: Item {
                required property var modelData
                required property int index

                Layout.fillWidth: true
                Layout.fillHeight: true

                MouseArea {
                    anchors.fill: parent
                    onClicked: root.tabSelected(index)
                }

                Column {
                    anchors.centerIn: parent
                    spacing: 6

                    Rectangle {
                        anchors.horizontalCenter: parent.horizontalCenter
                        width: 30
                        height: 30
                        radius: 8
                        color: root.currentIndex === index ? "#2D6DCC" : "#5B5952"
                    }

                    Label {
                        text: modelData.label
                        color: root.currentIndex === index ? "#2D6DCC" : "#D7D2C8"
                        font.pixelSize: 14
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
            }
        }
    }
}
