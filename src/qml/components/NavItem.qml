import QtQuick

Item {
    id: navItem

    property string label:  ""
    property string icon:   ""
    property bool   active: false

    signal clicked()

    height: 44

    // Active indicator bar on left edge
    Rectangle {
        anchors.left:   parent.left
        anchors.top:    parent.top
        anchors.bottom: parent.bottom
        width:  3
        radius: 2
        color:  navItem.active ? Theme.sidebarActiveBar : "transparent"

        Behavior on color {
            ColorAnimation { duration: Theme.animFast }
        }
    }

    // Background
    Rectangle {
        id: bg
        anchors.fill:        parent
        anchors.leftMargin:  4
        anchors.rightMargin: Theme.spaceSM
        radius: Theme.cardRadius / 2
        color: navItem.active   ? Theme.sidebarItemActive :
               hoverHandler.hovered ? Theme.sidebarItemHover : "transparent"

        Behavior on color {
            ColorAnimation { duration: Theme.animFast }
        }
    }

    // Icon + label
    Row {
        anchors.left:           parent.left
        anchors.leftMargin:     Theme.spaceLG
        anchors.verticalCenter: parent.verticalCenter
        spacing: Theme.spaceMD

        Text {
            text:           navItem.icon
            font.pixelSize: Theme.fontSizeMD
            color:          navItem.active ? Theme.accent : Theme.textSecondary
            anchors.verticalCenter: parent.verticalCenter

            Behavior on color {
                ColorAnimation { duration: Theme.animFast }
            }
        }

        Text {
            text:           navItem.label
            font.pixelSize: Theme.fontSizeMD
            font.weight:    navItem.active ? Font.Medium : Font.Normal
            color:          navItem.active ? Theme.textPrimary : Theme.textSecondary
            anchors.verticalCenter: parent.verticalCenter

            Behavior on color {
                ColorAnimation { duration: Theme.animFast }
            }
        }
    }

    HoverHandler { id: hoverHandler }
    TapHandler  { onTapped: navItem.clicked() }
}
