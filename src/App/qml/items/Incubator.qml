import QtQuick 2.12

Item {
    id: incubatorItem

    property bool active: true
    property bool asynchronous: false
    property var sourceComponent: null
    property var properties: ({})

    property var incubator: null
    property int status: Component.Null
    readonly property var item: {
        if(status === Component.Ready)
        {
            if(incubator)
            {
                return incubator.object
            }
        }

        return null
    }

    Component.onCompleted: {
        onActiveChanged.connect(updateItem)
        onSourceComponentChanged.connect(updateItem)
        updateItem()
    }

    Component.onDestruction: {
        destroyItem()
    }

    function updateItem() {
        if(active && sourceComponent !== null)
        {
            incubateItem()
        }
        else
        {
            destroyItem()
        }
    }

    function incubateItem()
    {
        if(incubator === null && item === null)
        {
            incubator = sourceComponent.incubateObject(incubatorItem, properties, asynchronous ? Qt.Asynchronous : Qt.Synchronous)
            incubator.onStatusChanged = function(status) {
                incubatorItem.status = status
            }

            status = incubator.status
        }
    }

    function destroyItem()
    {
        if(incubator !== null)
        {
            if(incubator.status !== Component.Null)
            {
                incubator.forceCompletion();
                incubator.object.destroy();
            }
            incubator = null;
            status = Component.Null;
        }
    }
}