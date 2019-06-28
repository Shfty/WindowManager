import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.5
import QtQuick.Window 2.12
import QtQuick.Controls.Universal 2.3

import ".."

Item {
    id: treeHeader

    Component.onDestruction: {
        print("Header Delegate Destruction")
    }

    property var modelData: null
    property bool hasModel: modelData ? true : false

    x: headerBounds.x
    y: headerBounds.y
    width: headerBounds.width
    height: headerBounds.height

    property rect headerBounds: hasModel ? modelData.headerBounds : Qt.rect(0, 0, 0, 0)
    Behavior on headerBounds {
        enabled: hasModel
        PropertyAnimation {
            easing.type: appCore.settingsContainer.itemAnimationCurve
            duration: appCore.settingsContainer.itemAnimationDuration
        }
    }

    property var windowInfo: modelData.windowInfo
    property var hwnd: windowInfo !== null ? windowInfo.hwnd : null

    property bool hwndValid: hwnd !== null
    property bool isRoot: hasModel ? modelData.depth === 0 : false
    property bool hasLaunchProperties: {
        return  modelData.launchUri !== "" ||
                modelData.launchParams !== "" ||
                modelData.autoGrabTitle !== "" ||
                modelData.autoGrabClass !== ""
    }

    Rectangle {
        width: parent.width
        height: appCore.settingsContainer.headerSize

        color: {
            if(!modelData) return appCore.settingsContainer.colorActiveHeader

            if(isRoot) return appCore.settingsContainer.colorActiveHeader

            if(modelData.treeParent.layout === "Split") {
                return appCore.settingsContainer.colorActiveHeader
            }

            if(modelData.treeParent.activeIndex === modelData.index) {
                return appCore.settingsContainer.colorActiveHeader
            }

            return appCore.settingsContainer.colorInactiveHeader
        }
    }

    Button {
        id: titleButton

        anchors.top: parent.top
        anchors.left: parent.left

        width: 100
        height: appCore.settingsContainer.headerSize

        text: modelData.objectName !== "" ? modelData.objectName : ""

        onClicked: {
            var size = Qt.size(600, 440)
            var pos = titleButton.mapToGlobal(0, titleButton.height)
            appCore.ipcClient.sendMessage(["MoveOverlay", pos, size]);
            appCore.ipcClient.sendMessage(["ShowOverlay", "ItemSettings"]);
        }
    }

    Button {
        id: windowSelectButton

        anchors.left: titleButton.right
        anchors.right: buttonLayout.left
        height: appCore.settingsContainer.headerSize

        enabled: modelData.children.length === 0
        text: modelData.windowInfo !== null ? modelData.windowInfo.winTitle : "[Container]"

        onClicked: {
            var size = Qt.size(windowSelectButton.width, 500)
            var pos = windowSelectButton.mapToGlobal(0, windowSelectButton.height)
            appCore.setPendingWindowRecipient(modelData)
            appCore.ipcClient.sendMessage(["SetPendingWindowInfoSocket"]);
            appCore.ipcClient.sendMessage(["MoveOverlay", pos, size]);
            appCore.ipcClient.sendMessage(["ShowOverlay", "WindowList"]);
        }

        contentItem: Label {
            anchors.left: parent.left
            anchors.leftMargin: iconImage.width + (windowSelectButton.padding * 2)
            anchors.right: parent.right

            elide: Text.ElideRight
            horizontalAlignment: Text.AlignHLeft
            verticalAlignment: Text.AlignVCenter

            text: parent.text
        }

        AppIcon {
            id: iconImage

            z: 1

            anchors.left: parent.left
            anchors.leftMargin: parent.padding
            anchors.verticalCenter: parent.verticalCenter

            sourceSize: Qt.size(parent.height - parent.padding, parent.height - parent.padding)

            model: treeHeader.modelData
        }
    }

    RowLayout {
        id: buttonLayout

        anchors.right: rootItemIncubator.left
        height: appCore.settingsContainer.headerSize

        spacing: 0

        Button {
            id: flipButton
            objectName: "flipButton"

            Layout.minimumWidth: appCore.settingsContainer.headerSize
            Layout.fillHeight: true

            ToolTip.visible: hovered
            ToolTip.delay: 500
            ToolTip.text: qsTr(modelData.flow + " Flow")

            Label {
                anchors.centerIn: parent
                font.family: "Segoe MDL2 Assets"
                text: "\uE174"
                rotation: hasModel ? (modelData.flow === "Horizontal" ? -90 : 0) : 0

                Behavior on rotation {
                    PropertyAnimation {
                        duration: appCore.settingsContainer.itemAnimationDuration
                        easing.type: appCore.settingsContainer.itemAnimationCurve
                    }
                }
            }

            visible: !hwndValid && !hasLaunchProperties

            onClicked: {
                if(!modelData) return

                if(!pressed) {
                    modelData.toggleFlow()
                    modelData.updateWindowPosition()
                }
            }
        }

        Button {
            id: layoutButton
            objectName: "layoutButton"

            Layout.minimumWidth: appCore.settingsContainer.headerSize
            Layout.fillHeight: true

            ToolTip.visible: hovered
            ToolTip.delay: 500
            ToolTip.text: qsTr(modelData.layout + " Layout")

            RowLayout {
                anchors.fill: parent
                anchors.margins: layoutButton.padding

                spacing: hasModel ? (modelData.layout === "Split" ? 2 : 0) : 0
                Behavior on spacing {
                    NumberAnimation {
                        duration: appCore.settingsContainer.itemAnimationDuration
                        easing.type: appCore.settingsContainer.itemAnimationCurve
                    }
                }

                rotation: hasModel ? (modelData.flow === "Horizontal" ? 0 : 90) : 0
                Behavior on rotation {
                    PropertyAnimation {
                        duration: appCore.settingsContainer.itemAnimationDuration
                        easing.type: appCore.settingsContainer.itemAnimationCurve
                    }
                }

                Repeater {
                    id: layoutRepeater
                    model: 3
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        color: "white"
                    }
                }
            }

            visible: !hwndValid && !hasLaunchProperties

            onClicked: {
                if(!modelData) return

                if(!pressed) {
                    modelData.toggleLayout()
                    modelData.updateWindowPosition()
                }
            }
        }

        Button {
            id: addButton
            objectName: "addButton"

            ToolTip.visible: hovered
            ToolTip.delay: 500
            ToolTip.text: qsTr("Add Child")

            Layout.maximumWidth: appCore.settingsContainer.headerSize
            Layout.fillHeight: true

            font.family: "Segoe MDL2 Assets"
            text: "\uE710"

            visible: !hwndValid && !hasLaunchProperties

            onClicked: {
                if(!modelData) return

                if(!pressed) {
                    modelData.addChild(
                        "",
                        modelData.flow === "Horizontal" ? "Vertical" : "Horizontal",
                        "Split"
                    )

                    modelData.updateWindowPosition()
                }
            }
        }

        Button {
            id: launchButton
            objectName: "launchButton"

            ToolTip.visible: hovered
            ToolTip.delay: 500
            ToolTip.text: qsTr("Launch")

            Layout.maximumWidth: appCore.settingsContainer.headerSize
            Layout.fillHeight: true

            font.family: "Segoe MDL2 Assets"
            text: "\uE768"

            enabled: !hwndValid

            onClicked: {
                if(!modelData) return

                modelData.launch()
            }
        }

        Button {
            id: closeButton
            objectName: "closeButton"

            ToolTip.visible: hovered
            ToolTip.delay: 500
            ToolTip.text: qsTr("Close")

            Layout.maximumWidth: appCore.settingsContainer.headerSize
            Layout.fillHeight: true

            font.family: "Segoe MDL2 Assets"
            text: "\uE8BB"

            visible: !isRoot

            onClicked: {
                appCore.ipcClient.sendMessage(["CloseWindow", windowInfo.hwnd]);

                /*
                if(!modelData) return

                modelData.remove()
                modelData.treeParent.updateWindowPosition()
                */
            }
        }
    }

    Incubator {
        id: rootItemIncubator

        active: isRoot
        anchors.top: parent.top
        anchors.right: parent.right
        width: itemInstance !== null ? itemInstance.width : 0
        height: hasSettingsContainer ? settingsContainer.headerSize : 0
        sourceComponent: Component {
            RowLayout {
                id: rootLayout
                spacing: 0

                Component.onDestruction: {
                    print("Header Incubator Item Destruction", modelData.objectName)
                }

                Button {
                    id: trayIconWidget
                    objectName: "trayIconWidget"

                    ToolTip.visible: hovered
                    ToolTip.delay: 500
                    ToolTip.text: qsTr("Tray")

                    Layout.maximumWidth: appCore.settingsContainer.headerSize
                    Layout.fillHeight: true

                    font.family: "Segoe MDL2 Assets"
                    text: "\uE8A5"

                    onClicked: {
                        var pos = mapToGlobal(0, appCore.settingsContainer.headerSize)
                        appCore.ipcClient.sendMessage(["ToggleTray", pos]);
                    }
                }

                Timer {
                    interval: 100
                    running: true
                    repeat: true
                    onTriggered: {
                        var now = new Date()
                        dateWidget.text = now.toLocaleDateString(Qt.locale(), "dddd d MMMM yyyy")
                        timeWidget.text = now.toLocaleTimeString()
                    }
                }

                Button {
                    id: dateWidget
                    objectName: "dateWidget"

                    ToolTip.visible: hovered
                    ToolTip.delay: 500
                    ToolTip.text: qsTr("Date")

                    Layout.fillHeight: true

                    font.family: "Segoe MDL2 Assets"
                }

                Button {
                    id: timeWidget
                    objectName: "timeWidget"

                    ToolTip.visible: hovered
                    ToolTip.delay: 500
                    ToolTip.text: qsTr("Time")

                    Layout.fillHeight: true

                    font.family: "Segoe MDL2 Assets"
                }

                RowLayout {
                    Layout.fillHeight: true

                    Spinner {
                        Layout.fillHeight: true
                        size: appCore.settingsContainer.headerSize
                        visible: false
                    }

                    Item {
                        Layout.minimumWidth: 80
                        Layout.fillHeight: true

                        FramerateCounter {
                            anchors.centerIn: parent
                        }
                    }
                }

                Button {
                    id: monitorConfigButton
                    objectName: "monitorConfigButton"

                    ToolTip.visible: hovered
                    ToolTip.delay: 500
                    ToolTip.text: qsTr("Configuration")

                    Layout.fillHeight: true

                    font.family: "Segoe MDL2 Assets"
                    text: "\uE713"

                    onClicked: {
                        var size = Qt.size(Screen.width * 0.5, Screen.height * 0.5)
                        var pos = Qt.point(Screen.virtualX + size.width * 0.5, Screen.virtualY + size.height * 0.5)
                        appCore.ipcClient.sendMessage(["MoveOverlay", pos, size]);
                        appCore.ipcClient.sendMessage(["ShowOverlay", "Config"]);
                    }
                }

                Button {
                    id: powerButton
                    objectName: "powerButton"

                    ToolTip.visible: hovered
                    ToolTip.delay: 500
                    ToolTip.text: qsTr("Quit")

                    Layout.fillHeight: true

                    font.family: "Segoe MDL2 Assets"
                    text: "\uE7E8"

                    onClicked: {
                        var size = Qt.size(160, 310)
                        var pos = powerButton.mapToGlobal(powerButton.width - size.width, powerButton.height)
                        appCore.ipcClient.sendMessage(["MoveOverlay", pos, size]);
                        appCore.ipcClient.sendMessage(["ShowOverlay", "PowerMenu"]);
                    }
                }
            }
        }
    }

    Button {
        id: activateButton
        objectName: "activateButton"

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: appCore.settingsContainer.headerSize

        ToolTip.visible: hovered
        ToolTip.delay: 500
        ToolTip.text: qsTr("Activate")

        font.family: "Segoe MDL2 Assets"

        visible: !isRoot && modelData.treeParent.layout !== "Split" && modelData.index !== modelData.treeParent.activeIndex

        onClicked: {
            if(!modelData) return

            modelData.setActive();
            modelData.treeParent.updateWindowPosition()
        }
    }

    MouseArea {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: appCore.settingsContainer.headerSize

        onWheel: function(event) {
            if(modelData.treeParent)
            {
                var delta = -Math.sign(event.angleDelta.y)
                if(modelData.treeParent.layout === "Tabbed")
                {
                    if(delta < 0 && modelData.treeParent.activeIndex > 0)
                    {
                        modelData.treeParent.scrollActiveIndex(delta)
                    }

                    if(delta > 0 && modelData.treeParent.activeIndex < modelData.treeParent.children.length - 1)
                    {
                        modelData.treeParent.scrollActiveIndex(delta)
                    }
                }
                else
                {
                    if(delta > 0)
                    {
                        modelData.moveDown()
                    }
                    else if(delta < 0)
                    {
                        modelData.moveUp()
                    }
                }

                modelData.treeParent.updateWindowPosition()
            }
        }
        onPressed: mouse.accepted = false
    }
}
