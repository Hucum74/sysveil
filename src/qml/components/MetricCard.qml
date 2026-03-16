import QtQuick
import QtQuick.Layouts

Item {
    id: card

    // ── API ──────────────────────────────────────────────────────────
    property string label:      ""
    property string unit:       "%"
    property real   value:      0.0
    property real   maxValue:   100.0
    property real   ratio:      maxValue > 0 ? value / maxValue : 0
    property string subtitle:   ""
    property var    details:    []    // array of {label, value} maps
    property color  accentColor: Theme.metricColor(ratio)

    implicitWidth:  240
    implicitHeight: contentCol.implicitHeight + Theme.cardPadding * 2

    onValueChanged: pulseAnim.restart()

    // ── Card background ───────────────────────────────────────────────
    Rectangle {
        anchors.fill: parent
        radius:       Theme.cardRadius
        color:        Theme.bgSurface
        border.color: card.accentColor
        border.width: 1

        Behavior on border.color {
            ColorAnimation { duration: Theme.animNormal }
        }

        // Pulse overlay
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
                target: pulseOverlay; property: "opacity"
                to: 0.08; duration: Theme.pulseAnimDuration / 2
                easing.type: Easing.OutQuad
            }
            NumberAnimation {
                target: pulseOverlay; property: "opacity"
                to: 0; duration: Theme.pulseAnimDuration / 2
                easing.type: Easing.InQuad
            }
        }

        // ── Content ───────────────────────────────────────────────────
        Column {
            id: contentCol
            anchors.left:    parent.left
            anchors.right:   parent.right
            anchors.top:     parent.top
            anchors.margins: Theme.cardPadding
            spacing:         Theme.spaceSM

            // Label
            Text {
                text:               card.label
                font.pixelSize:     Theme.fontSizeXS
                font.weight:        Font.Medium
                font.letterSpacing: 1.5
                color:              Theme.textSecondary
            }

            // Value + unit
            Row {
                spacing: 3
                Text {
                    id: valueText
                    font.pixelSize: Theme.fontSizeXXL
                    font.weight:    Font.Light
                    color:          card.accentColor
                    Behavior on color {
                        ColorAnimation { duration: Theme.animNormal }
                    }
                    property real displayValue: card.value
                    Behavior on displayValue {
                        NumberAnimation {
                            duration:    Theme.numberAnimDuration
                            easing.type: Easing.OutCubic
                        }
                    }
                    onDisplayValueChanged: text = displayValue.toFixed(1)
                    Component.onCompleted:  text = displayValue.toFixed(1)
                }
                Text {
                    text:  card.unit
                    font.pixelSize: Theme.fontSizeSM
                    color: Theme.textSecondary
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
                            duration:    Theme.numberAnimDuration
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

            // Detail rows
            Column {
                width:   parent.width
                spacing: 4
                visible: card.details.length > 0

                // Divider
                Rectangle {
                    width:  parent.width
                    height: 1
                    color:  Theme.borderSubtle
                    visible: card.details.length > 0
                }

                Repeater {
                    model: card.details
                    Row {
                        width: parent.width
                        Text {
                            text:           modelData.label
                            font.pixelSize: Theme.fontSizeXS
                            color:          Theme.textMuted
                            width:          parent.width * 0.5
                        }
                        Text {
                            text:           modelData.value
                            font.pixelSize: Theme.fontSizeXS
                            color:          Theme.textSecondary
                            width:          parent.width * 0.5
                            horizontalAlignment: Text.AlignRight
                        }
                    }
                }
            }
        }
    }
}
