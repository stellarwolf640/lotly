import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

// Bottom navigation — four tabs with inline Canvas SVG icons.
// Active tab: icon + label in blue (#185FA5).  Inactive: muted gray (#7A7770).
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
        anchors.leftMargin: 8
        anchors.rightMargin: 8
        spacing: 0

        Repeater {
            model: [
                { "label": "Home",     "iconType": 0 },
                { "label": "Report",   "iconType": 1 },
                { "label": "Profile",  "iconType": 2 },
                { "label": "Settings", "iconType": 3 }
            ]

            delegate: Item {
                required property var modelData
                required property int index

                Layout.fillWidth: true
                Layout.fillHeight: true

                readonly property bool  active:    root.currentIndex === index
                readonly property color iconColor: active ? "#185FA5" : "#7A7770"

                MouseArea {
                    anchors.fill: parent
                    onClicked: root.tabSelected(index)
                }

                Column {
                    anchors.centerIn: parent
                    spacing: 5

                    // ── Inline icon drawn with Canvas ─────────────────────────
                    Canvas {
                        id: navIcon
                        width: 28
                        height: 28
                        anchors.horizontalCenter: parent.horizontalCenter

                        // Mirror the delegate properties so onPaint can read them directly.
                        property int   iconType:  modelData.iconType
                        property color drawColor: iconColor

                        onDrawColorChanged: requestPaint()
                        Component.onCompleted: requestPaint()

                        onPaint: {
                            var ctx = getContext("2d")
                            ctx.clearRect(0, 0, width, height)
                            var w = width, h = height
                            var dc = drawColor.toString()
                            ctx.fillStyle   = dc
                            ctx.strokeStyle = dc

                            // ── 0: House ─────────────────────────────────────
                            if (iconType === 0) {
                                // Roof — filled triangle
                                ctx.beginPath()
                                ctx.moveTo(w * 0.5, 2)
                                ctx.lineTo(w - 2,   h * 0.52)
                                ctx.lineTo(2,       h * 0.52)
                                ctx.closePath()
                                ctx.fill()
                                // Walls — rectangle below roof
                                ctx.fillRect(5, h * 0.49, w - 10, h * 0.48)

                            // ── 1: Document ───────────────────────────────────
                            } else if (iconType === 1) {
                                // Rounded-rect outline
                                ctx.lineWidth = 1.5
                                var rx = 4, ry = 2, rw = w - 8, rh = h - 4, rr = 3
                                ctx.beginPath()
                                ctx.moveTo(rx + rr, ry)
                                ctx.arcTo(rx + rw, ry,      rx + rw, ry + rh, rr)
                                ctx.arcTo(rx + rw, ry + rh, rx,      ry + rh, rr)
                                ctx.arcTo(rx,      ry + rh, rx,      ry,      rr)
                                ctx.arcTo(rx,      ry,      rx + rw, ry,      rr)
                                ctx.closePath()
                                ctx.stroke()
                                // Text lines
                                var lx = 7, lw2 = w - 14
                                ctx.fillRect(lx, 8,  lw2,       2)
                                ctx.fillRect(lx, 13, lw2,       2)
                                ctx.fillRect(lx, 18, lw2,       2)
                                ctx.fillRect(lx, 23, lw2 * 0.6, 2)

                            // ── 2: Person silhouette ──────────────────────────
                            } else if (iconType === 2) {
                                // Head circle
                                ctx.beginPath()
                                ctx.arc(w * 0.5, 8.5, 5.5, 0, Math.PI * 2)
                                ctx.fill()
                                // Shoulders — filled semi-circle
                                ctx.beginPath()
                                ctx.arc(w * 0.5, h + 2.5, 12, Math.PI, 0, false)
                                ctx.fill()

                            // ── 3: Gear ───────────────────────────────────────
                            } else {
                                // 6-tooth star polygon (alternating outer / inner radius)
                                var cx2 = w * 0.5, cy2 = h * 0.5
                                var Ro = 12, Ri = 8.5, n = 6
                                ctx.beginPath()
                                for (var i = 0; i < n * 2; i++) {
                                    var a = i * Math.PI / n - Math.PI * 0.5
                                    var ra = (i % 2 === 0) ? Ro : Ri
                                    if (i === 0)
                                        ctx.moveTo(cx2 + ra * Math.cos(a), cy2 + ra * Math.sin(a))
                                    else
                                        ctx.lineTo(cx2 + ra * Math.cos(a), cy2 + ra * Math.sin(a))
                                }
                                ctx.closePath()
                                ctx.fill()
                                // Center hole — drawn in the nav-bar background colour
                                ctx.fillStyle = "#30302D"
                                ctx.beginPath()
                                ctx.arc(cx2, cy2, 4.5, 0, Math.PI * 2)
                                ctx.fill()
                            }
                        }
                    }

                    Label {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: modelData.label
                        color: iconColor
                        font.pixelSize: 13
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
            }
        }
    }
}
