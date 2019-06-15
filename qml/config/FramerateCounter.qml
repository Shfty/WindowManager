import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Window 2.12

Label {
    id: framerateMonitor

    readonly property int sampleWindow: 120
    property var framerate: 60
    property var tickIndex: 0
    property var tickSum: 0
    property var lastTick: 0

    property var tickList: []
    Component.onCompleted: {
        lastTick = Number(Date.now());
        for(var i = 0; i < sampleWindow; ++i)
        {
            tickList.push(0)
        }
    }

    property var win: Window.window

    Connections {
        id: fpsConnections
        target: framerateMonitor.win
        onFrameSwapped: {
            var fm = framerateMonitor

            if(fm.tickList.length === 0) return

            var timestamp = Number(Date.now())
            var tick = timestamp - fm.lastTick
            fm.lastTick = timestamp

            fm.tickSum -= fm.tickList[fm.tickIndex]
            fm.tickSum += tick
            fm.tickList[fm.tickIndex] = tick

            fm.tickIndex++
            if(fm.tickIndex === fm.sampleWindow)
            {
                fm.tickIndex = 0
            }

            fm.framerate = (1000 / (fm.tickSum / fm.sampleWindow)).toFixed(2)
        }
    }

    text: framerate + " FPS"
}
