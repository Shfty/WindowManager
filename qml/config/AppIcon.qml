import QtQuick 2.12

Image {
    property var model: null

    source: {
        if(model && model.windowInfo)
        {
            return "image://treeIcon/" + model.windowInfo.winProcess
        }

        if(model && model.launchUri)
        {
            return "image://treeIcon/" + model.launchUri
        }

        return ""
    }
}