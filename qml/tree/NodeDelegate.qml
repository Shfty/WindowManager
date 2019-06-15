import QtQuick 2.12
import QtQuick.Controls 2.5
import DWMThumbnail 1.0

import ".."

Item {
    id: itemWrapper

    property var model: null
    property bool hasModel: model ? true : false

    x: hasModel ? model.contentBounds.x : 0
    y: hasModel ? model.contentBounds.y : 0
    width: hasModel ? model.contentBounds.width : 0
    height: hasModel ? model.contentBounds.height : 0

    visible: hasModel ? model.isVisible : false

    property var animationEasing: Easing.OutCubic
    property int animationDuration: 300

    Component.onCompleted: {
        print("NodeDelegate Completed", model)
    }

    Behavior on x {
        enabled: hasModel ? true : false
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
        enabled: hasModel ? true : false
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
        enabled: hasModel ? true : false
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
        enabled: hasModel ? true : false
        SequentialAnimation {
            ScriptAction { script: model.isAnimating = true }
            NumberAnimation {
                duration: animationDuration
                easing.type: animationEasing
            }
            ScriptAction { script: model.isAnimating = false }
        }
    }

    Item {
        anchors.fill: parent
        clip: true

        Rectangle {
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            height: 1

            visible: itemBackground.radius > 1

            color: appCore.settingsContainer.colorContainerBorder
        }

        Rectangle {
            id: itemBackground
            anchors.fill: parent

            border.color: appCore.settingsContainer.colorContainerBorder
            border.width: appCore.settingsContainer.itemBorder

            radius: appCore.settingsContainer.itemRadius
            anchors.topMargin: -radius

            visible: hasModel ? model.depth > 1 : false

            color: appCore.settingsContainer.colorContainerPlaceholder

            AppIcon {
                id: appIcon

                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter

                model: hasModel ? itemWrapper.model : null
            }
        }

        DWMThumbnail {
            anchors.fill: parent
            hwnd: {
                if(!hasModel) return appCore.windowView.windowList[0].hwnd
                if(!model.windowInfo) return appCore.windowView.windowList[0].hwnd
                return model.windowInfo.hwnd
            }
        }
    }
}
