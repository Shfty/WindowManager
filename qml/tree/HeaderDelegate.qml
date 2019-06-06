import QtQuick 2.12
import QtQuick.Controls 2.5

import ".."

TreeHeader {
    id: treeHeader

    x: model ? model.headerBounds.x : 0
    y: model ? model.headerBounds.y : 0
    width: model ? model.headerBounds.width : 0
    height: model ? model.headerBounds.height : 0

    property var animationEasing: Easing.OutCubic
    property int animationDuration: 300

    Behavior on x {
        enabled: model ? true : false
        SequentialAnimation {
            ScriptAction { script: model.isAnimating = true }
            NumberAnimation {
                duration: animationDuration
                easing.type: animationEasing
            }
            ScriptAction { script: model.isAnimating = false }
        }
    }

    Behavior on y {
        enabled: model ? true : false
        SequentialAnimation {
            ScriptAction { script: model.isAnimating = true }
            NumberAnimation {
                duration: animationDuration
                easing.type: animationEasing
            }
            ScriptAction { script: model.isAnimating = false }
        }
    }

    Behavior on width {
        enabled: model ? true : false
        SequentialAnimation {
            ScriptAction { script: model.isAnimating = true }
            NumberAnimation {
                duration: animationDuration
                easing.type: animationEasing
            }
            ScriptAction { script: model.isAnimating = false }
        }
    }

    Behavior on height {
        enabled: model ? true : false
        SequentialAnimation {
            ScriptAction { script: model.isAnimating = true }
            NumberAnimation {
                duration: animationDuration
                easing.type: animationEasing
            }
            ScriptAction { script: model.isAnimating = false }
        }
    }

    Drag.active: dragHandler.active
    Drag.source: treeHeader
    Drag.hotSpot.x: treeHeader.width / 2
    Drag.hotSpot.y: appCore.settingsContainer.headerSize / 2

    DragHandler {
        id: dragHandler
        xAxis.enabled: model.treeParent.flow === "Horizontal"
        yAxis.enabled: model.treeParent.flow === "Vertical"
    }
}