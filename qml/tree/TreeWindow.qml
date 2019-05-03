import QtQuick 2.11
import QtQuick.Window 2.11
import QtGraphicalEffects 1.0
import QtQuick.Controls.Universal 2.3

import ".."

MonitorWindow {
    id: treeWindow

    visible: wallpaper.status == Image.Ready
    
    property var treeItem: model.modelData
    property var root: rootItem

    property var windowIndex: index
    property rect desktopRect: windowManager.getDesktopRect()
    offset: {
        return Qt.point(Math.abs(desktopRect.x), Math.abs(desktopRect.y))
    }

    property var thumbnailOverlay: null

    flags: Qt.FramelessWindowHint

    title: treeItem ? treeItem.title : ""

    Universal.theme: Universal.Dark

    Component.onCompleted: {
        windowManager.attachWindowToDesktop(treeWindow)
    }

    // Wallpaper
    Image {
        id: wallpaper
        
        x: desktopRect.x - treeWindow.x
        y: desktopRect.y - treeWindow.y
        width: desktopRect.width
        height: desktopRect.height

        asynchronous: true
        cache: true

        source: Options.wallpaperUrl
        fillMode: Options.wallpaperFillMode
    }

    // Recursive tree delegate
    Component {
        id: treeDelegate

        TreeDelegate {}
    }

    // Root tree item
    TreeDelegate {
        id: rootItem
        anchors.fill: parent
        treeItem: treeWindow.treeItem
    }
}