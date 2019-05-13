import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.5
import QtQuick.Controls.Universal 2.3

Rectangle {
    id: overlayWindow
    objectName: "PowerMenuOverlay"

    Universal.theme: Universal.Dark

    anchors.fill: parent

    color: "black"
    property bool isVisible: false

    property var windowActive: Window.active
    onWindowActiveChanged: {
        if(isVisible && !windowActive) hide()
    }

    function show(x, y) {
        Window.window.x = x
        Window.window.y = y
        Window.window.height = powerMenu.contentHeight
        Window.window.requestActivate()

        isVisible = true
    }

    function hide() {
        var offscreen = appCore.windowView.getOffscreenArea()

        Window.window.x = offscreen.x
        Window.window.y = offscreen.y

        isVisible = false
    }

    function toggle(x, y) {
        if(isVisible && x === Window.window.x || y === Window.window.y) {
            hide()
        }
        else {
            show(x, y)
        }
    }

    ListView {
        id: powerMenu
        spacing: 0

        anchors.fill: parent

        model: ListModel {
            ListElement {
                name: "Exit"
                func: function() {
                    Qt.quit();
                }
            }
            ListElement {
                name: "Shut Down"
                func: function() {
                    appCore.shutdown();
                }
            }
            ListElement {
                name: "Restart"
                func: function() {
                    appCore.restart();
                }
            }
            ListElement {
                name: "Sleep"
                func: function() {
                    appCore.sleep();
                }
            }
        }

        delegate: ItemDelegate {
            width: parent.width

            text: name

            onClicked: func()
        }
    }
}