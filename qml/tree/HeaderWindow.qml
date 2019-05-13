import QtQuick 2.12
import QtQuick.Window 2.12

import ".."

Item {
    id: headerWindow
    objectName: treeItem.objectName + " Header"
    anchors.fill: parent

    property real animationRate: Window.screen ? Window.screen.refreshRate / 60 : 1

    // Recursive tree delegate
    Component {
        id: headerDelegate
        HeaderDelegate {}
    }

    // Root tree item
    HeaderDelegate {
        id: rootItem
        model: treeItem
    }
}
