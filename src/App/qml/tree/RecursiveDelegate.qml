import QtQuick 2.12
import QtQuick.Controls 2.5

import ".."

Item {
    id: recursiveWrapper

    objectName: modelData ? modelData.objectName : "Recursive Wrapper"

    property var modelData: null
    property var visualDelegate: null

    readonly property bool visualLoaded: visualWrapper.status === Component.Ready
    readonly property bool childrenLoaded: {
        for(var childKey in childIncubator.itemInstances)
        {
            var instance = childIncubator.itemInstances[childKey]
            if(instance.loadComplete === false)
            {
                return false
            }
        }

        return true
    }

    readonly property bool loadComplete: visualLoaded && childrenLoaded

    onLoadCompleteChanged: {
        print(objectName, "Load Complete", loadComplete)
    }

    // Positioning
    property string boundsProperty: null
    property rect delegateBounds: {
        if (!boundsProperty)
            return Qt.rect(0, 0, 0, 0)
        if (!modelData)
            return Qt.rect(0, 0, 0, 0)
        return modelData[boundsProperty]
    }

    Behavior on delegateBounds {
        enabled: modelData ? true : false
        PropertyAnimation {
            duration: appCore.settingsContainer.itemAnimationDuration
            easing.type: appCore.settingsContainer.itemAnimationCurve
            onRunningChanged: modelData.isAnimating = running
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
            modelData: recursiveWrapper.modelData
        })
    }

    property bool clipChildren: false
    property string childBoundsProperty: null
    property rect childBounds: {
        if (!childBoundsProperty)
            return Qt.rect(0, 0, delegateBounds.width, delegateBounds.height)
        if (!modelData)
            return Qt.rect(0, 0, delegateBounds.width, delegateBounds.height)
        return modelData[childBoundsProperty]
    }

    Behavior on childBounds {
        enabled: modelData ? true : false
        PropertyAnimation {
            easing.type: appCore.settingsContainer.itemAnimationCurve
            duration: appCore.settingsContainer.itemAnimationDuration
        }
    }

    MultiIncubator {
        id: childIncubator

        x: childBounds.x
        y: childBounds.y
        width: childBounds.width
        height: childBounds.height
        clip: clipChildren

        active: recursiveWrapper.modelData && childIncubator.itemInstances.length === 0
        model: recursiveWrapper.modelData ? recursiveWrapper.modelData.children : null
        sourceComponent: recursiveDelegate
        properties: ({
            visualDelegate: recursiveWrapper.visualDelegate,
            boundsProperty: recursiveWrapper.boundsProperty,
            childBoundsProperty: recursiveWrapper.childBoundsProperty,
            clipChildren: recursiveWrapper.clipChildren
        })
    }
}
