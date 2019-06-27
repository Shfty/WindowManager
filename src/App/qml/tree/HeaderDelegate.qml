import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.5
import QtQuick.Window 2.12
import QtQuick.Controls.Universal 2.3

import ".."

Item {
    id: treeHeader

    property var model: null
    property bool hasModel: model ? true : false

    x: headerBounds.x
    y: headerBounds.y
    width: headerBounds.width
    height: headerBounds.height

    property rect headerBounds: hasModel ? model.headerBounds : Qt.rect(0, 0, 0, 0)
    Behavior on headerBounds {
        enabled: hasModel
        PropertyAnimation {
            easing.type: appCore.settingsContainer.itemAnimationCurve
            duration: appCore.settingsContainer.itemAnimationDuration
        }
    }

    property var windowInfo: model.windowInfo
    property var hwnd: windowInfo !== null ? windowInfo.hwnd : null

    property bool hwndValid: hwnd !== null
    property bool isRoot: hasModel ? model.depth === 0 : false
    property bool hasLaunchProperties: {
        return  model.launchUri !== "" ||
                model.launchParams !== "" ||
                model.autoGrabTitle !== "" ||
                model.autoGrabClass !== ""
    }

    Rectangle {
        width: parent.width
        height: appCore.settingsContainer.headerSize

        color: {
            if(!model) return appCore.settingsContainer.colorActiveHeader

            if(isRoot) return appCore.settingsContainer.colorActiveHeader

            if(model.treeParent.layout === "Split") {
                return appCore.settingsContainer.colorActiveHeader
            }

            if(model.treeParent.activeIndex === model.index) {
                return appCore.settingsContainer.colorActiveHeader
            }

            return appCore.settingsContainer.colorInactiveHeader
        }
    }

    Button {
        id: titleButton

        width: 100
        height: appCore.settingsContainer.headerSize

        text: model.objectName !== "" ? model.objectName : ""

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

        enabled: model.children.length === 0
        text: model.windowInfo !== null ? model.windowInfo.winTitle : "[Container]"

        onClicked: {
            var size = Qt.size(windowSelectButton.width, 500)
            var pos = windowSelectButton.mapToGlobal(0, windowSelectButton.height)
            appCore.setPendingWindowRecipient(model)
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

            model: treeHeader.model
        }
    }

    RowLayout {
        id: buttonLayout

        anchors.right: parent.right
        height: appCore.settingsContainer.headerSize

        spacing: 0

        Button {
            id: flipButton
            objectName: "flipButton"

            Layout.minimumWidth: appCore.settingsContainer.headerSize
            Layout.fillHeight: true

            ToolTip.visible: hovered
            ToolTip.delay: 500
            ToolTip.text: qsTr(model.flow + " Flow")

            Label {
                anchors.centerIn: parent
                font.family: "Segoe MDL2 Assets"
                text: "\uE174"
                rotation: hasModel ? (model.flow === "Horizontal" ? -90 : 0) : 0

                Behavior on rotation {
                    PropertyAnimation {
                        duration: appCore.settingsContainer.itemAnimationDuration
                        easing.type: appCore.settingsContainer.itemAnimationCurve
                    }
                }
            }

            visible: !hwndValid && !hasLaunchProperties

            onClicked: {
                if(!model) return

                if(!pressed) {
                    model.toggleFlow()
                    model.updateWindowPosition()
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
            ToolTip.text: qsTr(model.layout + " Layout")

            RowLayout {
                anchors.fill: parent
                anchors.margins: layoutButton.padding

                spacing: hasModel ? (model.layout === "Split" ? 2 : 0) : 0
                Behavior on spacing {
                    NumberAnimation {
                        duration: appCore.settingsContainer.itemAnimationDuration
                        easing.type: appCore.settingsContainer.itemAnimationCurve
                    }
                }

                rotation: hasModel ? (model.flow === "Horizontal" ? 0 : 90) : 0
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
                if(!model) return

                if(!pressed) {
                    model.toggleLayout()
                    model.updateWindowPosition()
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
                if(!model) return

                if(!pressed) {
                    model.addChild(
                        "",
                        model.flow === "Horizontal" ? "Vertical" : "Horizontal",
                        "Split"
                    )

                    model.updateWindowPosition()
                }
            }
        }

        /*
        Button {
            id: moveLeftButton
            objectName: "moveLeftButton"

            ToolTip.visible: hovered
            ToolTip.delay: 500
            ToolTip.text: qsTr("Move Left")

            Layout.maximumWidth: appCore.settingsContainer.headerSize
            Layout.fillHeight: true

            font.family: "Segoe MDL2 Assets"
            text: {
                if(!model) return "\uE76B"

                if(model.treeParent)
                {
                    return model.treeParent.flow === "Horizontal" ? "\uE76B" : "\uE70E"
                }

                return "\uE76B"
            }

            visible: !isRoot
            enabled: {
                if(!model) return false
                if(!model.treeParent) return false

                return model.treeParent.children.length > 0 && model.index > 0
            }

            onClicked: {
                if(!model) return

                model.moveUp()
                model.treeParent.updateWindowPosition()
            }
        }

        Button {
            id: moveRightButton
            objectName: "moveRightButton"

            ToolTip.visible: hovered
            ToolTip.delay: 500
            ToolTip.text: qsTr("Move Right")

            Layout.maximumWidth: appCore.settingsContainer.headerSize
            Layout.fillHeight: true

            font.family: "Segoe MDL2 Assets"
            text: {
                if(model.treeParent)
                {
                    return model.treeParent.flow === "Horizontal" ? "\uE76C" : "\uE70D"
                }

                return "\uE76C"
            }

            visible: !isRoot
            enabled: {
                if(!model) return false
                if(!model.treeParent) return false

                return model.treeParent.children.length > 0 && model.index < model.treeParent.children.length - 1
            }

            onClicked: {
                if(!model) return

                model.moveDown()
                model.treeParent.updateWindowPosition()
            }
        }
        */

        Button {
            id: swapButton
            objectName: "swapButton"

            ToolTip.visible: hovered
            ToolTip.delay: 500
            ToolTip.text: qsTr("Swap")

            Layout.maximumWidth: appCore.settingsContainer.headerSize
            Layout.fillHeight: true

            font.family: "Segoe MDL2 Assets"
            text: "\uE895"
            visible: isRoot

            onClicked: {
                swapMenu.visible = !swapMenu.visible
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
                if(!model) return

                model.launch()
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
                if(!model) return

                model.remove()
                model.treeParent.updateWindowPosition()
            }
        }

        /*
        Window {
            id: swapWindow
            property var rootTransform: {
                // Dependencies
                swapMenu.opened

                return mapToGlobal(windowComboBox.x + windowComboBox.width, windowComboBox.y + windowComboBox.height)
            }
            x: rootTransform.x
            y: rootTransform.y
            width: buttonLayout.width
            height: 50 * swapRepeater.count

            flags: Qt.FramelessWindowHint | Qt.WA_TranslucentBackground | Qt.WindowStaysOnTopHint | Qt.Dialog
            color: "#00000000"
            visible: swapMenu.visible

            Menu {
                id: swapMenu

                width: parent.width

                property int parentIndex: model.index
                property string parentTitle: model.objectName

                property bool hasParentBounds: model.bounds ? true : false
                property rect parentBounds: hasParentBounds ? model.bounds : Qt.rect(-1, -1, -1, -1)

                property rect cachedItemBounds
                property rect cachedParentBounds

                Repeater {
                    id: swapRepeater
                    model: nestedModel
                    MenuItem {
                        text: model.objectName
                        enabled: model.index != swapMenu.parentIndex
                        width: parent.width
                        onTriggered: {
                            var fromIndex = nestedModel.index(index, 3)
                            var toIndex = nestedModel.index(swapMenu.parentIndex, 3)

                            swapMenu.cachedItemBounds = model.bounds
                            swapMenu.cachedParentBounds = swapMenu.parentBounds

                            nestedModel.setData(fromIndex, swapMenu.cachedParentBounds)
                            nestedModel.setData(toIndex, swapMenu.cachedItemBounds)
                        }
                    }
                }
            }
        }
        */

        Loader {
            active: isRoot
            Layout.fillHeight: true

            sourceComponent: RowLayout {
                anchors.fill: parent
                spacing: 0

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

        visible: !isRoot && model.treeParent.layout !== "Split" && model.index !== model.treeParent.activeIndex

        onClicked: {
            if(!model) return

            model.setActive();
            model.treeParent.updateWindowPosition()
        }
    }

    MouseArea {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: appCore.settingsContainer.headerSize

        onWheel: function(event) {
            var delta = -Math.sign(event.angleDelta.y)

            if(model.treeParent.layout === "Tabbed")
            {
                if(delta < 0 && model.treeParent.activeIndex > 0)
                {
                    model.treeParent.scrollActiveIndex(delta)
                }

                if(delta > 0 && model.treeParent.activeIndex < model.treeParent.children.length - 1)
                {
                    model.treeParent.scrollActiveIndex(delta)
                }
            }
            else
            {
                if(delta > 0)
                {
                    model.moveDown()
                }
                else if(delta < 0)
                {
                    model.moveUp()
                }
            }

            model.treeParent.updateWindowPosition()
        }
        onPressed: mouse.accepted = false
    }
}
