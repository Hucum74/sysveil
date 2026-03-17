import QtQuick
import QtQuick.Controls.Material

Item {
    id: sidebar

    property int   currentIndex: 0
    property alias model: repeater.model

    signal itemSelected(int index)

    // ── Nav items data ────────────────────────────────────────────────
    ListModel {
        id: navModel
        ListElement { label: "Overview";  icon: "⬡" }
        ListElement { label: "CPU";       icon: "⚡" }
        ListElement { label: "Memory";    icon: "▦" }
        ListElement { label: "Disk";      icon: "◫" }
        ListElement { label: "Network";   icon: "⇅" }
        ListElement { label: "Processes"; icon: "☰" }
    }

    Component.onCompleted: repeater.model = navModel

    Rectangle {
        anchors.fill: parent
        color: Theme.sidebarBg

        // Right border
        Rectangle {
            anchors.right:  parent.right
            anchors.top:    parent.top
            anchors.bottom: parent.bottom
            width: 1
            color: Theme.borderSubtle
        }

        Column {
            anchors.fill: parent
            anchors.topMargin: Theme.spaceXL

            // App title
            Item {
                width:  parent.width
                height: 56

                Text {
                    anchors.left:           parent.left
                    anchors.leftMargin:     Theme.spaceLG
                    anchors.verticalCenter: parent.verticalCenter
                    text:  "SysVeil"
                    font.pixelSize: Theme.fontSizeLG
                    font.weight:    Font.Medium
                    color:          Theme.accent
                }
            }

            // Divider
            Rectangle {
                width:  parent.width
                height: 1
                color:  Theme.borderSubtle
                anchors.leftMargin:  Theme.spaceLG
                anchors.rightMargin: Theme.spaceLG
            }

            Item { width: 1; height: Theme.spaceMD }

            // Nav items
            Repeater {
                id: repeater
                delegate: NavItem {
                    width:     sidebar.width
                    label:     model.label
                    icon:      model.icon
                    active:    sidebar.currentIndex === index
                    onClicked: {
                        sidebar.currentIndex = index
                        sidebar.itemSelected(index)
                    }
                }
            }
        }

        // Version tag at bottom
        Text {
            anchors.bottom:      parent.bottom
            anchors.left:        parent.left
            anchors.margins:     Theme.spaceMD
            text:  "v0.1.0"
            font.pixelSize: Theme.fontSizeXS
            color: Theme.textMuted
        }
    }
}
