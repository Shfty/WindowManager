import QtQuick 2.12

Item {
    id: incubator

    property bool active: true
    property var sourceComponent: null
    property var properties: null

    property var itemIncubator: null
    property var itemInstance: null
    property var status: Component.Null

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
        if(itemIncubator === null && itemInstance === null)
        {
            itemIncubator = sourceComponent.incubateObject(incubator, properties)

            itemIncubator.onStatusChanged = function(status) {
                if(status === Component.Ready)
                {
                    incubator.status = status
                    incubator.itemInstance = itemIncubator.object
                }
            }
        }
    }

    function destroyItem()
    {
        if(itemIncubator !== null)
        {
            if(itemIncubator.status !== Component.Null)
            {
                itemIncubator.forceCompletion();
            }
            itemIncubator = null;
        }

        if(itemInstance !== null)
        {
            itemInstance.destroy()
            itemInstance = null
        }
    }
}