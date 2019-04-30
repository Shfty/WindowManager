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

    // Blur wallpaper
    property Image wallpaper: Image {
        width: desktopRect.width
        height: desktopRect.height

        asynchronous: true
        cache: true

        source: Options.wallpaperUrl
        fillMode: Options.wallpaperFillMode
    }

    property GaussianBlur blurWallpaper: GaussianBlur {
        anchors.fill: wallpaper

        visible: false

        source: wallpaper
        cached: true

        radius: Options.wallpaperBlurRadius
        samples: 1 + radius * 2
    }

    // Recursive tree delegate
    Component {
        id: treeDelegate

        TreeDelegate {}
    }

    // Non-blurred background
    ShaderEffectSource {
        anchors.fill: parent

        sourceItem: wallpaper
        sourceRect: Qt.rect(treeWindow.x - desktopRect.x,
                            treeWindow.y - desktopRect.y,
                            treeWindow.width,
                            treeWindow.height)
    }

    // Root tree item
    TreeDelegate {
        id: rootItem
        anchors.fill: parent
        treeItem: treeWindow.treeItem
    }
}