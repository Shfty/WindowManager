import QtQuick 2.0

ShaderEffectSource {
    id: blurBehind

    property var targetMonitor: null
    property string tintColor: "#00000000"

    Rectangle {
        anchors.fill: parent
        color: tintColor
    }

    sourceItem: targetMonitor.blurWallpaper

    function updateSourceRect() {
        // Calculate rect
        var globalPos = mapToGlobal(0, 0)
        var offsetPos = Qt.point(globalPos.x + Math.abs(windowManager.getDesktopRect().left),
                                globalPos.y + Math.abs(windowManager.getDesktopRect().top))

        var sceneGeometry = mapToItem(targetMonitor.root, 0, 0, width, height)

        sourceRect = Qt.rect(sceneGeometry.x + Math.abs(windowManager.getDesktopRect().left), sceneGeometry.y, width, height)
    }

    Connections {
        target: targetMonitor
        onBeforeRendering: {
            blurBehind.updateSourceRect()
        }
    }
}
