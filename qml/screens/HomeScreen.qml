import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../components" as Components

Page {
    id: root

    property var lots: []

    signal lotSelected(string lotId)
    signal devToolsRequested()

    // ── Derived lot data ──────────────────────────────────────────────────────

    // First lot with bestPick === true, or the first lot overall.
    readonly property var bestLot: {
        for (var i = 0; i < lots.length; i++) {
            if (lots[i].bestPick) return lots[i]
        }
        return lots.length > 0 ? lots[0] : null
    }

    // All lots except the best lot.
    readonly property var remainingLots: {
        if (!bestLot || lots.length <= 1) return []
        var arr = []
        for (var j = 0; j < lots.length; j++) {
            if (lots[j].id !== bestLot.id) arr.push(lots[j])
        }
        return arr
    }

    // ── Address tracking ──────────────────────────────────────────────────────
    property bool hasCustomAddress: false

    Connections {
        target: appController
        function onSearchLocationNameChanged() {
            if (!appController.searchLocationName)
                hasCustomAddress = false
        }
    }

    // ── Helpers ───────────────────────────────────────────────────────────────
    function availColor(pct) {
        if (pct > 50) return "#34A853"   // green
        if (pct > 25) return "#D6A51D"   // amber
        return "#D5523F"                  // red
    }

    background: Rectangle { color: "#151515" }

    // ─────────────────────────────────────────────────────────────────────────
    // Layout uses absolute anchoring so the autocomplete panel can float above
    // the bottom search bar without fighting a ColumnLayout for space.
    // ─────────────────────────────────────────────────────────────────────────

    // ── Header ────────────────────────────────────────────────────────────────
    Rectangle {
        id: pageHeader
        anchors.top:   parent.top
        anchors.left:  parent.left
        anchors.right: parent.right
        height: 90
        color: "#2D64B3"

        RowLayout {
            anchors.left:    parent.left
            anchors.right:   parent.right
            anchors.bottom:  parent.bottom
            anchors.margins: 20

            Label {
                Layout.fillWidth: true
                text: "Lotly"
                color: "#F5F3EE"
                font.pixelSize: 28
                font.bold: true
            }

            // Dev-tools entry point
            Rectangle {
                width: 36; height: 24; radius: 6
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
    }

    // ── Pinned best lot card ──────────────────────────────────────────────────
    Rectangle {
        id: bestLotCard
        anchors.top:   pageHeader.bottom
        anchors.left:  parent.left
        anchors.right: parent.right
        height:        visible ? bestCardContent.implicitHeight + 32 : 0
        visible:       root.bestLot !== null
        color:         "#222220"
        border.width:  1
        border.color:  "#185FA5"

        MouseArea {
            anchors.fill: parent
            enabled: root.bestLot !== null
            onClicked: {
                if (root.bestLot && root.bestLot.id)
                    root.lotSelected(root.bestLot.id)
            }
        }

        ColumnLayout {
            id: bestCardContent
            anchors.top:   parent.top
            anchors.left:  parent.left
            anchors.right: parent.right
            anchors.margins: 16
            spacing: 10

            // "Best lot" pill + lot name row
            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                // "Best lot" badge
                Rectangle {
                    radius: 10
                    color: "#185FA5"
                    implicitWidth:  bestBadge.implicitWidth + 16
                    implicitHeight: 24

                    Label {
                        id: bestBadge
                        anchors.centerIn: parent
                        text: "Best lot"
                        color: "#FFFFFF"
                        font.pixelSize: 12
                        font.bold: true
                    }
                }

                Label {
                    Layout.fillWidth: true
                    text: root.bestLot ? (root.bestLot.name || "Lot") : ""
                    color: "#F3F1EA"
                    font.pixelSize: 20
                    font.bold: true
                    elide: Text.ElideRight
                }
            }

            // Availability bar
            Components.AvailabilityBar {
                Layout.fillWidth: true
                value: root.bestLot ? (root.bestLot.percentOpen || 0) / 100 : 0
                fillColor: root.bestLot ? availColor(root.bestLot.percentOpen || 0) : "#34A853"
                trackColor: "#4A4844"
                barHeight: 10
            }

            // Distance + confidence row
            RowLayout {
                Layout.fillWidth: true
                spacing: 16

                Label {
                    text: {
                        if (!root.bestLot) return ""
                        var m = root.bestLot.distanceMeters || 0
                        if (m <= 0) return ""
                        var ft = m * 3.28084
                        return ft < 1000 ? Math.round(ft) + " ft" : (m / 1609.34).toFixed(1) + " mi"
                    }
                    color: "#8A8780"
                    font.pixelSize: 14
                    visible: text.length > 0
                }

                Label {
                    text: root.bestLot ? (root.bestLot.percentOpen || 0) + "% open" : ""
                    color: root.bestLot ? availColor(root.bestLot.percentOpen || 0) : "#34A853"
                    font.pixelSize: 14
                    font.bold: true
                }

                Label {
                    Layout.fillWidth: true
                    text: root.bestLot ? ("Confidence: " + (root.bestLot.confidenceLabel || "med").toLowerCase()) : ""
                    color: "#C8C3B8"
                    font.pixelSize: 13
                    horizontalAlignment: Text.AlignRight
                }
            }
        }
    }

    // ── Scrollable remaining lot list ─────────────────────────────────────────
    ListView {
        id: lotList
        anchors.top:    bestLotCard.bottom
        anchors.left:   parent.left
        anchors.right:  parent.right
        anchors.bottom: bottomSearchBar.top
        topMargin:    12
        leftMargin:   16
        rightMargin:  16
        bottomMargin: 12
        spacing:      12
        clip:         true
        visible:      root.lots.length > 0

        model: root.remainingLots.length

        delegate: Rectangle {
            required property int index
            readonly property var lotItem: root.remainingLots[index]

            width:  lotList.width - lotList.leftMargin - lotList.rightMargin
            height: delegateContent.implicitHeight + 32
            radius: 18
            color:  "#2A2A27"
            border.width: 1
            border.color: "#4A4844"

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if (lotItem && lotItem.id)
                        root.lotSelected(lotItem.id)
                }
            }

            ColumnLayout {
                id: delegateContent
                anchors.top:   parent.top
                anchors.left:  parent.left
                anchors.right: parent.right
                anchors.margins: 16
                spacing: 10

                // Lot name
                Label {
                    Layout.fillWidth: true
                    text: lotItem ? (lotItem.name || "Lot") : ""
                    color: "#F3F1EA"
                    font.pixelSize: 18
                    font.bold: true
                    elide: Text.ElideRight
                }

                // Availability bar
                Components.AvailabilityBar {
                    Layout.fillWidth: true
                    value: lotItem ? (lotItem.percentOpen || 0) / 100 : 0
                    fillColor: lotItem ? availColor(lotItem.percentOpen || 0) : "#34A853"
                    trackColor: "#4A4844"
                    barHeight: 8
                }

                // Distance + confidence
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 12

                    Label {
                        text: {
                            if (!lotItem) return ""
                            var m = lotItem.distanceMeters || 0
                            if (m <= 0) return ""
                            var ft = m * 3.28084
                            return ft < 1000 ? Math.round(ft) + " ft" : (m / 1609.34).toFixed(1) + " mi"
                        }
                        color: "#8A8780"
                        font.pixelSize: 13
                        visible: text.length > 0
                    }

                    Label {
                        text: lotItem ? (lotItem.percentOpen || 0) + "% open" : ""
                        color: lotItem ? availColor(lotItem.percentOpen || 0) : "#34A853"
                        font.pixelSize: 13
                        font.bold: true
                    }

                    Label {
                        Layout.fillWidth: true
                        text: lotItem ? ("Confidence: " + (lotItem.confidenceLabel || "med").toLowerCase()) : ""
                        color: "#8A8780"
                        font.pixelSize: 13
                        horizontalAlignment: Text.AlignRight
                    }
                }
            }
        }
    }

    // ── Empty state ───────────────────────────────────────────────────────────
    Column {
        anchors.centerIn: lotList
        visible: root.lots.length === 0 && !appController.locationLoading
        spacing: 20
        width: parent.width - 64

        Rectangle {
            anchors.horizontalCenter: parent.horizontalCenter
            width: 72; height: 72; radius: 36
            color: "#252520"
            border.width: 1; border.color: "#3A3A36"
            Label {
                anchors.centerIn: parent
                text: "P"
                color: "#555248"
                font.pixelSize: 32
                font.bold: true
            }
        }

        Label {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "No parking lots found nearby"
            color: "#D9D5CD"
            font.pixelSize: 18
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            width: parent.width
        }

        Label {
            anchors.horizontalCenter: parent.horizontalCenter
            text: appController.locationStatus
            color: "#8C887E"
            font.pixelSize: 14
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            width: parent.width
        }

        Button {
            id: retryBtn
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Try again"
            implicitHeight: 44; implicitWidth: 160
            onClicked: appController.requestLocationAndSearch()
            background: Rectangle { radius: 14; color: "#2D6DCC" }
            contentItem: Label {
                text: retryBtn.text
                color: "#F5F3EE"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment:   Text.AlignVCenter
                font.pixelSize: 15; font.bold: true
            }
        }
    }

    // Loading spinner
    Column {
        anchors.centerIn: lotList
        visible: root.lots.length === 0 && appController.locationLoading
        spacing: 16

        Rectangle {
            anchors.horizontalCenter: parent.horizontalCenter
            width: 12; height: 12; radius: 6
            color: "#2D6DCC"

            SequentialAnimation on opacity {
                running: parent.visible
                loops: Animation.Infinite
                NumberAnimation { to: 0.2; duration: 700; easing.type: Easing.InOutSine }
                NumberAnimation { to: 1.0; duration: 700; easing.type: Easing.InOutSine }
            }
        }

        Label {
            anchors.horizontalCenter: parent.horizontalCenter
            text: appController.locationStatus
            color: "#8C887E"
            font.pixelSize: 14
            horizontalAlignment: Text.AlignHCenter
        }
    }

    // ── Autocomplete panel — floats above the bottom search bar ──────────────
    Rectangle {
        id: autocompletePanel
        z: 20
        anchors.left:         parent.left
        anchors.right:        parent.right
        anchors.bottom:       bottomSearchBar.top
        height:               visible ? Math.min(acList.contentHeight, 240) : 0
        visible:              appController.autocompleteActive
        color:                "#1E1E1C"
        border.width:         1
        border.color:         "#3A3A36"

        ListView {
            id: acList
            anchors.fill: parent
            clip:         true
            model:        appController.autocompleteSuggestions
            boundsBehavior: Flickable.StopAtBounds

            delegate: Item {
                required property var modelData
                required property int index
                width:  acList.width
                height: acRow.implicitHeight + 16

                Rectangle {
                    anchors.fill: parent
                    color: acMouse.containsMouse ? "#2A2A28" : "transparent"
                }

                ColumnLayout {
                    id: acRow
                    anchors.left:           parent.left
                    anchors.right:          parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin:     14
                    anchors.rightMargin:    14
                    spacing: 2

                    Label {
                        Layout.fillWidth: true
                        text:  modelData.mainText || modelData.fullText
                        color: "#F0EDE6"
                        font.pixelSize: 14
                        font.bold: true
                        elide: Text.ElideRight
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

                Rectangle {
                    anchors.bottom: parent.bottom
                    anchors.left:   parent.left
                    anchors.right:  parent.right
                    height:  1
                    color:   "#2E2E2C"
                    visible: index < acList.count - 1
                }

                MouseArea {
                    id: acMouse
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        appController.selectAutocompleteSuggestion(
                            modelData.placeId, modelData.fullText)
                        root.hasCustomAddress = true
                        addressInput.text = ""
                        addressInput.focus = false
                    }
                }
            }
        }
    }

    // ── Bottom search bar ─────────────────────────────────────────────────────
    Rectangle {
        id: bottomSearchBar
        anchors.left:   parent.left
        anchors.right:  parent.right
        anchors.bottom: parent.bottom
        color:   "#1E1E1C"
        border.width: 1
        border.color: "#3A3A36"
        implicitHeight: searchBarCol.implicitHeight + 20

        Column {
            id: searchBarCol
            anchors.left:           parent.left
            anchors.right:          parent.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.leftMargin:     16
            anchors.rightMargin:    16
            spacing: 8

            // Selected custom address label (shown when a custom address is active)
            RowLayout {
                width: parent.width
                visible: root.hasCustomAddress && appController.searchLocationName.length > 0
                spacing: 6

                Label {
                    Layout.fillWidth: true
                    text: appController.searchLocationName
                    color: "#8A8780"
                    font.pixelSize: 12
                    elide: Text.ElideRight
                }

                // ✕ clear button
                Label {
                    text: "✕"
                    color: "#6A6762"
                    font.pixelSize: 14

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            root.hasCustomAddress = false
                            appController.useDefaultLocation()
                        }
                    }
                }
            }

            // Search input field
            Rectangle {
                width:  parent.width
                height: 46
                radius: 14
                color:  "#2C2C29"
                border.width: 1
                border.color: addressInput.activeFocus ? "#185FA5" : "#4A4844"

                RowLayout {
                    anchors.fill:        parent
                    anchors.leftMargin:  12
                    anchors.rightMargin: 8
                    spacing: 8

                    Label {
                        text: "⌕"
                        color: "#7A7770"
                        font.pixelSize: 18
                    }

                    TextField {
                        id: addressInput
                        Layout.fillWidth: true
                        placeholderText: "Search by address or location…"
                        placeholderTextColor: "#6B6860"
                        color: "#F4F1EA"
                        background: Item {}
                        font.pixelSize: 14
                        onTextChanged: appController.updateAutocompleteQuery(text)
                        onAccepted: {
                            var raw = text.trim()
                            if (raw.length === 0) return
                            var parts = raw.split(",")
                            if (parts.length === 2) {
                                var lat = parseFloat(parts[0].trim())
                                var lng = parseFloat(parts[1].trim())
                                if (!isNaN(lat) && !isNaN(lng)
                                        && lat >= -90 && lat <= 90
                                        && lng >= -180 && lng <= 180) {
                                    appController.setSearchLocation(lat, lng)
                                    root.hasCustomAddress = true
                                    text = ""
                                    return
                                }
                            }
                            appController.geocodeAddress(raw)
                            root.hasCustomAddress = true
                            text = ""
                        }
                    }

                    // Clear input button
                    Label {
                        text: "✕"
                        color: "#6A6762"
                        font.pixelSize: 15
                        visible: addressInput.text.length > 0

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                addressInput.text = ""
                                appController.clearAutocomplete()
                                addressInput.forceActiveFocus()
                            }
                        }
                    }
                }
            }
        }
    }
}
