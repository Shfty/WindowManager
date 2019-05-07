import QtQuick 2.11
import QtQml 2.2

import "."

QtObject {
    id: rootObject

    property var model: treeItem

    signal showTrayIconWindow(point position)
    signal shutdown()
    signal restart()
    signal sleep()
    
    property Instantiator windowInstantiator: Instantiator {
        model: treeItem.children
        asynchronous: true

        QtObject {
            property var headerWindow: HeaderWindow {}
            property var itemWindow: ItemWindow {}
        }
    }
}