import QtQuick 2.0

QtObject {
    property var target: []
    property var cachedTarget: []

    signal itemAdded(var item)
    signal itemRemoved(var item)

    onTargetChanged: {
        var removedItems = cachedTarget.filter(function(element) {
            return !target.includes(element)
        })

        var addedItems = target.filter(function(element) {
            return !cachedTarget.includes(element)
        })

        cachedTarget = target.slice()
        print("model changed, removed: ", removedItems, " added: ", addedItems)

        for(var removedKey in removedItems)
        {
            var removedModelData = removedItems[removedKey]
            var removedInstance = incubators.find(function(element) {
                return element.object.modelData === removedModelData
            })

            itemRemoved(removedInstance)
        }

        for(var addedKey in addedItems)
        {
            var addedItem = addedItems[addedKey]

            itemAdded(addedItem)
        }
    }
}
