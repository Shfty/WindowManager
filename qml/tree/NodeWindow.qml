import QtQuick 2.12
import QtQuick.Window 2.12

import ".."

Item {
    id: nodeWindow
    objectName: treeItem.objectName + " Item"
    anchors.fill: parent

    property real animationRate: Window.screen ? Window.screen.refreshRate / 60 : 1

    // Recursive tree delegate
    Component {
        id: nodeDelegate
        NodeDelegate {}
    }

    // Root tree item
    NodeDelegate {
        id: rootNode
        model: treeItem
    }
}