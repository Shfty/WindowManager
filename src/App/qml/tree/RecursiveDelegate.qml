import QtQuick 2.12
import QtQuick.Controls 2.5

import ".."

Item {
    id: recursiveWrapper

    objectName: model ? model.objectName : "Recursive Wrapper"

    property var model: null
    property var visualDelegate: null

    // Positioning
    property string boundsProperty: null
    property rect delegateBounds: {
        if (!boundsProperty)
            return Qt.rect(0, 0, 0, 0)
        if (!model)
            return Qt.rect(0, 0, 0, 0)
        return model[boundsProperty]
    }

    Behavior on delegateBounds {
        enabled: model ? true : false
        PropertyAnimation {
            duration: appCore.settingsContainer.itemAnimationDuration
            easing.type: appCore.settingsContainer.itemAnimationCurve
            onRunningChanged: model.isAnimating = running
        }
    }

    x: delegateBounds.x
    y: delegateBounds.y - parent.y
    width: delegateBounds.width
    height: delegateBounds.height

    // Visual delegate
    Incubator {
        id: visualWrapper
        anchors.fill: parent

        sourceComponent: visualDelegate
        properties: ({
            model: recursiveWrapper.model
        })
    }

    // Child elements
    property var incubateReady: model && childWrapper.children.length === 0
    onIncubateReadyChanged: {
        if (incubateReady) {
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
            for(var i = 0; i < childWrapper.children.length; ++i) {
                var candidate = childWrapper.children[i]
                if (candidate.model === child) {
                    candidate.destroy()
                }
            }
        }
    }

    function incubateItem(childModel) {
        recursiveDelegate.incubateObject(
            childWrapper, {
                model: childModel,
                visualDelegate: recursiveWrapper.visualDelegate,
                boundsProperty: recursiveWrapper.boundsProperty,
                childBoundsProperty: recursiveWrapper.childBoundsProperty,
                clipChildren: recursiveWrapper.clipChildren
            }
        )
    }

    property bool clipChildren: false
    property string childBoundsProperty: null
    property rect childBounds: {
        if (!childBoundsProperty)
            return Qt.rect(0, 0, delegateBounds.width, delegateBounds.height)
        if (!model)
            return Qt.rect(0, 0, delegateBounds.width, delegateBounds.height)
        return model[childBoundsProperty]
    }

    Behavior on childBounds {
        enabled: model ? true : false
        PropertyAnimation {
            easing.type: appCore.settingsContainer.itemAnimationCurve
            duration: appCore.settingsContainer.itemAnimationDuration
        }
    }

    Item {
        id: childWrapper
        objectName: recursiveWrapper.objectName + " Child Wrapper"
        x: childBounds.x
        y: childBounds.y
        width: childBounds.width
        height: childBounds.height
        clip: clipChildren
    }
}
