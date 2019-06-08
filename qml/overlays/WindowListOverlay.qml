import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls.Universal 2.3

import ".."

Rectangle {
    id: overlayWindow
    objectName: "WindowListOverlay"

    Universal.theme: Universal.Dark

    anchors.fill: parent

    color: "black"
    property bool isVisible: false

    property var windowActive: Window.active
    onWindowActiveChanged: {
        if(isVisible && !windowActive) hide()
    }

    property var targetItem: null

    function show(newTargetItem, x, y, width) {
        targetItem = newTargetItem

        Window.window.width = width
        Window.window.height = Math.min(windowList.contentHeight, 500)
        Window.window.x = x
        Window.window.y = y
        Window.window.requestActivate()

        isVisible = true
    }

    function hide() {
        var offscreen = appCore.windowView.getOffscreenArea()

        Window.window.x = offscreen.x
        Window.window.y = offscreen.y

        isVisible = false
        targetItem = null
    }

    function toggle(newTargetItem, x, y, width) {
        if(isVisible && targetItem === newTargetItem) {
            hide()
        }
        else {
            show(newTargetItem, x, y, width)
        }
    }

    WindowList {
        id: windowList
        anchors.fill: parent

        onWindowSelected: function(windowInfo) {
            targetItem.windowInfo = windowInfo
            targetItem.updateWindowPosition()
            hide()
        }
    }
}