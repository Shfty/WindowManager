import QtQuick 2.12
import QtQuick.Window 2.12

import ".."

Item {
    id: nodeWindow
    objectName: treeItem.objectName + " Item"
    anchors.fill: parent

    property real animationRate: Window.screen ? Window.screen.refreshRate / 60 : 1

    // Visual delegate
    Component {
        id: visualDelegate
        NodeDelegate {}
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