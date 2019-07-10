import QtQuick 2.12
import QtQuick.Controls 2.5
import DWMThumbnail 1.0

import ".."

Item {
    id: itemWrapper

    property var modelData: null
    readonly property bool hasModel: modelData ? true : false

    readonly property var settingsContainer: clientCore ? clientCore.settingsContainer : null
    readonly property real itemRadius: settingsContainer.itemRadius
    readonly property real itemBorder: settingsContainer.itemBorder
    readonly property color colorContainerBorder: settingsContainer.colorContainerBorder
    readonly property color colorContainerPlaceholder: settingsContainer.colorContainerPlaceholder

    Incubator {
        anchors.fill: parent
        active: hasModel ? modelData.depth > 0 : false

        sourceComponent: Component {
            Item {
                anchors.fill: parent
                clip: true

                // Background
                Rectangle {
                    id: itemBackground
                    anchors.fill: parent

                    border.color: colorContainerBorder
                    border.width: itemBorder

                    radius: itemRadius
                    anchors.topMargin: -radius

                    color: colorContainerPlaceholder

                    AppIcon {
                        id: appIcon

                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter

                        model: hasModel ? itemWrapper.modelData : null
                    }

                    Rectangle {
                        anchors.top: parent.top
                        anchors.topMargin: itemBackground.radius

                        anchors.left: parent.left
                        anchors.leftMargin: itemBorder

                        anchors.right: parent.right
                        anchors.rightMargin: itemBorder

                        height: itemBorder
                        color: colorContainerBorder
                    }
                }
            }
        }
    }

    DWMThumbnail {
        id: dwmThumbnail
        objectName: modelData.objectName

        anchors.fill: parent
        hwnd: windowHwnd

        property var windowInfo: hasModel ? modelData.windowInfo : null
        property var windowHwnd: windowInfo ? windowInfo.hwnd : HWND_NULL

        onWindowHwndChanged: {
            print(modelData.objectName, "hwnd changed", windowHwnd, "valid?", windowHwnd !== HWND_NULL)
        }
    }
}
