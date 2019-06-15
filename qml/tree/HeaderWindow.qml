import QtQuick 2.12
import QtQuick.Window 2.12

import ".."

Item {
    id: headerWindow
    objectName: treeItem.objectName + " Header"
    anchors.fill: parent

    property real animationRate: {
        if(!Window) return 1
        if(!Window.screen) return 1
        if(!Window.screen.refreshRate) return 1
        return Window.screen.refreshRate / 60
    }

    // Visual delegate
    Component {
        id: visualDelegate
        HeaderDelegate {}
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
            opacity = 1
        }
    }
}
