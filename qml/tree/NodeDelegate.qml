import QtQuick 2.12
import QtQuick.Controls 2.5
import DWMThumbnail 1.0

import ".."

Item {
    id: itemWrapper

    property var model: null

    x: model ? model.contentBounds.x : 0
    y: model ? model.contentBounds.y : 0
    width: model ? model.contentBounds.width : 0
    height: model ? model.contentBounds.height : 0

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

    Rectangle {
        id: itemBackground
        anchors.fill: parent

        visible: model ? model.depth > 1 : false

        color: appCore.settingsContainer.colorContainerPlaceholder

        AppIcon {
            id: icon

            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter

            model: itemWrapper.model
        }
    }

    DWMThumbnail {
        anchors.fill: parent
        hwnd: model.windowInfo ? model.windowInfo.hwnd : appCore.windowView.windowList[0].hwnd
        clipTarget: itemWrapper.parent
    }
}
