import QtQuick 2.12
import QtQuick.Window 2.12

import ".."

Rectangle {
    id: overlayWindow
    objectName: "WindowListOverlay"

    color: "black"

    WindowList {
        id: windowList
        anchors.fill: parent

        onWindowSelected: function(windowInfo) {
            appWindow.windowSelected(windowInfo)
            Window.window.close()
        }
    }
}