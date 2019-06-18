import QtQuick 2.12
import QtQuick.Controls 2.5
import DWMThumbnail 1.0

import ".."

Item {
    id: itemWrapper

    property var model: null
    property bool hasModel: model ? true : false

    Loader {
        anchors.fill: parent
        active: hasModel ? model.depth > 0 : false

        sourceComponent: Item {
            clip: true

            Rectangle {
                id: itemBackground
                anchors.fill: parent

                border.color: appCore.settingsContainer.colorContainerBorder
                border.width: appCore.settingsContainer.itemBorder

                radius: appCore.settingsContainer.itemRadius
                anchors.topMargin: -radius

                color: appCore.settingsContainer.colorContainerPlaceholder

                AppIcon {
                    id: appIcon

                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter

                    model: hasModel ? itemWrapper.model : null
                }
            }
        }
    }

    DWMThumbnail {
        objectName: model.objectName
        anchors.fill: parent
        hwnd: {
            if(!hasModel) return appCore.windowView.windowList[0].hwnd
            if(!model.windowInfo) return appCore.windowView.windowList[0].hwnd
            return model.windowInfo.hwnd
        }
    }
}
