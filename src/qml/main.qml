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
        color: "#1a1a2e"

        Column {
            anchors.centerIn: parent
            spacing: 16

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text:  "SysVeil"
                font.pixelSize: 48
                font.weight:    Font.Light
                color:          "#4ecdc4"
            }

            // ── CPU ───────────────────────────────────────────────────────
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text:  "CPU: " + bridge.cpuOverall.toFixed(1) + "%"
                font.pixelSize: 20
                color: "#ffffff"
            }

            // ── Memory ────────────────────────────────────────────────────
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "RAM: " +
                      (bridge.ramUsed  / (1024 * 1024)).toFixed(0) + " MiB / " +
                      (bridge.ramTotal / (1024 * 1024)).toFixed(0) + " MiB" +
                      "  (" + (bridge.ramRatio * 100).toFixed(1) + "%)"
                font.pixelSize: 20
                color: "#ffffff"
            }

            // ── Disk ──────────────────────────────────────────────────────
            Repeater {
                model: bridge.disks
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "Disk " + modelData.mountPoint + ": " +
                          (modelData.usedBytes  / (1024*1024*1024)).toFixed(1) + " / " +
                          (modelData.totalBytes / (1024*1024*1024)).toFixed(1) + " GiB"
                    font.pixelSize: 16
                    color: "#aaaaaa"
                }
            }

            // ── Network ───────────────────────────────────────────────────
            Repeater {
                model: bridge.networks
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: modelData.name +
                          "  rx: " + (modelData.rxBytesPerSec / 1024).toFixed(1) + " KiB/s" +
                          "  tx: " + (modelData.txBytesPerSec / 1024).toFixed(1) + " KiB/s"
                    font.pixelSize: 16
                    color: "#aaaaaa"
                }
            }
        }
    }
}