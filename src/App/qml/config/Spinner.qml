import QtQuick 2.12

Item {
    id: spinner

    property real size: 128
    property real dotScale: 1
    property int dotCount: 8
    property color color: "white"
    property int duration: 2000

    readonly property real dotSize: (size / 4) * dotScale

    width: size
    height: size

    RotationAnimation on rotation {
        loops: Animation.Infinite
        from: 0
        to: 360
        duration: spinner.duration
    }

    Repeater {
        model: spinner.dotCount

        Rectangle {
            id: dot

            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter

            readonly property real offset: (spinner.size - spinner.dotSize) * 0.5
            readonly property real angle: (index / spinner.dotCount) * Math.PI * 2

            transform: Translate {
                x: offset * Math.sin(angle)
                y: offset * Math.cos(angle)
            }

            width: spinner.dotSize
            height: spinner.dotSize
            radius: spinner.dotSize
            color: spinner.color
        }
    }
}