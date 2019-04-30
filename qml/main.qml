import QtQuick 2.11
import QtQml 2.2
import Process 1.0

import "tree"

QtObject {
    id: rootObject

    property bool hasModel: typeof treeModel !== "undefined"
    property var model: hasModel ? treeModel : null
    
    signal showTrayIconWindow(point position)

    signal showConfigWindow()
    onShowConfigWindow:{
        configWindow.show()
        configWindow.requestActivate()
    }
    
    property Instantiator windowInstantiator: Instantiator {
        model: rootObject.model ? rootObject.model.children : null
        asynchronous: true

        QtObject {
            property var treeWindow: TreeWindow {
                thumbnailOverlay: thumbnailWindow
            }
            property var thumbnailWindow: ThumbnailOverlayWindow {}
        }
    }

    property var configWindow: TestWindow {
        visible: false
        model: rootObject.model ? rootObject.model.children : null
    }

    property var process: Process {}
}