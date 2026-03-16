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
            spacing: 12

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text:  "SysVeil"
                font.pixelSize: 48
                font.weight:    Font.Light
                color:          "#4ecdc4"
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text:  "Phase 1 scaffold — window alive ✓"
                font.pixelSize: 16
                color:          "#888"
            }
        }
    }
}