import QtQuick 2.11
import QtQuick.Window 2.11
import QtGraphicalEffects 1.0
import QtQuick.Controls.Universal 2.3

import ".."

Item {
    id: overlayWindow

    property var hasTreeItem: typeof treeItem != 'undefined'
    objectName: hasTreeItem ? treeItem.objectName + " Overlay" : "Overlay"

    Universal.theme: Universal.Dark
}