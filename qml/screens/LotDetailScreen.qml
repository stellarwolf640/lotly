import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../components" as Components

Page {
    id: root

    property var    lotData:        ({})
    property string selectedStatus: "Half"

    // Clear the backend error whenever the user changes their rating selection.
    onSelectedStatusChanged: appController.clearReportError()

    signal backRequested()
    signal openRecommendations()
    // openReport() kept for signal compatibility — submission is now inline.
    signal openReport()

    // ── Helpers ───────────────────────────────────────────────────────────────
    function availColor(pct) {
        if (pct > 50) return "#34A853"
        if (pct > 25) return "#D6A51D"
        return "#D5523F"
    }

    // ── Chart data ────────────────────────────────────────────────────────────
    // 21 values: hour 6 AM (index 0) through 2 AM (index 20).
    // Uses the lot's pre-computed hourly curve when available, otherwise falls
    // back to a hard-coded illustrative sample.
    readonly property var chartData: {
        var d = root.lotData.hourlyAvailability
        if (d && d.length >= 21) return d
        return [90, 85, 78, 65, 50, 38, 30, 28, 31, 38,
                48, 58, 65, 68, 66, 70, 78, 84, 89, 92, 95]
    }

    background: Rectangle { color: "#151515" }

    // ── Header ────────────────────────────────────────────────────────────────
    Rectangle {
        id: detailHeader
        anchors.top:   parent.top
        anchors.left:  parent.left
        anchors.right: parent.right
        height: 90
        color: "#2D64B3"

        RowLayout {
            anchors.left:    parent.left
            anchors.right:   parent.right
            anchors.bottom:  parent.bottom
            anchors.margins: 16
            spacing: 8

            ToolButton {
                onClicked: root.backRequested()
                background: Item {}
                contentItem: Label {
                    text: "‹"      // ‹
                    color: "#F5F3EE"
                    font.pixelSize: 34
                    font.bold: true
                    verticalAlignment: Text.AlignVCenter
                }
            }

            Column {
                Layout.fillWidth: true
                spacing: 2

                Label {
                    text: "Lot details"
                    color: "#F5F3EE"
                    font.pixelSize: 26
                    font.bold: true
                }

                Label {
                    text: root.lotData.name || ""
                    color: "#D5DEEF"
                    font.pixelSize: 15
                    elide: Text.ElideRight
                    width: parent.width
                }
            }
        }
    }

    // ── Pinned lot card ───────────────────────────────────────────────────────
    Rectangle {
        id: lotCard
        anchors.top:   detailHeader.bottom
        anchors.left:  parent.left
        anchors.right: parent.right
        height:        lotCardContent.implicitHeight + 32
        color:         "#222220"
        border.width:  1
        border.color:  "#185FA5"
        visible:       root.lotData && root.lotData.id

        ColumnLayout {
            id: lotCardContent
            anchors.top:    parent.top
            anchors.left:   parent.left
            anchors.right:  parent.right
            anchors.margins: 16
            spacing: 10

            // Badge + lot name
            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                Rectangle {
                    radius: 10
                    color: "#185FA5"
                    implicitWidth:  lotBadgeLabel.implicitWidth + 16
                    implicitHeight: 24

                    Label {
                        id: lotBadgeLabel
                        anchors.centerIn: parent
                        text: "Selected lot"
                        color: "#FFFFFF"
                        font.pixelSize: 12
                        font.bold: true
                    }
                }

                Label {
                    Layout.fillWidth: true
                    text: root.lotData.name || ""
                    color: "#F3F1EA"
                    font.pixelSize: 18
                    font.bold: true
                    elide: Text.ElideRight
                }
            }

            // Availability bar
            Components.AvailabilityBar {
                Layout.fillWidth: true
                value:      (root.lotData.percentOpen || 0) / 100
                fillColor:  availColor(root.lotData.percentOpen || 0)
                trackColor: "#4A4844"
                barHeight:  10
            }

            // Distance + percent open + confidence
            RowLayout {
                Layout.fillWidth: true
                spacing: 14

                Label {
                    text: {
                        var m = root.lotData.distanceMeters || 0
                        if (m <= 0) return ""
                        var ft = m * 3.28084
                        return ft < 1000 ? Math.round(ft) + " ft"
                                         : (m / 1609.34).toFixed(1) + " mi"
                    }
                    color: "#8A8780"
                    font.pixelSize: 13
                    visible: text.length > 0
                }

                Label {
                    text: (root.lotData.percentOpen || 0) + "% open"
                    color: availColor(root.lotData.percentOpen || 0)
                    font.pixelSize: 13
                    font.bold: true
                }

                Label {
                    Layout.fillWidth: true
                    text: "Confidence: " + (root.lotData.confidenceLabel || "med").toLowerCase()
                    color: "#8A8780"
                    font.pixelSize: 13
                    horizontalAlignment: Text.AlignRight
                }
            }
        }
    }

    // ── Scrollable content ────────────────────────────────────────────────────
    ScrollView {
        anchors.top:    lotCard.visible ? lotCard.bottom : detailHeader.bottom
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

            // ── Bezier trend chart ────────────────────────────────────────────
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

                            // Map data → canvas coords
                            var xs = [], ys = []
                            for (var i = 0; i < n; i++) {
                                xs.push(padL + (i / (n - 1)) * cW)
                                ys.push(padT + (1 - pts[i] / 100) * cH)
                            }

                            // Filled area under curve
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

                            // Trend line
                            ctx.beginPath()
                            ctx.moveTo(xs[0], ys[0])
                            for (var k = 1; k < n; k++) {
                                var xm2 = (xs[k - 1] + xs[k]) / 2
                                ctx.bezierCurveTo(xm2, ys[k - 1], xm2, ys[k], xs[k], ys[k])
                            }
                            ctx.strokeStyle = "#2D6DCC"
                            ctx.lineWidth   = 2
                            ctx.stroke()

                            // X-axis labels at 6 am, 9 am, 12 pm, 3 pm, 6 pm, 9 pm, 12 am
                            var labelIdx  = [0,     3,     6,      9,     12,     15,     18]
                            var labelText = ["6am", "9am", "12pm", "3pm", "6pm",  "9pm",  "12am"]
                            ctx.fillStyle    = "#6A6762"
                            ctx.font         = "10px sans-serif"
                            ctx.textAlign    = "center"
                            ctx.textBaseline = "bottom"
                            for (var l = 0; l < labelIdx.length; l++)
                                ctx.fillText(labelText[l], xs[labelIdx[l]], H - 4)

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

            // ── Address ───────────────────────────────────────────────────────
            Components.SectionCard {
                Layout.fillWidth: true
                visible: (root.lotData.address || "").length > 0
                implicitHeight: addrContent.implicitHeight + 32

                ColumnLayout {
                    id: addrContent
                    anchors.top:    parent.top
                    anchors.left:   parent.left
                    anchors.right:  parent.right
                    anchors.margins: 16
                    spacing: 4

                    Label {
                        text: "Address"
                        color: "#9B988F"
                        font.pixelSize: 13
                        font.bold: true
                    }

                    Label {
                        Layout.fillWidth: true
                        text: root.lotData.address || ""
                        color: "#D9D5CD"
                        font.pixelSize: 15
                        wrapMode: Text.WordWrap
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
                visible:     appController.lastSubmissionMessage.length > 0
                cardColor:   "#E6EEF8"
                borderColor: "#D8E2EE"
                radiusValue: 18
                implicitHeight: feedbackLabel.implicitHeight + 32

                Label {
                    id: feedbackLabel
                    anchors.fill:    parent
                    anchors.margins: 16
                    wrapMode:        Text.WordWrap
                    text:            appController.lastSubmissionMessage
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
                    id: recBtn
                    Layout.fillWidth: true
                    text: "Recommend"
                    onClicked: root.openRecommendations()

                    background: Rectangle {
                        radius: 18; color: "#343432"
                        border.width: 1; border.color: "#67645D"
                    }
                    contentItem: Label {
                        text: recBtn.text
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
                    onClicked: appController.submitReport(root.lotData.id || "",
                                                         root.selectedStatus)

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
