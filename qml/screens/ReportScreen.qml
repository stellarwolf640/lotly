import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../components" as Components

Page {
    id: root

    // ── Inputs from Main.qml ──────────────────────────────────────────────────
    property var    lots:              []
    property var    lotData:           ({})
    property string submissionMessage: ""

    // ── Local state ───────────────────────────────────────────────────────────
    property string selectedStatus: "Half"

    // Clear the backend rate-limit error whenever the user picks a new status.
    onSelectedStatusChanged: appController.clearReportError()

    signal backRequested()
    // lotId is now included in the signal so the screen can report the closest lot
    // even when no lot was pre-selected.
    signal submitPressed(string lotId, string statusLabel)

    // ── Closest / displayed lot ───────────────────────────────────────────────
    // If a lot was already selected (came from lot detail), use it.
    // Otherwise auto-select the closest lot from the full list.
    readonly property var displayLot: {
        if (lotData && lotData.id) return lotData
        var closest = null
        var minDist = Infinity
        for (var i = 0; i < lots.length; i++) {
            var d = (lots[i].distanceMeters !== undefined) ? lots[i].distanceMeters : Infinity
            if (d < minDist) { minDist = d; closest = lots[i] }
        }
        return closest || {}
    }

    // ── Helpers ───────────────────────────────────────────────────────────────
    function availColor(pct) {
        if (pct > 50) return "#34A853"
        if (pct > 25) return "#D6A51D"
        return "#D5523F"
    }

    // ── Chart data ────────────────────────────────────────────────────────────
    // 21 values: hour 6 AM (index 0) through 2 AM (index 20).
    // Reads the pre-computed hourlyAvailability from the displayed lot (which
    // already blends real report data with the fallback curve via PredictionEngine).
    // Falls back to a static illustrative curve when the lot isn't loaded yet.
    readonly property var chartData: {
        var d = root.displayLot ? root.displayLot.hourlyAvailability : null
        if (d && d.length >= 21) return d
        return [90, 85, 78, 65, 50, 38, 30, 28, 31, 38,
                48, 58, 65, 68, 66, 70, 78, 84, 89, 92, 95]
    }

    background: Rectangle { color: "#151515" }

    // ── Layout: header → pinned lot card → scrollable content ─────────────────

    // ── Header ────────────────────────────────────────────────────────────────
    Rectangle {
        id: reportHeader
        anchors.top:   parent.top
        anchors.left:  parent.left
        anchors.right: parent.right
        height: 90
        color: "#2D64B3"

        Column {
            anchors.left:    parent.left
            anchors.right:   parent.right
            anchors.bottom:  parent.bottom
            anchors.margins: 20
            spacing: 2

            Label {
                text: "Submit a report"
                color: "#F5F3EE"
                font.pixelSize: 26
                font.bold: true
            }

            Label {
                text: root.displayLot.name || "Select a lot"
                color: "#D5DEEF"
                font.pixelSize: 15
                elide: Text.ElideRight
                width: parent.width
            }
        }
    }

    // ── Pinned closest lot card ───────────────────────────────────────────────
    Rectangle {
        id: closestCard
        anchors.top:   reportHeader.bottom
        anchors.left:  parent.left
        anchors.right: parent.right
        height:        closestCardContent.implicitHeight + 32
        color:         "#222220"
        border.width:  1
        border.color:  "#185FA5"
        visible:       root.displayLot && root.displayLot.id

        ColumnLayout {
            id: closestCardContent
            anchors.top:    parent.top
            anchors.left:   parent.left
            anchors.right:  parent.right
            anchors.margins: 16
            spacing: 10

            // Badge + name
            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                Rectangle {
                    radius: 10
                    color: "#185FA5"
                    implicitWidth:  closestBadge.implicitWidth + 16
                    implicitHeight: 24
                    Label {
                        id: closestBadge
                        anchors.centerIn: parent
                        text: root.lotData && root.lotData.id ? "Selected lot" : "Closest lot"
                        color: "#FFFFFF"
                        font.pixelSize: 12
                        font.bold: true
                    }
                }

                Label {
                    Layout.fillWidth: true
                    text: root.displayLot ? (root.displayLot.name || "") : ""
                    color: "#F3F1EA"
                    font.pixelSize: 18
                    font.bold: true
                    elide: Text.ElideRight
                }
            }

            // Availability bar
            Components.AvailabilityBar {
                Layout.fillWidth: true
                value: root.displayLot ? (root.displayLot.percentOpen || 0) / 100 : 0
                fillColor: root.displayLot ? availColor(root.displayLot.percentOpen || 0) : "#34A853"
                trackColor: "#4A4844"
                barHeight: 10
            }

            // Distance + confidence
            RowLayout {
                Layout.fillWidth: true
                spacing: 14

                Label {
                    text: {
                        if (!root.displayLot) return ""
                        var m = root.displayLot.distanceMeters || 0
                        if (m <= 0) return ""
                        var ft = m * 3.28084
                        return ft < 1000 ? Math.round(ft) + " ft" : (m / 1609.34).toFixed(1) + " mi"
                    }
                    color: "#8A8780"
                    font.pixelSize: 13
                    visible: text.length > 0
                }

                Label {
                    text: root.displayLot ? (root.displayLot.percentOpen || 0) + "% open" : ""
                    color: root.displayLot ? availColor(root.displayLot.percentOpen || 0) : "#34A853"
                    font.pixelSize: 13
                    font.bold: true
                }

                Label {
                    Layout.fillWidth: true
                    text: root.displayLot
                          ? ("Confidence: " + (root.displayLot.confidenceLabel || "med").toLowerCase())
                          : ""
                    color: "#8A8780"
                    font.pixelSize: 13
                    horizontalAlignment: Text.AlignRight
                }
            }
        }
    }

    // ── Scrollable content ────────────────────────────────────────────────────
    ScrollView {
        anchors.top:    closestCard.visible ? closestCard.bottom : reportHeader.bottom
        anchors.left:   parent.left
        anchors.right:  parent.right
        anchors.bottom: parent.bottom
        clip: true
        contentWidth: -1

        ColumnLayout {
            anchors.left:    parent.left
            anchors.right:   parent.right
            anchors.margins: 16
            spacing: 16

            // ── Trend chart ───────────────────────────────────────────────────
            Components.SectionCard {
                Layout.fillWidth: true
                implicitHeight: 220
                cardColor: "#1E1E1C"

                ColumnLayout {
                    anchors.fill:    parent
                    anchors.margins: 12
                    spacing: 6

                    Label {
                        text: "Availability prediction"
                        color: "#D9D5CD"
                        font.pixelSize: 15
                        font.bold: true
                    }

                    // X-axis hour labels — rendered separately above the Canvas
                    // so the Canvas can devote its full height to the chart area.
                    Canvas {
                        id: trendChart
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        property var chartPoints: root.chartData

                        Component.onCompleted: requestPaint()
                        onChartPointsChanged:  requestPaint()

                        onPaint: {
                            var ctx  = getContext("2d")
                            var W    = width, H = height
                            var padL = 6, padR = 6, padT = 8, padB = 28
                            var cW   = W - padL - padR
                            var cH   = H - padT - padB
                            var pts  = chartPoints
                            var n    = pts.length

                            ctx.clearRect(0, 0, W, H)

                            // ── Map data to canvas coords ─────────────────────
                            var xs = [], ys = []
                            for (var i = 0; i < n; i++) {
                                xs.push(padL + (i / (n - 1)) * cW)
                                ys.push(padT + (1 - pts[i] / 100) * cH)
                            }

                            // ── Filled area under the line ────────────────────
                            ctx.beginPath()
                            ctx.moveTo(xs[0], padT + cH)
                            ctx.lineTo(xs[0], ys[0])
                            for (var j = 1; j < n; j++) {
                                var xm = (xs[j - 1] + xs[j]) / 2
                                ctx.bezierCurveTo(xm, ys[j - 1], xm, ys[j], xs[j], ys[j])
                            }
                            ctx.lineTo(xs[n - 1], padT + cH)
                            ctx.closePath()
                            ctx.fillStyle = "rgba(29, 90, 168, 0.18)"
                            ctx.fill()

                            // ── Trend line ────────────────────────────────────
                            ctx.beginPath()
                            ctx.moveTo(xs[0], ys[0])
                            for (var k = 1; k < n; k++) {
                                var xm2 = (xs[k - 1] + xs[k]) / 2
                                ctx.bezierCurveTo(xm2, ys[k - 1], xm2, ys[k], xs[k], ys[k])
                            }
                            ctx.strokeStyle = "#2D6DCC"
                            ctx.lineWidth   = 2
                            ctx.stroke()

                            // ── X-axis labels ─────────────────────────────────
                            // Labels at 6 am (idx 0), 9 am (3), 12 pm (6),
                            //            3 pm (9), 6 pm (12), 9 pm (15), 12 am (18)
                            var labelIdx  = [0,     3,     6,      9,     12,     15,     18]
                            var labelText = ["6am", "9am", "12pm", "3pm", "6pm",  "9pm",  "12am"]
                            ctx.fillStyle  = "#6A6762"
                            ctx.font       = "10px sans-serif"
                            ctx.textAlign  = "center"
                            ctx.textBaseline = "bottom"
                            for (var l = 0; l < labelIdx.length; l++) {
                                ctx.fillText(labelText[l], xs[labelIdx[l]], H - 4)
                            }

                            // ── Current-hour marker ───────────────────────────
                            // Chart covers 6 AM (index 0) → 2 AM (index 20).
                            var nowHour   = new Date().getHours()
                            var markerIdx = (nowHour - 6 + 24) % 24
                            if (markerIdx < 21) {
                                var mx = padL + (markerIdx / (n - 1)) * cW
                                ctx.save()
                                ctx.beginPath()
                                ctx.setLineDash([4, 3])
                                ctx.moveTo(mx, padT)
                                ctx.lineTo(mx, padT + cH)
                                ctx.strokeStyle = "#F0B429"
                                ctx.lineWidth   = 1.5
                                ctx.stroke()
                                ctx.restore()
                            }
                        }
                    }
                }
            }

            // ── Rate this lot ─────────────────────────────────────────────────
            Components.SectionCard {
                Layout.fillWidth: true
                implicitHeight: rateContent.implicitHeight + 36

                ColumnLayout {
                    id: rateContent
                    anchors.top:    parent.top
                    anchors.left:   parent.left
                    anchors.right:  parent.right
                    anchors.margins: 18
                    spacing: 14

                    Label {
                        text: "How full is this lot right now?"
                        color: "#D9D5CD"
                        font.pixelSize: 17
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
                                id: statusBtn
                                required property string modelData
                                Layout.fillWidth: true
                                text: modelData
                                onClicked: root.selectedStatus = modelData

                                background: Rectangle {
                                    radius: 12
                                    color: root.selectedStatus === statusBtn.modelData
                                           ? "#185FA5" : "#343432"
                                    border.width: 1
                                    border.color: root.selectedStatus === statusBtn.modelData
                                                  ? "#185FA5" : "#67645D"
                                }
                                contentItem: Label {
                                    text: statusBtn.text
                                    color: "#F5F3EE"
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment:   Text.AlignVCenter
                                    font.pixelSize: 13
                                    font.bold: root.selectedStatus === statusBtn.modelData
                                }
                            }
                        }
                    }
                }
            }

            // ── Submission feedback ───────────────────────────────────────────
            Components.SectionCard {
                Layout.fillWidth: true
                visible:     root.submissionMessage.length > 0
                cardColor:   "#E6EEF8"
                borderColor: "#D8E2EE"
                radiusValue: 18
                implicitHeight: msgLabel.implicitHeight + 32

                Label {
                    id: msgLabel
                    anchors.fill:    parent
                    anchors.margins: 16
                    wrapMode:        Text.WordWrap
                    text:            root.submissionMessage
                    color:           "#315E9D"
                    font.pixelSize:  15
                }
            }

            // ── Rate-limit / validation error ─────────────────────────────────
            Label {
                Layout.fillWidth: true
                visible:          appController.reportError.length > 0
                text:             appController.reportError
                color:            "#D5523F"
                font.pixelSize:   14
                wrapMode:         Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
            }

            // ── Action buttons ────────────────────────────────────────────────
            RowLayout {
                Layout.fillWidth: true
                spacing: 12

                Button {
                    id: cancelBtn
                    Layout.fillWidth: true
                    text: "Cancel"
                    onClicked: root.backRequested()

                    background: Rectangle {
                        radius: 18; color: "#343432"
                        border.width: 1; border.color: "#67645D"
                    }
                    contentItem: Label {
                        text: cancelBtn.text
                        color: "#D9D5CD"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment:   Text.AlignVCenter
                        font.pixelSize: 17; font.bold: true
                    }
                }

                Button {
                    id: submitBtn
                    Layout.fillWidth: true
                    text: "Submit report"
                    onClicked: root.submitPressed(root.displayLot.id || "", root.selectedStatus)

                    background: Rectangle { radius: 18; color: "#2D6DCC" }
                    contentItem: Label {
                        text: submitBtn.text
                        color: "#F5F3EE"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment:   Text.AlignVCenter
                        font.pixelSize: 17; font.bold: true
                    }
                }
            }

            Item { Layout.preferredHeight: 12 }
        }
    }
}
