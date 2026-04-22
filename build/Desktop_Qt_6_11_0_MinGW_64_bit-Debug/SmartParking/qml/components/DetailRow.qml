import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

RowLayout {
    property string labelText: ""
    property string valueText: ""

    spacing: 12

    Label {
        text: labelText
        font.bold: true
        color: "#23404a"
        Layout.preferredWidth: 120
    }

    Label {
        text: valueText
        wrapMode: Text.WordWrap
        color: "#567078"
        Layout.fillWidth: true
    }
}
