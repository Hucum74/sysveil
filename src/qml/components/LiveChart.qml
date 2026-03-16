import QtQuick
import QtCharts

Item {
    id: chart

    property var   historyModel: null
    property int   valueRole:   Qt.UserRole + 2
    property real  yMin:        0
    property real  yMax:        100
    property color lineColor:   Theme.chartCpuLine
    property string label:      ""
    property int   capacity:    60

    implicitHeight: Theme.chartHeight

    // Defer all model connections until after component is fully loaded
    Component.onCompleted: {
        if (historyModel) {
            historyModel.rowsInserted.connect(function() {
                Qt.callLater(refreshSeries)
            })
        }
    }

    function refreshSeries() {
        if (!historyModel) return
        try {
            var count = historyModel.rowCount()
            if (count === 0) return
            series.clear()
            for (var i = 0; i < count; i++) {
                var idx   = historyModel.index(i, 0)
                var value = historyModel.data(idx, chart.valueRole)
                series.append(i, (value !== undefined && value !== null) ? value : 0)
            }
            axisX.max = Math.max(capacity - 1, count - 1)
        } catch(e) {
            console.log("LiveChart error:", e)
        }
    }

    ChartView {
        id: chartView
        anchors.fill:    parent
        backgroundColor: Theme.bgSurface
        plotAreaColor:   Theme.bgSurface
        legend.visible:  false
        antialiasing:    true
        margins.top:     0
        margins.bottom:  0
        margins.left:    0
        margins.right:   0

        ValuesAxis {
            id: axisY
            min:          chart.yMin
            max:          chart.yMax
            tickCount:    5
            gridLineColor:    Theme.chartGrid
            labelsColor:      Theme.chartAxisText
            labelsFont.pixelSize: Theme.fontSizeXS
            lineVisible:      false
            shadesVisible:    false
        }

        ValuesAxis {
            id: axisX
            min:          0
            max:          chart.capacity - 1
            tickCount:    6
            gridLineColor:    Theme.chartGrid
            labelsColor:      Theme.chartAxisText
            labelsFont.pixelSize: Theme.fontSizeXS
            lineVisible:      false
            shadesVisible:    false
            labelsVisible:    false
        }

        SplineSeries {
            id:    series
            axisX: axisX
            axisY: axisY
            color: chart.lineColor
            width: 2
        }
    }

    Text {
        anchors.top:        parent.top
        anchors.left:       parent.left
        anchors.margins:    Theme.spaceMD
        text:               chart.label
        font.pixelSize:     Theme.fontSizeXS
        font.weight:        Font.Medium
        color:              Theme.textMuted
        font.letterSpacing: 1.2
        visible:            chart.label !== ""
    }
}
