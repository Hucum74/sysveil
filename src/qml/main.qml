import QtQuick
import QtQuick.Controls.Material

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

    Rectangle {
        anchors.fill: parent
        color: Theme.bgBase

        Column {
            anchors.centerIn: parent
            spacing: Theme.spaceMD

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text:  "SysVeil"
                font.pixelSize: Theme.fontSizeXXL
                font.weight:    Font.Light
                color:          Theme.accent
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text:  "CPU: " + bridge.cpuOverall.toFixed(1) + "%"
                font.pixelSize: Theme.fontSizeLG
                color: Theme.metricColor(bridge.cpuOverall / 100)
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "RAM: " +
                      (bridge.ramUsed  / (1024 * 1024)).toFixed(0) + " MiB / " +
                      (bridge.ramTotal / (1024 * 1024)).toFixed(0) + " MiB" +
                      "  (" + (bridge.ramRatio * 100).toFixed(1) + "%)"
                font.pixelSize: Theme.fontSizeLG
                color: Theme.metricColor(bridge.ramRatio)
            }

            Repeater {
                model: bridge.disks
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "Disk " + modelData.mountPoint + ": " +
                          (modelData.usedBytes  / (1024*1024*1024)).toFixed(1) +
                          " / " +
                          (modelData.totalBytes / (1024*1024*1024)).toFixed(1) + " GiB"
                    font.pixelSize: Theme.fontSizeMD
                    color: Theme.metricColor(modelData.usageRatio)
                }
            }

            Repeater {
                model: bridge.networks
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: modelData.name +
                          "  rx: " + (modelData.rxBytesPerSec / 1024).toFixed(1) + " KiB/s" +
                          "  tx: " + (modelData.txBytesPerSec / 1024).toFixed(1) + " KiB/s"
                    font.pixelSize: Theme.fontSizeMD
                    color: Theme.textSecondary
                }
            }

            // History buffer counter
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                font.pixelSize: Theme.fontSizeSM
                color: Theme.textMuted
                Timer {
                    interval: 1000; running: true; repeat: true
                    onTriggered: parent.text =
                        "buffers — cpu: " + cpuHistory.rowCount()   +
                        "  ram: "  + memoryHistory.rowCount() +
                        "  disk: " + diskHistory.rowCount()   +
                        "  net: "  + networkHistory.rowCount()
                }
            }
        }
    }
}