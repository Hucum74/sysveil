import QtQuick

Item {
    property string label:   ""
    property bool   sortAsc: false

    signal clicked()

    Text {
        anchors.left:           parent.left
        anchors.verticalCenter: parent.verticalCenter
        text:               parent.label
        font.pixelSize:     Theme.fontSizeXS
        font.weight:        Font.Medium
        font.letterSpacing: 1.2
        color:              Theme.textMuted
    }

    HoverHandler { id: hoverHandler }
    TapHandler {
        onTapped: {
            parent.sortAsc = !parent.sortAsc
            parent.clicked()
        }
    }
}