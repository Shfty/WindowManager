import QtQuick 2.0

Item {
    id: multiIncubator

    property bool active: true
    property var model: null
    property var sourceComponent: null
    property var properties: null

    property var targetModel: active && model ? model : []
    property var cachedModel: []

    property var itemIncubators: []
    property var itemInstances: []

    property var status: Component.Null

    Component.onDestruction: {
        itemIncubators.length = 0

        for(var itemInstance in itemInstances)
        {
            itemInstances[itemInstance].destroy()
        }
        itemInstances.length = 0
    }

    onTargetModelChanged: {
        var removedItems = cachedModel.filter(function(element) {
            return !targetModel.includes(element)
        })

        var addedItems = targetModel.filter(function(element) {
            return !cachedModel.includes(element)
        })

        cachedModel = targetModel.slice()
        print("model changed, removed: ", removedItems, " added: ", addedItems)

        for(var removedKey in removedItems)
        {
            var removedModelData = removedItems[removedKey]
            var removedInstance = itemInstances.find(function(element) {
                return element.modelData === removedModelData
            })

            print("destroying instance", removedInstance)
            removedInstance.destroy()
        }

        for(var addedKey in addedItems)
        {
            var addedModelData = addedItems[addedKey]

            print("incubating instance with", addedModelData)

            var props = {
                modelData: addedModelData
            }

            for(var prop in properties)
            {
                props[prop] = properties[prop]
            }

            var incubator = sourceComponent.incubateObject(multiIncubator, props)

            itemIncubators.push(incubator)

            incubator.onStatusChanged = function(status) {
                if(status === Component.Ready)
                {
                    itemInstances.push(incubator.object)
                }
            }
        }
    }
}
