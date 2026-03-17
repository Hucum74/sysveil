import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    // ── Search bar ────────────────────────────────────────────────────
    Rectangle {
        id: searchBar
        anchors.left:  parent.left
        anchors.right: parent.right
        anchors.top:   parent.top
        height: 44
        color:  Theme.bgSurface
        radius: Theme.cardRadius

        border.color: searchField.activeFocus
                      ? Theme.accent : Theme.borderSubtle
        border.width: 1

        Behavior on border.color {
            ColorAnimation { duration: Theme.animFast }
        }

        TextField {
            id: searchField
            anchors.fill:        parent
            anchors.margins:     Theme.spaceSM
            placeholderText:     "Search by name, user or PID..."
            placeholderTextColor: Theme.textMuted
            color:               Theme.textPrimary
            font.pixelSize:      Theme.fontSizeMD
            background:          Item {}

            onTextChanged: processModel.filterText = text
        }
    }

    // ── Column headers ────────────────────────────────────────────────
    Rectangle {
        id: header
        anchors.left:  parent.left
        anchors.right: parent.right
        anchors.top:   searchBar.bottom
        anchors.topMargin: Theme.spaceSM
        height: 36
        color:  Theme.bgElevated
        radius: Theme.cardRadius

        Row {
            anchors.fill:        parent
            anchors.leftMargin:  Theme.spaceMD
            anchors.rightMargin: Theme.spaceMD

            HeaderCell {
                width:  parent.width * 0.30
                height: parent.height
                label:  "NAME"
                onClicked: processModel.sortByRole(0x102, sortAsc)
            }
            HeaderCell {
                width:  parent.width * 0.08
                height: parent.height
                label:  "PID"
                onClicked: processModel.sortByRole(0x101, sortAsc)
            }
            HeaderCell {
                width:  parent.width * 0.14
                height: parent.height
                label:  "CPU %"
                onClicked: processModel.sortByRole(0x104, sortAsc)
            }
            HeaderCell {
                width:  parent.width * 0.14
                height: parent.height
                label:  "MEM %"
                onClicked: processModel.sortByRole(0x105, sortAsc)
            }
            HeaderCell {
                width:  parent.width * 0.14
                height: parent.height
                label:  "MEM MiB"
                onClicked: processModel.sortByRole(0x106, sortAsc)
            }
            HeaderCell {
                width:  parent.width * 0.12
                height: parent.height
                label:  "USER"
                onClicked: processModel.sortByRole(0x103, sortAsc)
            }
            // Kill button column — no header label
            Item { width: parent.width * 0.08; height: parent.height }
        }
    }

    // ── Process rows ──────────────────────────────────────────────────
    ListView {
        id: listView
        anchors.left:   parent.left
        anchors.right:  parent.right
        anchors.top:    header.bottom
        anchors.bottom: parent.bottom
        anchors.topMargin: Theme.spaceXS
        clip:           true
        model:          processModel
        ScrollBar.vertical: ScrollBar {}

        delegate: Rectangle {
            width:  listView.width
            height: 36
            color:  index % 2 === 0 ? Theme.bgSurface : Theme.bgElevated

            // Hover highlight
            Rectangle {
                anchors.fill: parent
                color:        Theme.accent
                opacity:      hoverHandler.hovered ? 0.05 : 0
                Behavior on opacity {
                    NumberAnimation { duration: Theme.animFast }
                }
            }

            HoverHandler { id: hoverHandler }

            Row {
                anchors.fill:        parent
                anchors.leftMargin:  Theme.spaceMD
                anchors.rightMargin: Theme.spaceMD

                // Name
                Text {
                    width:              parent.width * 0.30
                    height:             parent.height
                    text:               model.name
                    font.pixelSize:     Theme.fontSizeSM
                    color:              Theme.textPrimary
                    verticalAlignment:  Text.AlignVCenter
                    elide:              Text.ElideRight
                }
                // PID
                Text {
                    width:              parent.width * 0.08
                    height:             parent.height
                    text:               model.pid
                    font.pixelSize:     Theme.fontSizeSM
                    color:              Theme.textSecondary
                    verticalAlignment:  Text.AlignVCenter
                }
                // CPU %
                Text {
                    width:              parent.width * 0.14
                    height:             parent.height
                    text:               model.cpuUsage.toFixed(1) + "%"
                    font.pixelSize:     Theme.fontSizeSM
                    color:              Theme.metricColor(model.cpuUsage / 100)
                    verticalAlignment:  Text.AlignVCenter
                }
                // MEM %
                Text {
                    width:              parent.width * 0.14
                    height:             parent.height
                    text:               model.memoryPerc.toFixed(1) + "%"
                    font.pixelSize:     Theme.fontSizeSM
                    color:              Theme.metricColor(model.memoryPerc / 100)
                    verticalAlignment:  Text.AlignVCenter
                }
                // MEM MiB
                Text {
                    width:              parent.width * 0.14
                    height:             parent.height
                    text:               model.memoryMiB + " MiB"
                    font.pixelSize:     Theme.fontSizeSM
                    color:              Theme.textSecondary
                    verticalAlignment:  Text.AlignVCenter
                }
                // User
                Text {
                    width:              parent.width * 0.12
                    height:             parent.height
                    text:               model.user
                    font.pixelSize:     Theme.fontSizeSM
                    color:              Theme.textMuted
                    verticalAlignment:  Text.AlignVCenter
                    elide:              Text.ElideRight
                }
                // Kill button
                Item {
                    width:  parent.width * 0.08
                    height: parent.height

                    Rectangle {
                        anchors.centerIn: parent
                        width:  52
                        height: 22
                        radius: 4
                        color:  killHover.hovered
                                ? Theme.metricHigh : "transparent"
                        border.color: Theme.metricHigh
                        border.width: 1
                        opacity: hoverHandler.hovered ? 1 : 0

                        Behavior on color {
                            ColorAnimation { duration: Theme.animFast }
                        }

                        Text {
                            anchors.centerIn: parent
                            text:             "KILL"
                            font.pixelSize:   Theme.fontSizeXS
                            font.weight:      Font.Medium
                            color:            Theme.metricHigh
                        }

                        HoverHandler { id: killHover }
                        TapHandler  {
                            onTapped: confirmDialog.open(model.pid, model.name)
                        }
                    }
                }
            }
        }
    }

    // ── Kill confirmation dialog ───────────────────────────────────────
    Dialog {
        id: confirmDialog
        anchors.centerIn: parent
        modal:            true
        title:            "Terminate process"

        property int    targetPid:  -1
        property string targetName: ""

        function open(pid, name) {
            targetPid  = pid
            targetName = name
            visible    = true
        }

        background: Rectangle {
            color:        Theme.bgElevated
            radius:       Theme.cardRadius
            border.color: Theme.borderStrong
            border.width: 1
        }

        header: Rectangle {
            color:  Theme.bgElevated
            height: 48
            radius: Theme.cardRadius
            Text {
                anchors.left:           parent.left
                anchors.leftMargin:     Theme.spaceLG
                anchors.verticalCenter: parent.verticalCenter
                text:           "Terminate process"
                font.pixelSize: Theme.fontSizeMD
                font.weight:    Font.Medium
                color:          Theme.textPrimary
            }
        }

        contentItem: Column {
            spacing: Theme.spaceSM
            padding: Theme.spaceMD

            Text {
                text: "Send SIGTERM to <b>" + confirmDialog.targetName +
                      "</b> (PID " + confirmDialog.targetPid + ")?"
                font.pixelSize: Theme.fontSizeMD
                color:          Theme.textPrimary
                textFormat:     Text.RichText
            }
            Text {
                text:           "If the process does not exit within 5 seconds,\nSIGKILL will be sent automatically."
                font.pixelSize: Theme.fontSizeSM
                color:          Theme.textSecondary
            }
        }

        footer: Row {
            spacing:        Theme.spaceSM
            padding:        Theme.spaceMD
            layoutDirection: Qt.RightToLeft

            Button {
                text: "Terminate"
                onClicked: {
                    bridge.killProcess(confirmDialog.targetPid, true)
                    confirmDialog.visible = false
                }
            }
            Button {
                text: "Cancel"
                onClicked: confirmDialog.visible = false
            }
        }
    }
}