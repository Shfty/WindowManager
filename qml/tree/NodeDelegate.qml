import QtQuick 2.12
import QtQuick.Controls 2.5
import DWMThumbnail 1.0

import ".."

Item {
    id: itemWrapper

    parent: nodeWindow

    property var model: null

    property var animationEasing: Easing.OutCubic
    property int animationDuration: 300 * nodeWindow.animationRate

    // Positioning and animation
    x: model ? model.contentBounds.x : 0
    y: model ? model.contentBounds.y : 0
    width: model ? model.contentBounds.width : 0
    height: model ? model.contentBounds.height : 0

    Behavior on x {
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
        SequentialAnimation {
            ScriptAction { script: model.isAnimating = true }
            NumberAnimation {
                duration: animationDuration
                easing.type: animationEasing
            }
            ScriptAction { script: model.isAnimating = false }
        }
    }

    // Visual components
    Rectangle {
        id: itemBackground
        anchors.fill: parent

        visible: model ? model.depth > 1 : false

        color: appCore.settingsContainer.colorContainerPlaceholder
    }

    DWMThumbnail {
        anchors.fill: parent
        hwnd: model.windowInfo ? model.windowInfo.hwnd : appCore.windowView.windowList[0].hwnd
    }

    // Child handling
    property var childItems: []
    property var itemBuffer: []
    property var incubator: null

    function addChild(childModel) {
        if(incubator == null)
        {
            incubateItem(childModel);
        }
        else
        {
            itemBuffer.push(childModel);
        }
    }

    function incubateItem(childModel) {
        incubator = nodeDelegate.incubateObject(
            nodeWindow,
            {
                model: childModel
            }
        );

        if (incubator.status !== Component.Ready) {
            incubator.onStatusChanged = function(status) {
                if (status === Component.Ready) {
                    incubatorReady();
                }
            }
        } else {
            incubatorReady();
        }
    }

    function incubatorReady() {
        childItems.push(incubator.object)
        if(itemBuffer.length > 0) {
            var item = itemBuffer.shift()
            incubateItem(item)
        }
        else {
            incubator = null
        }
    }

    function remove() {
        for(var i = 0; i < childItems.length; ++i)
        {
            childItems[i].remove()
        }

        destroy()
    }

    Component.onCompleted: {
        if(!model) return

        for(var i = 0; i < model.children.length; ++i)
        {
            addChild(model.children[i])
        }
    }

    Connections {
        target: model
        onChildAdded: function(index, child) {
            addChild(child)
        }
        onChildRemoved: function(index, child) {
            for(var i = 0; i < childItems.length; ++i)
            {
                var candidate = childItems[i]
                if(candidate.model === child)
                {
                    childItems.splice(i, 1)
                    candidate.remove()
                }
            }
        }
    }
}