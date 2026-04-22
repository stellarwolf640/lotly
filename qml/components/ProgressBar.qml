import QtQuick

// Reusable styled progress bar for parking availability.
Item {
    id: root

    property real value: 0
    property color fillColor: "#2D6DCC"
    property color trackColor: "#57564E"
    property int barHeight: 10

    implicitHeight: barHeight

    Rectangle {
        anchors.fill: parent
        radius: root.barHeight / 2
        color: root.trackColor
    }

    Rectangle {
        width: Math.max(root.barHeight, parent.width * Math.max(0, Math.min(1, root.value)))
        height: parent.height
        radius: root.barHeight / 2
        color: root.fillColor
    }
}
