import QtQuick 2.12
import QtQuick.Controls 2.5

import ".."

Item {
    id: recursiveWrapper

    property var model: null
    property var delegate: null

    property var animationEasing: Easing.OutCubic
    property int animationDuration: 300

    property int loadedChildren: 0
    signal childrenLoaded

    // Positioning and animation
    x: model ? model.bounds.x : 0
    y: model ? model.bounds.y : 0
    width: model ? model.bounds.width : 0
    height: model ? model.bounds.height : 0

    clip: true

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
    Loader {
        sourceComponent: recursiveWrapper.delegate

        onLoaded: {
            item.model = recursiveWrapper.model
        }
    }

    // Child handling
    property var childItems: []

    function incubateItem(childModel) {
        var incubator = recursiveDelegate.incubateObject(
            recursiveWrapper,
            {
                model: childModel
            }
        )

        incubator.forceCompletion()

        if (incubator.status !== Component.Ready) {
            incubator.onStatusChanged = function(status) {
                if (status === Component.Ready) {
                    incubatorReady(incubator.object)
                }
            }
        } else {
            incubatorReady(incubator.object)
        }
    }

    function incubatorReady(object)
    {
        childItems.push(object)
        loadedChildren++
        if(loadedChildren == childItems.length)
        {
            childrenLoaded()
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
        console.log("RecursiveDelegate Completed", model);

        if(!model) return

        for(var i = 0; i < model.children.length; ++i)
        {
            incubateItem(model.children[i])
        }
    }

    Connections {
        target: model
        onChildAdded: function(index, child) {
            incubateItem(child)
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