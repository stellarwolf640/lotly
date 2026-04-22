import QtQuick
import QtQuick.Controls

// Shared rounded card shell used across all screens.
Frame {
    id: root

    default property alias cardChildren: cardContent.data
    property color cardColor: "#343432"
    property color borderColor: "#53524C"
    property int radiusValue: 24
    property int innerPadding: 18

    padding: 0
    background: Rectangle {
        radius: root.radiusValue
        color: root.cardColor
        border.width: 1
        border.color: root.borderColor
    }

    contentItem: Item {
        id: cardContent
        implicitWidth: 100
        implicitHeight: 100
    }
}
