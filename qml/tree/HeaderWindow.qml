import QtQuick 2.12
import QtQuick.Window 2.12

import ".."

Item {
    id: headerWindow
    objectName: treeItem.objectName + " Header"
    anchors.fill: parent

    property real animationRate: Window.screen ? Window.screen.refreshRate / 60 : 1

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
    }
}
