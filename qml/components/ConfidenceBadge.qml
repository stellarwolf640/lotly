import QtQuick
import QtQuick.Controls

// Reusable confidence badge used across detail and recommendation cards.
Rectangle {
    id: root

    property string labelText: "High"
    property int reportCount: 0
    property bool compact: false

    radius: compact ? 18 : 16
    color: "#E7F1CE"
    implicitWidth: label.implicitWidth + 24
    implicitHeight: label.implicitHeight + 16

    Label {
        id: label
        anchors.centerIn: parent
        text: root.reportCount > 0
              ? root.labelText + " \u2014 " + root.reportCount + (root.compact ? " reports" : "\nrecent reports")
              : root.labelText + " confidence"
        color: "#5B7F2C"
        font.pixelSize: root.compact ? 13 : 14
        font.bold: true
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
