import QtQuick
import QtQuick.Controls.Material
import QtQuick.Controls
import QtQuick.Layouts
import "components"

ApplicationWindow {
    id: root

    width:  1024
    height: 680
    minimumWidth:  800
    minimumHeight: 560
    visible: true
    title:   "SysVeil — System Monitor"

    Material.theme:  Material.Dark
    Material.accent: Material.Teal

    // ── Root background ───────────────────────────────────────────────
    Rectangle {
        anchors.fill: parent
        color: Theme.bgBase
    }

    // ── Sidebar ───────────────────────────────────────────────────────
    Sidebar {
        id: sidebar
        anchors.left:   parent.left
        anchors.top:    parent.top
        anchors.bottom: parent.bottom
        width: Theme.sidebarWidth

        onItemSelected: function(index) {
            stack.currentIndex = index
        }
    }

    // ── Content area ──────────────────────────────────────────────────
    Item {
        anchors.left:   sidebar.right
        anchors.right:  parent.right
        anchors.top:    parent.top
        anchors.bottom: parent.bottom

        // Page header
        Rectangle {
            id: header
            anchors.left:  parent.left
            anchors.right: parent.right
            anchors.top:   parent.top
            height: 56
            color:  Theme.bgSurface

            // Bottom border
            Rectangle {
                anchors.left:   parent.left
                anchors.right:  parent.right
                anchors.bottom: parent.bottom
                height: 1
                color:  Theme.borderSubtle
            }

            Text {
                anchors.left:           parent.left
                anchors.leftMargin:     Theme.spaceLG
                anchors.verticalCenter: parent.verticalCenter
                text: ["Overview", "CPU", "Memory", "Disk", "Network"][sidebar.currentIndex]
                font.pixelSize: Theme.fontSizeLG
                font.weight:    Font.Medium
                color:          Theme.textPrimary
            }
        }

        // Page stack
        StackLayout {
            id: stack
            anchors.left:   parent.left
            anchors.right:  parent.right
            anchors.top:    header.bottom
            anchors.bottom: parent.bottom
            anchors.margins: Theme.spaceLG
            currentIndex:   sidebar.currentIndex

            // Page 0: Overview
            Item {
                id: overviewPage

                Flickable {
                    anchors.fill:  parent
                    contentHeight: overviewCol.implicitHeight + Theme.spaceXXL
                    clip:          true
                    ScrollBar.vertical: ScrollBar {}

                    Column {
                        id: overviewCol
                        anchors.left:   parent.left
                        anchors.right:  parent.right
                        anchors.top:    parent.top
                        spacing:        Theme.spaceLG

                        // ── Section: Resources ────────────────────────
                        Text {
                            text:               "RESOURCES"
                            font.pixelSize:     Theme.fontSizeXS
                            font.letterSpacing: 1.5
                            font.weight:        Font.Medium
                            color:              Theme.textMuted
                            topPadding:         Theme.spaceSM
                        }

                        GridLayout {
                            id: resourceGrid
                            width:         parent.width
                            columns:       Math.max(1, Math.floor(width / 280))
                            columnSpacing: Theme.spaceMD
                            rowSpacing:    Theme.spaceMD

                            // CPU
                            MetricCard {
                                Layout.fillWidth: true
                                label:    "CPU"
                                unit:     "%"
                                value:    bridge.cpuOverall
                                maxValue: 100
                                subtitle: bridge.cpuCores.length + " logical cores"
                                details: [
                                    { label: "Core 0",  value: bridge.cpuCores[0]  !== undefined ? bridge.cpuCores[0].toFixed(1)  + "%" : "—" },
                                    { label: "Core 1",  value: bridge.cpuCores[1]  !== undefined ? bridge.cpuCores[1].toFixed(1)  + "%" : "—" },
                                    { label: "Core 2",  value: bridge.cpuCores[2]  !== undefined ? bridge.cpuCores[2].toFixed(1)  + "%" : "—" },
                                    { label: "Core 3",  value: bridge.cpuCores[3]  !== undefined ? bridge.cpuCores[3].toFixed(1)  + "%" : "—" }
                                ]
                            }

                            // Memory
                            MetricCard {
                                Layout.fillWidth: true
                                label:    "MEMORY"
                                unit:     "%"
                                value:    bridge.ramRatio * 100
                                maxValue: 100
                                subtitle: (bridge.ramUsed  / (1024*1024*1024)).toFixed(2) +
                                          " / " +
                                          (bridge.ramTotal / (1024*1024*1024)).toFixed(1) + " GiB"
                                details: [
                                    { label: "Used",      value: (bridge.ramUsed  / (1024*1024*1024)).toFixed(2) + " GiB" },
                                    { label: "Available", value: ((bridge.ramTotal - bridge.ramUsed) / (1024*1024*1024)).toFixed(2) + " GiB" },
                                    { label: "Swap used", value: (bridge.swapUsed  / (1024*1024*1024)).toFixed(2) + " GiB" },
                                    { label: "Swap total",value: (bridge.swapTotal / (1024*1024*1024)).toFixed(1) + " GiB" }
                                ]
                            }
                        }

                        // ── Section: Storage ──────────────────────────
                        Text {
                            text:               "STORAGE"
                            font.pixelSize:     Theme.fontSizeXS
                            font.letterSpacing: 1.5
                            font.weight:        Font.Medium
                            color:              Theme.textMuted
                        }

                        GridLayout {
                            width:         parent.width
                            columns:       Math.max(1, Math.floor(width / 280))
                            columnSpacing: Theme.spaceMD
                            rowSpacing:    Theme.spaceMD

                            Repeater {
                                model: bridge.disks
                                MetricCard {
                                    Layout.fillWidth: true
                                    label:    "DISK " + modelData.mountPoint.toUpperCase()
                                    unit:     "%"
                                    value:    modelData.usageRatio * 100
                                    maxValue: 100
                                    subtitle: (modelData.usedBytes  / (1024*1024*1024)).toFixed(1) +
                                              " / " +
                                              (modelData.totalBytes / (1024*1024*1024)).toFixed(1) + " GiB"
                                    details: [
                                        { label: "Used",    value: (modelData.usedBytes  / (1024*1024*1024)).toFixed(2) + " GiB" },
                                        { label: "Free",    value: ((modelData.totalBytes - modelData.usedBytes) / (1024*1024*1024)).toFixed(2) + " GiB" },
                                        { label: "Read",    value: (modelData.readBytesPerSec  / 1024).toFixed(1) + " KiB/s" },
                                        { label: "Write",   value: (modelData.writeBytesPerSec / 1024).toFixed(1) + " KiB/s" }
                                    ]
                                }
                            }
                        }

                        // ── Section: Network ──────────────────────────
                        Text {
                            text:               "NETWORK"
                            font.pixelSize:     Theme.fontSizeXS
                            font.letterSpacing: 1.5
                            font.weight:        Font.Medium
                            color:              Theme.textMuted
                        }

                        GridLayout {
                            width:         parent.width
                            columns:       Math.max(1, Math.floor(width / 280))
                            columnSpacing: Theme.spaceMD
                            rowSpacing:    Theme.spaceMD

                            Repeater {
                                model: bridge.networks
                                MetricCard {
                                    Layout.fillWidth: true
                                    label:    modelData.name.toUpperCase()
                                    unit:     "KiB/s"
                                    value:    modelData.rxBytesPerSec / 1024
                                    maxValue: 10240
                                    subtitle: "rx: " + (modelData.rxBytesPerSec / 1024).toFixed(1) +
                                              "  tx: " + (modelData.txBytesPerSec / 1024).toFixed(1) + " KiB/s"
                                    details: [
                                        { label: "Download", value: (modelData.rxBytesPerSec / 1024).toFixed(1) + " KiB/s" },
                                        { label: "Upload",   value: (modelData.txBytesPerSec / 1024).toFixed(1) + " KiB/s" }
                                    ]
                                }
                            }
                        }
                    }
                }
            }
            // Page 1: CPU detail
            Item {
                Flickable {
                    anchors.fill:  parent
                    contentHeight: cpuDetailCol.implicitHeight + Theme.spaceXXL
                    clip:          true
                    ScrollBar.vertical: ScrollBar {}
                    Column {
                        id: cpuDetailCol
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.top: parent.top
                        spacing: Theme.spaceLG

                        MetricCard {
                            width: parent.width
                            label:    "CPU OVERALL"
                            unit:     "%"
                            value:    bridge.cpuOverall
                            maxValue: 100
                            subtitle: bridge.cpuCores.length + " logical cores"
                        }

                        Text {
                            text: "PER CORE"
                            font.pixelSize: Theme.fontSizeXS
                            font.letterSpacing: 1.5
                            font.weight: Font.Medium
                            color: Theme.textMuted
                        }

                        GridLayout {
                            width: parent.width
                            columns: Math.max(1, Math.floor(width / 200))
                            columnSpacing: Theme.spaceMD
                            rowSpacing: Theme.spaceMD
                            Repeater {
                                model: bridge.cpuCores.length
                                MetricCard {
                                    Layout.fillWidth: true
                                    label:    "CORE " + index
                                    unit:     "%"
                                    value:    bridge.cpuCores[index] !== undefined ? bridge.cpuCores[index] : 0
                                    maxValue: 100
                                }
                            }
                        }
                    }
                }
            }

            // Page 2: Memory detail
            Item {
                Flickable {
                    anchors.fill:  parent
                    contentHeight: memDetailCol.implicitHeight + Theme.spaceXXL
                    clip:          true
                    ScrollBar.vertical: ScrollBar {}
                    Column {
                        id: memDetailCol
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.top: parent.top
                        spacing: Theme.spaceLG

                        MetricCard {
                            width: parent.width
                            label:    "RAM USAGE"
                            unit:     "%"
                            value:    bridge.ramRatio * 100
                            maxValue: 100
                            subtitle: (bridge.ramUsed / (1024*1024*1024)).toFixed(2) +
                                      " / " +
                                      (bridge.ramTotal / (1024*1024*1024)).toFixed(1) + " GiB"
                            details: [
                                { label: "Used",      value: (bridge.ramUsed  / (1024*1024*1024)).toFixed(2) + " GiB" },
                                { label: "Available", value: ((bridge.ramTotal - bridge.ramUsed) / (1024*1024*1024)).toFixed(2) + " GiB" },
                                { label: "Total",     value: (bridge.ramTotal / (1024*1024*1024)).toFixed(1) + " GiB" }
                            ]
                        }

                        MetricCard {
                            width: parent.width
                            label:    "SWAP"
                            unit:     "%"
                            value:    bridge.swapRatio * 100
                            maxValue: 100
                            subtitle: (bridge.swapUsed / (1024*1024*1024)).toFixed(2) +
                                      " / " +
                                      (bridge.swapTotal / (1024*1024*1024)).toFixed(1) + " GiB"
                            details: [
                                { label: "Used",  value: (bridge.swapUsed  / (1024*1024*1024)).toFixed(2) + " GiB" },
                                { label: "Free",  value: ((bridge.swapTotal - bridge.swapUsed) / (1024*1024*1024)).toFixed(2) + " GiB" },
                                { label: "Total", value: (bridge.swapTotal / (1024*1024*1024)).toFixed(1) + " GiB" }
                            ]
                        }
                    }
                }
            }

            // Page 3: Disk detail
            Item {
                Flickable {
                    anchors.fill:  parent
                    contentHeight: diskDetailCol.implicitHeight + Theme.spaceXXL
                    clip:          true
                    ScrollBar.vertical: ScrollBar {}
                    Column {
                        id: diskDetailCol
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.top: parent.top
                        spacing: Theme.spaceLG

                        Text {
                            text: "MOUNTED VOLUMES"
                            font.pixelSize: Theme.fontSizeXS
                            font.letterSpacing: 1.5
                            font.weight: Font.Medium
                            color: Theme.textMuted
                        }

                        Repeater {
                            model: bridge.disks
                            MetricCard {
                                width: parent.width
                                label:    "DISK " + modelData.mountPoint.toUpperCase()
                                unit:     "%"
                                value:    modelData.usageRatio * 100
                                maxValue: 100
                                subtitle: (modelData.usedBytes  / (1024*1024*1024)).toFixed(1) +
                                          " / " +
                                          (modelData.totalBytes / (1024*1024*1024)).toFixed(1) + " GiB"
                                details: [
                                    { label: "Used",    value: (modelData.usedBytes  / (1024*1024*1024)).toFixed(2) + " GiB" },
                                    { label: "Free",    value: ((modelData.totalBytes - modelData.usedBytes) / (1024*1024*1024)).toFixed(2) + " GiB" },
                                    { label: "Total",   value: (modelData.totalBytes / (1024*1024*1024)).toFixed(1) + " GiB" },
                                    { label: "Read",    value: (modelData.readBytesPerSec  / 1024).toFixed(1) + " KiB/s" },
                                    { label: "Write",   value: (modelData.writeBytesPerSec / 1024).toFixed(1) + " KiB/s" }
                                ]
                            }
                        }
                    }
                }
            }

            // Page 4: Network detail
            Item {
                Flickable {
                    anchors.fill:  parent
                    contentHeight: netDetailCol.implicitHeight + Theme.spaceXXL
                    clip:          true
                    ScrollBar.vertical: ScrollBar {}
                    Column {
                        id: netDetailCol
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.top: parent.top
                        spacing: Theme.spaceLG

                        Text {
                            text: "INTERFACES"
                            font.pixelSize: Theme.fontSizeXS
                            font.letterSpacing: 1.5
                            font.weight: Font.Medium
                            color: Theme.textMuted
                        }

                        Repeater {
                            model: bridge.networks
                            MetricCard {
                                width: parent.width
                                label:    modelData.name.toUpperCase()
                                unit:     "KiB/s"
                                value:    modelData.rxBytesPerSec / 1024
                                maxValue: 10240
                                subtitle: "rx: " + (modelData.rxBytesPerSec / 1024).toFixed(1) +
                                          "  tx: " + (modelData.txBytesPerSec / 1024).toFixed(1) + " KiB/s"
                                details: [
                                    { label: "Download", value: (modelData.rxBytesPerSec / 1024).toFixed(1) + " KiB/s" },
                                    { label: "Upload",   value: (modelData.txBytesPerSec / 1024).toFixed(1) + " KiB/s" },
                                    { label: "Total rx", value: "—" },
                                    { label: "Total tx", value: "—" }
                                ]
                            }
                        }
                    }
                }
            }
        }
    }
}
