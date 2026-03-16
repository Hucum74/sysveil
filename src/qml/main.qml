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

            // ── Current values ────────────────────────────────────────────
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text:  "CPU: " + bridge.cpuOverall.toFixed(1) + "%"
                font.pixelSize: 20
                color: "#ffffff"
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "RAM: " +
                      (bridge.ramUsed  / (1024 * 1024)).toFixed(0) + " MiB / " +
                      (bridge.ramTotal / (1024 * 1024)).toFixed(0) + " MiB" +
                      "  (" + (bridge.ramRatio * 100).toFixed(1) + "%)"
                font.pixelSize: 20
                color: "#ffffff"
            }

            Repeater {
                model: bridge.disks
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "Disk " + modelData.mountPoint + ": " +
                          (modelData.usedBytes  / (1024*1024*1024)).toFixed(1) +
                          " / " +
                          (modelData.totalBytes / (1024*1024*1024)).toFixed(1) + " GiB"
                    font.pixelSize: 16
                    color: "#aaaaaa"
                }
            }

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

            // ── History buffer smoke test ──────────────────────────────────
            Rectangle {
                anchors.horizontalCenter: parent.horizontalCenter
                width: 420
                height: 1
                color: "#333"
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "History buffers (samples accumulated):"
                font.pixelSize: 13
                color: "#666"
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "CPU: "     + cpuHistory.rowCount()     +
                      "  RAM: "   + memoryHistory.rowCount()  +
                      "  Disk: "  + diskHistory.rowCount()    +
                      "  Net: "   + networkHistory.rowCount()
                font.pixelSize: 13
                color: "#4ecdc4"

                // Refresh the count display every second
                // (models emit signals but Text needs an explicit binding trigger)
                Timer {
                    interval: 1000
                    running:  true
                    repeat:   true
                    onTriggered: parent.text =
                        "CPU: "    + cpuHistory.rowCount()    +
                        "  RAM: "  + memoryHistory.rowCount() +
                        "  Disk: " + diskHistory.rowCount()   +
                        "  Net: "  + networkHistory.rowCount()
                }
            }
        }
    }
}