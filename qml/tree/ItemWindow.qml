import QtQuick 2.11
import QtQuick.Window 2.11
import QtGraphicalEffects 1.0
import QtQuick.Controls.Universal 2.3

import ".."

Item {
    id: itemWindow
    objectName: treeItem.objectName + " Item"

    Universal.theme: Universal.Dark
    
    // Recursive tree delegate
    Component {
        id: itemDelegate
        ItemDelegate {}
    }

    // Root tree item
    ItemDelegate {
        id: rootItem
        anchors.fill: parent
        model: treeItem
        delegate: itemDelegate
    }
}