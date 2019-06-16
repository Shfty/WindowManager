import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.5
import QtQuick.Controls.Universal 2.3

import ".."

Rectangle {
    id: overlayWindow
    objectName: "ConfigOverlay"

    Universal.theme: Universal.Dark

    anchors.fill: parent

    color: "black"
    property bool isVisible: false

    property var windowActive: Window.active
    onWindowActiveChanged: {
        if(isVisible && !windowActive) hide()
    }

    function show(x, y, width, height) {

        Window.window.width = width
        Window.window.height = height
        Window.window.x = x
        Window.window.y = y
        Window.window.requestActivate()

        isVisible = true
    }

    function hide() {
        var offscreen = appCore.windowView.getOffscreenArea()

        if(Window.window)
        {
            Window.window.x = offscreen.x
            Window.window.y = offscreen.y
        }

        isVisible = false
    }

    function toggle(x, y, width, height) {
        if(isVisible && x === Window.window.x || y === Window.window.y) {
            hide()
        }
        else {
            show(x, y, width, height)
        }
    }

    TabBar {
        id: bar

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        TabButton {
            text: qsTr("Tree")
        }
        TabButton {
            text: qsTr("Windows")
        }
        TabButton {
            text: qsTr("Monitors")
        }
        TabButton {
            text: qsTr("Settings")
        }
    }

    StackLayout {
        anchors.top: bar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        currentIndex: bar.currentIndex
        anchors.margins: 10

        clip: true

        TreeTab {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
        WindowList {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
        MonitorsTab {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
        SettingsTab {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
}