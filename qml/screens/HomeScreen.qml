import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../components" as Components

Page {
    id: root

    property var lots: []
    property string currentDateTime: Qt.formatDateTime(new Date(), "dddd h:mm AP")

    signal lotSelected(string lotId)
    signal devToolsRequested()

    // Refresh the clock every 30 s
    Timer {
        interval: 30000
        running: true
        repeat: true
        onTriggered: root.currentDateTime = Qt.formatDateTime(new Date(), "dddd h:mm AP")
    }

    background: Rectangle { color: "#151515" }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // ── Header ────────────────────────────────────────────────────────────
        Rectangle {
            Layout.fillWidth: true
            color: "#2D64B3"
            implicitHeight: 140

            Column {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.margins: 22
                spacing: 6

                RowLayout {
                    width: parent.width

                    Label {
                        Layout.fillWidth: true
                        text: "ParkSmart"
                        color: "#F5F3EE"
                        font.pixelSize: 30
                        font.bold: true
                    }

                    // Dev-tools entry point — subtle button in header corner
                    Rectangle {
                        width: 36
                        height: 24
                        radius: 6
                        color: "#1A4A90"
                        border.width: 1
                        border.color: "#3A6AD4"

                        Label {
                            anchors.centerIn: parent
                            text: "DEV"
                            color: "#8AABDC"
                            font.pixelSize: 10
                            font.bold: true
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: root.devToolsRequested()
                        }
                    }
                }

                Label {
                    text: "Campus \u2014 " + root.currentDateTime
                    color: "#D5DEEF"
                    font.pixelSize: 18
                }
            }
        }

        // ── Location selector ─────────────────────────────────────────────────
        Rectangle {
            Layout.fillWidth: true
            color: "#1E1E1C"
            implicitHeight: locationRow.implicitHeight + 20
            border.width: 1
            border.color: "#3A3A36"

            RowLayout {
                id: locationRow
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                anchors.leftMargin: 18
                anchors.rightMargin: 18
                spacing: 10

                // Location pin icon (styled rectangle placeholder)
                Rectangle {
                    width: 10
                    height: 10
                    radius: 5
                    color: appController.locationLoading ? "#D6A51D" : "#2D6DCC"
                }

                Label {
                    Layout.fillWidth: true
                    text: appController.locationStatus || "No location set"
                    color: "#C8C3B8"
                    font.pixelSize: 14
                    elide: Text.ElideRight
                }

                // "Default" quick-access button
                Button {
                    id: defaultLocBtn
                    text: "Default"
                    implicitHeight: 34
                    onClicked: appController.useDefaultLocation()

                    background: Rectangle {
                        radius: 8
                        color: "#2A2A27"
                        border.width: 1
                        border.color: "#5A5850"
                    }
                    contentItem: Label {
                        text: defaultLocBtn.text
                        color: "#D5D0C6"
                        font.pixelSize: 13
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }

                // "Search" — triggers address search popup
                Button {
                    id: nearMeBtn
                    text: "Search"
                    implicitHeight: 34
                    onClicked: addressPopup.open()

                    background: Rectangle {
                        radius: 8
                        color: "#213358"
                        border.width: 1
                        border.color: "#2D6DCC"
                    }
                    contentItem: Label {
                        text: nearMeBtn.text
                        color: "#8DB8E8"
                        font.pixelSize: 13
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }
        }

        // ── Search bar ────────────────────────────────────────────────────────
        Rectangle {
            Layout.fillWidth: true
            color: "#151515"
            implicitHeight: 74

            Rectangle {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.margins: 18
                radius: 18
                color: "#31312E"
                border.width: 1
                border.color: "#595850"
                height: 50

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 14
                    anchors.rightMargin: 14
                    spacing: 10

                    Label { text: "Q"; color: "#9B988F"; font.pixelSize: 18 }

                    TextField {
                        id: searchField
                        Layout.fillWidth: true
                        placeholderText: "Search lots..."
                        placeholderTextColor: "#8C887E"
                        color: "#F4F1EA"
                        background: Item {}
                    }
                }
            }
        }

        // ── Lot list ──────────────────────────────────────────────────────────
        ListView {
            id: lotList
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: 14
            leftMargin: 18
            rightMargin: 18
            topMargin: 2
            bottomMargin: 18
            model: root.lots.length

            delegate: Item {
                required property int index
                readonly property var lotItem: root.lots[index]
                readonly property bool matchesSearch:
                    searchField.text.length === 0 ||
                    lotItem.name.toLowerCase().indexOf(searchField.text.toLowerCase()) >= 0

                width: lotList.width - lotList.leftMargin - lotList.rightMargin
                height: matchesSearch ? card.implicitHeight : 0
                visible: matchesSearch

                Components.LotCard {
                    id: card
                    width: parent.width
                    lotData: lotItem
                    onClicked: root.lotSelected(lotItem.id)
                }
            }
        }
    }

    // ── Location search popup (with autocomplete) ─────────────────────────────
    Popup {
        id: addressPopup
        anchors.centerIn: parent
        width: 340
        padding: 20
        modal: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        onOpened: {
            addressField.text = ""
            addressField.forceActiveFocus()
            appController.clearAutocomplete()
        }
        onClosed: appController.clearAutocomplete()

        background: Rectangle {
            radius: 18
            color: "#27272A"
            border.width: 1
            border.color: "#4A4A46"
        }

        ColumnLayout {
            width: parent.width
            spacing: 12

            // ── Title ─────────────────────────────────────────────────────────
            Label {
                text: "Search location"
                color: "#F4F1EA"
                font.pixelSize: 18
                font.bold: true
            }

            // ── Input field ───────────────────────────────────────────────────
            Rectangle {
                Layout.fillWidth: true
                height: 46
                radius: 12
                color: "#31312E"
                border.width: 1
                border.color: addressField.activeFocus ? "#2D6DCC" : "#595850"

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 12
                    anchors.rightMargin: 8
                    spacing: 8

                    Label {
                        text: "⌕"
                        color: "#8A8780"
                        font.pixelSize: 18
                    }

                    TextField {
                        id: addressField
                        Layout.fillWidth: true
                        placeholderText: "Address or lat, lng…"
                        placeholderTextColor: "#6B6860"
                        color: "#F4F1EA"
                        background: Item {}
                        font.pixelSize: 15
                        // Debounced autocomplete on every keystroke
                        onTextChanged: appController.updateAutocompleteQuery(text)
                        onAccepted:    searchAddrBtn.doSearch()
                    }

                    // Clear button
                    Label {
                        text: "✕"
                        color: "#6A6762"
                        font.pixelSize: 16
                        visible: addressField.text.length > 0
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                addressField.text = ""
                                appController.clearAutocomplete()
                                addressField.forceActiveFocus()
                            }
                        }
                    }
                }
            }

            // ── Autocomplete suggestions dropdown ─────────────────────────────
            // Appears when the backend returns results; hidden otherwise.
            Rectangle {
                Layout.fillWidth: true
                visible:          appController.autocompleteActive
                implicitHeight:   Math.min(suggestList.contentHeight, 220)
                radius:           10
                color:            "#1E1E1C"
                border.width:     1
                border.color:     "#3A3A36"
                clip:             true

                ListView {
                    id: suggestList
                    anchors.fill: parent
                    model:        appController.autocompleteSuggestions
                    clip:         true
                    boundsBehavior: Flickable.StopAtBounds

                    delegate: Item {
                        required property var    modelData
                        required property int    index
                        width:  suggestList.width
                        height: suggRow.implicitHeight + 18

                        Rectangle {
                            anchors.fill: parent
                            color:        suggMouse.containsMouse ? "#2A2A28" : "transparent"
                        }

                        ColumnLayout {
                            id: suggRow
                            anchors.left:    parent.left
                            anchors.right:   parent.right
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.leftMargin:  14
                            anchors.rightMargin: 14
                            spacing: 2

                            Label {
                                Layout.fillWidth: true
                                text:             modelData.mainText || modelData.fullText
                                color:            "#F0EDE6"
                                font.pixelSize:   14
                                font.bold:        true
                                elide:            Text.ElideRight
                            }
                            Label {
                                visible:          (modelData.secondaryText || "").length > 0
                                Layout.fillWidth: true
                                text:             modelData.secondaryText || ""
                                color:            "#8A8780"
                                font.pixelSize:   12
                                elide:            Text.ElideRight
                            }
                        }

                        // Subtle row divider
                        Rectangle {
                            anchors.bottom: parent.bottom
                            anchors.left:   parent.left
                            anchors.right:  parent.right
                            height:         1
                            color:          "#2E2E2C"
                            visible:        index < suggestList.count - 1
                        }

                        MouseArea {
                            id: suggMouse
                            anchors.fill: parent
                            hoverEnabled: true
                            onClicked: {
                                addressField.text = modelData.fullText
                                appController.selectAutocompleteSuggestion(
                                    modelData.placeId,
                                    modelData.fullText)
                                addressPopup.close()
                            }
                        }
                    }
                }
            }

            // ── Hint label (shown only when no suggestions yet) ───────────────
            Label {
                visible:        !appController.autocompleteActive
                                && addressField.text.length > 0
                                && addressField.text.length < 2
                text:           "Keep typing for suggestions…"
                color:          "#6A6762"
                font.pixelSize: 13
                Layout.alignment: Qt.AlignHCenter
            }

            // ── Action buttons ────────────────────────────────────────────────
            RowLayout {
                Layout.fillWidth: true
                spacing: 12

                Button {
                    id: cancelAddrBtn
                    Layout.fillWidth: true
                    text: "Cancel"
                    onClicked: addressPopup.close()

                    background: Rectangle {
                        radius: 14; color: "#343432"
                        border.width: 1; border.color: "#67645D"
                    }
                    contentItem: Label {
                        text: cancelAddrBtn.text
                        color: "#D9D5CD"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment:   Text.AlignVCenter
                        font.pixelSize: 15; font.bold: true
                    }
                }

                Button {
                    id: searchAddrBtn
                    Layout.fillWidth: true
                    text: "Search"

                    // Falls back to coordinate parsing or direct geocoding
                    // when the user hasn't tapped an autocomplete suggestion.
                    function doSearch() {
                        const raw = addressField.text.trim()
                        if (raw.length === 0) return

                        const parts = raw.split(",")
                        if (parts.length === 2) {
                            const lat = parseFloat(parts[0].trim())
                            const lng = parseFloat(parts[1].trim())
                            if (!isNaN(lat) && !isNaN(lng)
                                    && lat >= -90  && lat <= 90
                                    && lng >= -180 && lng <= 180) {
                                appController.setSearchLocation(lat, lng)
                                addressPopup.close()
                                return
                            }
                        }

                        appController.geocodeAddress(raw)
                        addressPopup.close()
                    }

                    onClicked: doSearch()

                    background: Rectangle { radius: 14; color: "#2D6DCC" }
                    contentItem: Label {
                        text: searchAddrBtn.text
                        color: "#F5F3EE"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment:   Text.AlignVCenter
                        font.pixelSize: 15; font.bold: true
                    }
                }
            }
        }
    }
}
