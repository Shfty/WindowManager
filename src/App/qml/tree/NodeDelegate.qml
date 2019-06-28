import QtQuick 2.12
import QtQuick.Controls 2.5
import DWMThumbnail 1.0

import ".."

Item {
    id: itemWrapper

    anchors.fill: parent

    Component.onDestruction: {
        print("Node Delegate Destruction", modelData.objectName)
    }

    property var modelData: null
    property bool hasModel: modelData ? true : false

    Incubator {
        anchors.fill: parent
        active: hasModel ? modelData.depth > 0 : false

        Component.onDestruction: {
            print("Node Delegate Incubator Destruction", modelData.objectName)
        }

        sourceComponent: Component {
            Item {
                anchors.fill: parent
                clip: true

                Component.onDestruction: {
                    print("Node Delegate Incubator Item Destruction", modelData.objectName)
                }

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
        objectName: modelData.objectName
        anchors.fill: parent
        hwnd: {
            if(!hasModel) return appCore.windowView.windowList[0].hwnd
            if(!modelData.windowInfo) return appCore.windowView.windowList[0].hwnd
            return modelData.windowInfo.hwnd
        }
    }
}
