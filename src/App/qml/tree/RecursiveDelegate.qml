import QtQuick 2.12
import QtQuick.Controls 2.5

import ".."

Item {
    id: recursiveWrapper

    objectName: model ? model.objectName : "Recursive Wrapper"

    property var model: null
    property var visualDelegate: null

    property var animationEasing: Easing.OutCubic
    property int animationDuration: 300

    property int loadedChildren: 0
    signal childrenLoaded

    property string boundsProperty: null
    property rect delegateBounds: {
        if (!boundsProperty)
            return Qt.rect(0, 0, 0, 0)
        if (!model)
            return Qt.rect(0, 0, 0, 0)
        return model[boundsProperty]
    }

    property string childBoundsProperty: null
    property rect childBounds: {
        if (!childBoundsProperty)
            return Qt.rect(0, 0, delegateBounds.width, delegateBounds.height)
        if (!model)
            return Qt.rect(0, 0, delegateBounds.width, delegateBounds.height)
        return model[childBoundsProperty]
    }

    property bool clipChildren: false

    // Positioning and animation
    x: delegateBounds.x
    y: delegateBounds.y - parent.y
    width: delegateBounds.width
    height: delegateBounds.height

    // Business logic
    property var ready: model && loadedChildren === 0
    onReadyChanged: {
        if (ready) {
            for (var i = 0; i < recursiveWrapper.model.children.length; ++i) {
                incubateItem(recursiveWrapper.model.children[i])
            }
        }
    }

    Connections {
        target: model
        onChildAdded: function (index, child) {
            incubateItem(child)
        }
        onChildRemoved: function (index, child) {
            for (; i < childItems.length; ++i) {
                var candidate = childItems[i]
                if (candidate.model === child) {
                    childItems.splice(i, 1)
                    candidate.remove()
                }
            }
        }
    }

    // Child elements
    Loader {
        anchors.fill: parent
        sourceComponent: recursiveWrapper.visualDelegate
        onLoaded: {
            if (item.model !== undefined) {
                item.model = recursiveWrapper.model
            }
        }
    }

    Item {
        id: childWrapper
        objectName: recursiveWrapper.objectName + " Child Wrapper"
        x: childBounds.x
        y: childBounds.y
        width: childBounds.width
        height: childBounds.height
        clip: true
    }

    // Child handling
    property var childItems: []

    function incubateItem(childModel) {
        var incubator = recursiveDelegate.incubateObject(childWrapper, {
                                                             "model": childModel,
                                                             "visualDelegate": recursiveWrapper.visualDelegate,
                                                             "boundsProperty": recursiveWrapper.boundsProperty,
                                                             "childBoundsProperty": recursiveWrapper.childBoundsProperty,
                                                             "clipChildren": recursiveWrapper.clipChildren
                                                         })

        if (incubator.status !== Component.Ready) {
            incubator.onStatusChanged = function (status) {
                if (status === Component.Ready) {
                    incubatorReady(incubator.object, childModel)
                }
            }
        } else {
            incubatorReady(incubator.object, childModel)
        }
    }

    function incubatorReady(object, model) {
        childItems.push(object)
        loadedChildren++
        if (loadedChildren == childItems.length) {
            childrenLoaded()
        }
    }

    function remove() {
        for (; i < childItems.length; ++i) {
            childItems[i].remove()
        }

        destroy()
    }

    // Animation
    Behavior on x {
        enabled: model ? true : false
        NumberAnimation {
            duration: animationDuration
            easing.type: animationEasing
            onRunningChanged: model.isAnimating = running
        }
    }

    Behavior on y {
        enabled: model ? true : false
        NumberAnimation {
            duration: animationDuration
            easing.type: animationEasing
            onRunningChanged: model.isAnimating = running
        }
    }

    Behavior on width {
        enabled: model ? true : false
        NumberAnimation {
            duration: animationDuration
            easing.type: animationEasing
            onRunningChanged: model.isAnimating = running
        }
    }

    Behavior on height {
        enabled: model ? true : false
        NumberAnimation {
            duration: animationDuration
            easing.type: animationEasing
            onRunningChanged: model.isAnimating = running
        }
    }
}
