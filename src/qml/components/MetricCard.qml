import QtQuick
import QtQuick.Layouts

Item {
    id: card

    // ── API ──────────────────────────────────────────────────────────
    property string label:    ""
    property string unit:     "%"
    property real   value:    0.0      // current value (e.g. 54.2)
    property real   maxValue: 100.0   // for ratio calculation
    property real   ratio:    value / maxValue  // 0.0–1.0
    property string subtitle: ""      // optional second line e.g. "6.2 / 32 GiB"
    property color  accentColor: Theme.metricColor(ratio)

    // ── Geometry ──────────────────────────────────────────────────────
    implicitWidth:  240
    implicitHeight: 140

    // ── Pulse animation on value change ───────────────────────────────
    onValueChanged: pulseAnim.restart()

    // ── Card background ───────────────────────────────────────────────
    Rectangle {
        id: bg
        anchors.fill: parent
        radius:       Theme.cardRadius
        color:        Theme.bgSurface

        border.color: card.accentColor
        border.width: 1

        Behavior on border.color {
            ColorAnimation { duration: Theme.animNormal }
        }

        // Pulse glow overlay
        Rectangle {
            id: pulseOverlay
            anchors.fill: parent
            radius:       Theme.cardRadius
            color:        card.accentColor
            opacity:      0

            Behavior on color {
                ColorAnimation { duration: Theme.animNormal }
            }
        }

        SequentialAnimation {
            id: pulseAnim
            NumberAnimation {
                target:   pulseOverlay
                property: "opacity"
                to:       0.08
                duration: Theme.pulseAnimDuration / 2
                easing.type: Easing.OutQuad
            }
            NumberAnimation {
                target:   pulseOverlay
                property: "opacity"
                to:       0
                duration: Theme.pulseAnimDuration / 2
                easing.type: Easing.InQuad
            }
        }

        // ── Content ───────────────────────────────────────────────────
        Column {
            anchors.fill:    parent
            anchors.margins: Theme.cardPadding
            spacing:         Theme.spaceSM

            // Label
            Text {
                text:           card.label
                font.pixelSize: Theme.fontSizeSM
                font.weight:    Font.Medium
                color:          Theme.textSecondary
                font.letterSpacing: 1.2
            }

            // Animated value
            Row {
                spacing: 4
                anchors.left: parent.left

                Text {
                    id: valueText
                    font.pixelSize: Theme.fontSizeXXL
                    font.weight:    Font.Light
                    color:          card.accentColor

                    Behavior on color {
                        ColorAnimation { duration: Theme.animNormal }
                    }

                    // Smooth number animation
                    property real displayValue: card.value
                    Behavior on displayValue {
                        NumberAnimation {
                            duration: Theme.numberAnimDuration
                            easing.type: Easing.OutCubic
                        }
                    }
                    onDisplayValueChanged: text = displayValue.toFixed(1)
                    Component.onCompleted: text = displayValue.toFixed(1)
                }

                Text {
                    text:           card.unit
                    font.pixelSize: Theme.fontSizeMD
                    color:          Theme.textSecondary
                    anchors.baseline: valueText.baseline
                }
            }

            // Progress bar
            Rectangle {
                width:  parent.width
                height: 4
                radius: 2
                color:  Theme.borderSubtle

                Rectangle {
                    width:  parent.width * Math.min(card.ratio, 1.0)
                    height: parent.height
                    radius: parent.radius
                    color:  card.accentColor

                    Behavior on width {
                        NumberAnimation {
                            duration: Theme.numberAnimDuration
                            easing.type: Easing.OutCubic
                        }
                    }
                    Behavior on color {
                        ColorAnimation { duration: Theme.animNormal }
                    }
                }
            }

            // Subtitle
            Text {
                text:           card.subtitle
                font.pixelSize: Theme.fontSizeXS
                color:          Theme.textMuted
                visible:        card.subtitle !== ""
            }
        }
    }
}
