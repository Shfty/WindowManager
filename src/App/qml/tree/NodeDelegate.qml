import QtQuick 2.12
import QtQuick.Controls 2.5
import DWMThumbnail 1.0

import ".."

Item {
    id: itemWrapper

    anchors.fill: parent

    property var modelData: null
    property bool hasModel: modelData ? true : false

    Incubator {
        anchors.fill: parent
        active: hasModel ? modelData.depth > 0 : false

        sourceComponent: Component {
            Item {
                anchors.fill: parent
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

                        model: hasModel ? itemWrapper.modelData : null
                    }
                }
            }
        }
    }

    DWMThumbnail {
        anchors.fill: parent
        hwnd: {
            if(!appCore) return undefined
            if(!hasModel) return appCore.windowView.windowList[0].hwnd
            if(!modelData.windowInfo) return appCore.windowView.windowList[0].hwnd
            return modelData.windowInfo.hwnd
        }
    }
}
