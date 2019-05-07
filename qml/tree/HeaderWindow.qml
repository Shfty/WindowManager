import QtQuick 2.11
import QtQuick.Window 2.11
import QtGraphicalEffects 1.0
import QtQuick.Controls.Universal 2.3

import ".."

Item {
    id: headerWindow
    objectName: treeItem.objectName + " Header"

    Universal.theme: Universal.Dark

    // Recursive tree delegate
    Component {
        id: headerDelegate
        HeaderDelegate {}
    }

    // Root tree item
    HeaderDelegate {
        id: rootItem
        anchors.fill: parent
        model: treeItem
        delegate: headerDelegate
    }
}
