import QtQuick 2.0

Item {
    id: multiIncubator

    property bool active: true
    property bool asynchronous: false
    property var sourceComponent: null
    property var properties: ({})
    property var model: null

    property var incubators: []

    Component.onDestruction: {
        for(var incubatorInstance in incubators)
        {
            incubators[incubatorInstance].object.destroy()
        }

        incubators.length = 0
    }

    ArrayWatcher {
        target: active && model ? model : []
        onItemAdded: function(item) {
            var props = {
                modelData: item
            }

            for(var prop in properties)
            {
                props[prop] = properties[prop]
            }

            var incubator = sourceComponent.incubateObject(multiIncubator, props, asynchronous ? Qt.Asynchronous : Qt.Synchronous)
            incubators.push(incubator)
        }

        onItemRemoved: function(item) {
            item.destroy()
        }
    }
}
