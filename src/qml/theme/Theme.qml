import QtQuick

QtObject {

    readonly property color bgBase:    "#0d0d1a"
    readonly property color bgSurface: "#13131f"
    readonly property color bgElevated:"#1a1a2e"
    readonly property color borderSubtle: "#ffffff18"
    readonly property color borderStrong: "#ffffff30"
    readonly property color textPrimary:   "#f0f0f0"
    readonly property color textSecondary: "#9090a0"
    readonly property color textMuted:     "#505060"
    readonly property color accent:      "#4ecdc4"
    readonly property color accentDim:   "#2a7a74"
    readonly property color accentGlow:  "#4ecdc440"
    readonly property color metricLow:      "#4ecdc4"
    readonly property color metricMedium:   "#f7b731"
    readonly property color metricHigh:     "#ff6b6b"
    readonly property color metricCritical: "#ff3333"
    readonly property color sidebarBg:         "#0d0d1a"
    readonly property color sidebarItemHover:  "#ffffff0d"
    readonly property color sidebarItemActive: "#4ecdc418"
    readonly property color sidebarActiveBar:  "#4ecdc4"
    readonly property color chartGrid:      "#ffffff0d"
    readonly property color chartAxisText:  "#505060"
    readonly property color chartCpuLine:   "#4ecdc4"
    readonly property color chartRamLine:   "#f7b731"
    readonly property color chartDiskLine:  "#a29bfe"
    readonly property color chartNetRxLine: "#55efc4"
    readonly property color chartNetTxLine: "#fd79a8"
    readonly property int fontSizeXS:  10
    readonly property int fontSizeSM:  12
    readonly property int fontSizeMD:  14
    readonly property int fontSizeLG:  18
    readonly property int fontSizeXL:  24
    readonly property int fontSizeXXL: 36
    readonly property int spaceXS:  4
    readonly property int spaceSM:  8
    readonly property int spaceMD:  16
    readonly property int spaceLG:  24
    readonly property int spaceXL:  32
    readonly property int spaceXXL: 48
    readonly property int sidebarWidth:   220
    readonly property int cardRadius:     12
    readonly property int cardPadding:    20
    readonly property int chartHeight:    200
    readonly property int animFast:   150
    readonly property int animNormal: 300
    readonly property int animSlow:   600
    readonly property int numberAnimDuration: 400
    readonly property int pulseAnimDuration:  600

    function metricColor(ratio) {
        if (ratio >= 0.95) return metricCritical
        if (ratio >= 0.80) return metricHigh
        if (ratio >= 0.60) return metricMedium
        return metricLow
    }
}
