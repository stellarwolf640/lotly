import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../components" as Components

// Developer tools panel — inject synthetic reports and inspect predictions.
// Entry point: the small "DEV" button in the HomeScreen header.
Page {
    id: root

    property var lots: []

    signal backRequested()

    background: Rectangle { color: "#151515" }

    // ── Local state ───────────────────────────────────────────────────────────

    property string selectedLotId:   lots.length > 0 ? lots[0].id   : ""
    property string selectedLotName: lots.length > 0 ? lots[0].name : "—"
    property int    startHour: 8
    property int    endHour:   14
    property int    reportCount: 10
    property real   minTrust:   0.40
    property real   maxTrust:   0.90

    // Status weight state (each 0–5)
    property int wEmpty:      1
    property int wModerate:   3
    property int wNearlyFull: 2
    property int wFull:       1

    function buildWeights() {
        var w = []
        if (wEmpty      > 0) w.push("Empty:"      + wEmpty)
        if (wModerate   > 0) w.push("Moderate:"   + wModerate)
        if (wNearlyFull > 0) w.push("NearlyFull:" + wNearlyFull)
        if (wFull       > 0) w.push("Full:"       + wFull)
        return w
    }

    // Refresh the output label whenever simulation manager emits a new summary
    Connections {
        target: appController.simulationManager
        function onLastSummaryChanged() {
            outputLabel.text = appController.simulationManager.lastSummary
            appController.refreshLots()
        }
    }

    // ── Layout ────────────────────────────────────────────────────────────────
    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // Header
        Rectangle {
            Layout.fillWidth: true
            color: "#1A2A1A"
            implicitHeight: 120

            RowLayout {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.margins: 18
                spacing: 14

                ToolButton {
                    onClicked: root.backRequested()
                    background: Item {}
                    contentItem: Label {
                        text: "<"
                        color: "#7DC87D"
                        font.pixelSize: 28
                        font.bold: true
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 4

                    Label {
                        text: "Dev Tools"
                        color: "#A8D8A8"
                        font.pixelSize: 26
                        font.bold: true
                    }

                    Label {
                        text: "Simulation & inspection"
                        color: "#6A8F6A"
                        font.pixelSize: 16
                    }
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
                spacing: 14

                // ── Lot selector ──────────────────────────────────────────────
                Components.SectionCard {
                    Layout.fillWidth: true
                    implicitHeight: lotSelectorLayout.implicitHeight + 36
                    cardColor: "#1E221E"
                    borderColor: "#3A4A3A"

                    ColumnLayout {
                        id: lotSelectorLayout
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.margins: 18
                        spacing: 12

                        Label {
                            text: "Target lot"
                            color: "#8BB88B"
                            font.pixelSize: 15
                            font.bold: true
                        }

                        // Simple button row — tap to select target lot
                        Flow {
                            Layout.fillWidth: true
                            spacing: 8

                            Repeater {
                                model: root.lots.length

                                delegate: Button {
                                    required property int index
                                    readonly property var lotItem: root.lots[index]
                                    readonly property bool isSelected: lotItem.id === root.selectedLotId

                                    text: lotItem.name
                                    onClicked: {
                                        root.selectedLotId   = lotItem.id
                                        root.selectedLotName = lotItem.name
                                    }

                                    background: Rectangle {
                                        radius: 12
                                        color: isSelected ? "#2A4A2A" : "#242422"
                                        border.width: 1
                                        border.color: isSelected ? "#5AAA5A" : "#4A4A44"
                                    }
                                    contentItem: Label {
                                        text: parent.text
                                        color: parent.isSelected ? "#88CC88" : "#888882"
                                        font.pixelSize: 13
                                        horizontalAlignment: Text.AlignHCenter
                                        verticalAlignment: Text.AlignVCenter
                                    }
                                }
                            }
                        }
                    }
                }

                // ── Time window ───────────────────────────────────────────────
                Components.SectionCard {
                    Layout.fillWidth: true
                    implicitHeight: timeLayout.implicitHeight + 36
                    cardColor: "#1E221E"
                    borderColor: "#3A4A3A"

                    ColumnLayout {
                        id: timeLayout
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.margins: 18
                        spacing: 14

                        Label {
                            text: "Time window: %1h \u2013 %2h".arg(root.startHour).arg(root.endHour)
                            color: "#8BB88B"
                            font.pixelSize: 15
                            font.bold: true
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 12

                            Label { text: "Start"; color: "#6A8F6A"; font.pixelSize: 13; Layout.preferredWidth: 36 }

                            Slider {
                                id: startSlider
                                Layout.fillWidth: true
                                from: 0; to: 23; stepSize: 1
                                value: root.startHour
                                onMoved: root.startHour = Math.min(value, root.endHour - 1)
                            }
                            Label { text: root.startHour + "h"; color: "#A8D8A8"; font.pixelSize: 13; Layout.preferredWidth: 28 }
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 12

                            Label { text: "End"; color: "#6A8F6A"; font.pixelSize: 13; Layout.preferredWidth: 36 }

                            Slider {
                                id: endSlider
                                Layout.fillWidth: true
                                from: 1; to: 24; stepSize: 1
                                value: root.endHour
                                onMoved: root.endHour = Math.max(value, root.startHour + 1)
                            }
                            Label { text: root.endHour + "h"; color: "#A8D8A8"; font.pixelSize: 13; Layout.preferredWidth: 28 }
                        }
                    }
                }

                // ── Report count ──────────────────────────────────────────────
                Components.SectionCard {
                    Layout.fillWidth: true
                    implicitHeight: countLayout.implicitHeight + 36
                    cardColor: "#1E221E"
                    borderColor: "#3A4A3A"

                    ColumnLayout {
                        id: countLayout
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.margins: 18
                        spacing: 10

                        Label {
                            text: "Reports to inject: " + root.reportCount
                            color: "#8BB88B"
                            font.pixelSize: 15
                            font.bold: true
                        }

                        Slider {
                            Layout.fillWidth: true
                            from: 1; to: 50; stepSize: 1
                            value: root.reportCount
                            onMoved: root.reportCount = value
                        }
                    }
                }

                // ── Trust score range ─────────────────────────────────────────
                Components.SectionCard {
                    Layout.fillWidth: true
                    implicitHeight: trustLayout.implicitHeight + 36
                    cardColor: "#1E221E"
                    borderColor: "#3A4A3A"

                    ColumnLayout {
                        id: trustLayout
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.margins: 18
                        spacing: 14

                        Label {
                            text: "Trust score range: %1 \u2013 %2"
                                  .arg(root.minTrust.toFixed(2))
                                  .arg(root.maxTrust.toFixed(2))
                            color: "#8BB88B"
                            font.pixelSize: 15
                            font.bold: true
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 12
                            Label { text: "Min"; color: "#6A8F6A"; font.pixelSize: 13; Layout.preferredWidth: 28 }
                            Slider {
                                Layout.fillWidth: true
                                from: 0.1; to: 1.0; stepSize: 0.05
                                value: root.minTrust
                                onMoved: root.minTrust = Math.min(value, root.maxTrust - 0.05)
                            }
                            Label { text: root.minTrust.toFixed(2); color: "#A8D8A8"; font.pixelSize: 13 }
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 12
                            Label { text: "Max"; color: "#6A8F6A"; font.pixelSize: 13; Layout.preferredWidth: 28 }
                            Slider {
                                Layout.fillWidth: true
                                from: 0.1; to: 1.0; stepSize: 0.05
                                value: root.maxTrust
                                onMoved: root.maxTrust = Math.max(value, root.minTrust + 0.05)
                            }
                            Label { text: root.maxTrust.toFixed(2); color: "#A8D8A8"; font.pixelSize: 13 }
                        }
                    }
                }

                // ── Status distribution ───────────────────────────────────────
                Components.SectionCard {
                    Layout.fillWidth: true
                    implicitHeight: statusLayout.implicitHeight + 36
                    cardColor: "#1E221E"
                    borderColor: "#3A4A3A"

                    ColumnLayout {
                        id: statusLayout
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.margins: 18
                        spacing: 12

                        Label {
                            text: "Status distribution (weights)"
                            color: "#8BB88B"
                            font.pixelSize: 15
                            font.bold: true
                        }

                        Repeater {
                            model: [
                                { label: "Empty",       prop: "wEmpty",      color: "#2D6DCC" },
                                { label: "Moderate",    prop: "wModerate",   color: "#D6A51D" },
                                { label: "Nearly Full", prop: "wNearlyFull", color: "#D08030" },
                                { label: "Full",        prop: "wFull",       color: "#D5523F" }
                            ]

                            delegate: RowLayout {
                                required property var modelData
                                Layout.fillWidth: true
                                spacing: 10

                                Rectangle {
                                    width: 10; height: 10; radius: 5
                                    color: modelData.color
                                }

                                Label {
                                    Layout.preferredWidth: 90
                                    text: modelData.label
                                    color: "#B8B3A8"
                                    font.pixelSize: 14
                                }

                                // Minus button
                                Button {
                                    id: minusBtn
                                    implicitWidth: 32; implicitHeight: 32
                                    text: "\u2212"
                                    onClicked: {
                                        if (modelData.prop === "wEmpty"      && root.wEmpty      > 0) root.wEmpty--
                                        if (modelData.prop === "wModerate"   && root.wModerate   > 0) root.wModerate--
                                        if (modelData.prop === "wNearlyFull" && root.wNearlyFull > 0) root.wNearlyFull--
                                        if (modelData.prop === "wFull"       && root.wFull       > 0) root.wFull--
                                    }
                                    background: Rectangle { radius: 8; color: "#2A2A27"; border.width:1; border.color:"#5A5850" }
                                    contentItem: Label { text: minusBtn.text; color:"#C0BBB0"; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter; font.pixelSize:18 }
                                }

                                Label {
                                    Layout.preferredWidth: 24
                                    text: modelData.prop === "wEmpty"      ? root.wEmpty
                                        : modelData.prop === "wModerate"   ? root.wModerate
                                        : modelData.prop === "wNearlyFull" ? root.wNearlyFull
                                        : root.wFull
                                    color: "#E8E3D8"
                                    font.pixelSize: 16
                                    font.bold: true
                                    horizontalAlignment: Text.AlignHCenter
                                }

                                // Plus button
                                Button {
                                    id: plusBtn
                                    implicitWidth: 32; implicitHeight: 32
                                    text: "+"
                                    onClicked: {
                                        if (modelData.prop === "wEmpty")      root.wEmpty++
                                        if (modelData.prop === "wModerate")   root.wModerate++
                                        if (modelData.prop === "wNearlyFull") root.wNearlyFull++
                                        if (modelData.prop === "wFull")       root.wFull++
                                    }
                                    background: Rectangle { radius: 8; color: "#2A2A27"; border.width:1; border.color:"#5A5850" }
                                    contentItem: Label { text: plusBtn.text; color:"#C0BBB0"; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter; font.pixelSize:18 }
                                }
                            }
                        }
                    }
                }

                // ── Action buttons ────────────────────────────────────────────
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 12

                    Button {
                        id: clearBtn
                        Layout.fillWidth: true
                        text: "Clear sim data"
                        onClicked: {
                            appController.simulationManager.clearSimulation(root.selectedLotId)
                        }

                        background: Rectangle {
                            radius: 16; color: "#2A2218"
                            border.width: 1; border.color: "#8A6030"
                        }
                        contentItem: Label {
                            text: clearBtn.text
                            color: "#C89050"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            font.pixelSize: 15; font.bold: true
                        }
                    }

                    Button {
                        id: runBtn
                        Layout.fillWidth: true
                        text: "Run simulation"
                        onClicked: {
                            appController.simulationManager.runSimulation(
                                root.selectedLotId,
                                root.startHour,
                                root.endHour,
                                root.reportCount,
                                root.minTrust,
                                root.maxTrust,
                                root.buildWeights()
                            )
                        }

                        background: Rectangle {
                            radius: 16; color: "#1A3A1A"
                            border.width: 1; border.color: "#5AAA5A"
                        }
                        contentItem: Label {
                            text: runBtn.text
                            color: "#88DD88"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            font.pixelSize: 15; font.bold: true
                        }
                    }
                }

                // ── Output panel ──────────────────────────────────────────────
                Components.SectionCard {
                    Layout.fillWidth: true
                    implicitHeight: outputCol.implicitHeight + 36
                    cardColor: "#191C19"
                    borderColor: "#2A3A2A"

                    ColumnLayout {
                        id: outputCol
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.margins: 18
                        spacing: 10

                        Label {
                            text: "Output"
                            color: "#6A8F6A"
                            font.pixelSize: 14
                            font.bold: true
                        }

                        Label {
                            id: outputLabel
                            Layout.fillWidth: true
                            text: "Run a simulation to see results here."
                            color: "#A8D8A8"
                            font.pixelSize: 14
                            wrapMode: Text.WordWrap
                        }

                        // Live prediction for selected lot (updates after simulation)
                        Label {
                            id: predictionLabel
                            Layout.fillWidth: true
                            wrapMode: Text.WordWrap
                            font.pixelSize: 13
                            color: "#6AAA8A"
                            text: {
                                if (!root.selectedLotId) return ""
                                // Find the lot in the live list
                                for (var i = 0; i < root.lots.length; i++) {
                                    if (root.lots[i].id === root.selectedLotId) {
                                        var l = root.lots[i]
                                        return "\u2014 " + l.name + ": "
                                             + l.percentOpen + "% open | "
                                             + "conf " + (l.confidence * 100).toFixed(0) + "% | "
                                             + l.explanation
                                    }
                                }
                                return ""
                            }
                        }
                    }
                }

                // Bottom spacer
                Item { Layout.preferredHeight: 18 }
            }
        }
    }
}
