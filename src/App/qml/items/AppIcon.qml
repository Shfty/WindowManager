import QtQuick 2.12
import QtQuick.Controls 2.12

Image {
    id: appIcon

    property var model: null

    asynchronous: true

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

    BusyIndicator {
        anchors.fill: parent
        running: appIcon.status !== Image.Ready
    }
}