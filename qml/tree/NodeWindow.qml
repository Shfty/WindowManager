import QtQuick 2.12
import QtQuick.Window 2.12

import ".."

Item {
    id: nodeWindow
    objectName: treeItem.objectName + " Item"
    anchors.fill: parent

    Component.onCompleted: {
        print("NodeWindow onCompleted")
    }

    property real animationRate: {
        print("NodeWindow updating animationRate")
        if(!Window) return 1
        if(!Window.screen) return 1
        if(!Window.screen.refreshRate) return 1
        return Window.screen.refreshRate / 60
    }

    // Visual delegate
    Component {
        id: visualDelegate
        Item {
            id: wrapper
            anchors.fill: parent
            property var model: null
            NodeDelegate {
                model: wrapper.model
            }
            HeaderDelegate {
                model: wrapper.model
            }
        }
    }

    // Recursive tree delegate
    Component {
        id: recursiveDelegate
        RecursiveDelegate {
            delegate: visualDelegate
            animationDuration: 300 * animationRate
        }
    }

    // Root tree item
    RecursiveDelegate {
        id: rootNode
        model: treeItem
        delegate: visualDelegate

        opacity: 0
        Behavior on opacity {
            NumberAnimation {
                duration: 300 * animationRate
            }
        }

        onChildrenLoaded: {
            spinner.opacity = 0
            opacity = 1
        }
    }

    Spinner {
        id: spinner
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        size: 240
        dotScale: 0.8
        color: "#D0FFFFFF"

        opacity: 1
        Behavior on opacity {
            NumberAnimation {
                duration: 300 * animationRate
            }
        }
    }
}