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

            // Page 0: Overview (placeholder)
            Rectangle {
                color: "transparent"
                Text {
                    anchors.centerIn: parent
                    text:  "Overview page — coming in Step 4"
                    color: Theme.textMuted
                    font.pixelSize: Theme.fontSizeMD
                }
            }

            // Page 1: CPU (placeholder)
            Rectangle {
                color: "transparent"
                Text {
                    anchors.centerIn: parent
                    text:  "CPU detail page — coming in Step 6"
                    color: Theme.textMuted
                    font.pixelSize: Theme.fontSizeMD
                }
            }

            // Page 2: Memory (placeholder)
            Rectangle {
                color: "transparent"
                Text {
                    anchors.centerIn: parent
                    text:  "Memory detail page — coming in Step 6"
                    color: Theme.textMuted
                    font.pixelSize: Theme.fontSizeMD
                }
            }

            // Page 3: Disk (placeholder)
            Rectangle {
                color: "transparent"
                Text {
                    anchors.centerIn: parent
                    text:  "Disk detail page — coming in Step 6"
                    color: Theme.textMuted
                    font.pixelSize: Theme.fontSizeMD
                }
            }

            // Page 4: Network (placeholder)
            Rectangle {
                color: "transparent"
                Text {
                    anchors.centerIn: parent
                    text:  "Network detail page — coming in Step 6"
                    color: Theme.textMuted
                    font.pixelSize: Theme.fontSizeMD
                }
            }
        }
    }
}
