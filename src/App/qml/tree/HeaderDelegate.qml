import QtQuick 2.12
import QtQuick.Controls 2.5

import ".."

TreeHeader {
    id: treeHeader

    property bool hasModel: model ? true : false

    x: hasModel ? model.headerBounds.x : 0
    y: hasModel ? model.headerBounds.y : 0
    width: hasModel ? model.headerBounds.width : 0
    height: hasModel ? model.headerBounds.height : 0

    property var animationEasing: Easing.OutCubic
    property int animationDuration: 300

    Component.onCompleted: {
        print("NodeDelegate Completed", model)
    }

    Behavior on x {
        enabled: hasModel ? true : false
        NumberAnimation {
            duration: animationDuration
            easing.type: animationEasing
        }
    }

    Behavior on y {
        enabled: hasModel ? true : false
        NumberAnimation {
            duration: animationDuration
            easing.type: animationEasing
        }
    }

    Behavior on width {
        enabled: hasModel ? true : false
        NumberAnimation {
            duration: animationDuration
            easing.type: animationEasing
        }
    }

    Behavior on height {
        enabled: hasModel ? true : false
        NumberAnimation {
            duration: animationDuration
            easing.type: animationEasing
        }
    }

    /*
    Drag.active: dragHandler.active
    Drag.source: treeHeader
    Drag.hotSpot.x: treeHeader.width / 2
    Drag.hotSpot.y: appCore.settingsContainer.headerSize / 2

    DragHandler {
        id: dragHandler
        xAxis.enabled: model.treeParent.flow === "Horizontal"
        yAxis.enabled: model.treeParent.flow === "Vertical"
    }
    */
}