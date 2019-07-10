import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.5
import QtQuick.Window 2.12
import QtQuick.Controls.Universal 2.3

import ".."

Item {
    id: treeHeader

    property var settingsContainer: clientCore ? clientCore.settingsContainer : null

    property var modelData: null
    property bool hasModel: modelData ? true : false

    property var windowInfo: modelData ? modelData.windowInfo : null
    property var hwnd: windowInfo ? windowInfo.hwnd : null

    property bool hwndValid: hwnd ? true : false
    property bool isRoot: hasModel ? modelData.depth === 0 : false
    property bool hasLaunchProperties: {
        if(!modelData) return false

        return  modelData.launchUri !== "" ||
                modelData.launchParams !== "" ||
                modelData.autoGrabTitle !== "" ||
                modelData.autoGrabClass !== ""
    }

    Rectangle {
        width: parent.width
        height: parent.height

        color: {
            if(!clientCore) return "black"

            if(!settingsContainer) return "black"

            if(!modelData) return settingsContainer.colorActiveHeader

            if(isRoot) return settingsContainer.colorActiveHeader

            if(modelData.treeParent.layout === "Split") {
                return settingsContainer.colorActiveHeader
            }

            if(modelData.treeParent.activeIndex === modelData.index) {
                return clientCore.settingsContainer.colorActiveHeader
            }

            return clientCore.settingsContainer.colorInactiveHeader
        }
    }

    Button {
        id: titleButton

        anchors.top: parent.top
        anchors.left: parent.left

        width: 100
        height: parent.height

        text: modelData ? (modelData.objectName !== "" ? modelData.objectName : "") : ""

        onClicked: {
            var size = Qt.size(600, 440)
            var pos = titleButton.mapToGlobal(0, titleButton.height)
            clientCore.sendMessage(["MoveOverlay", pos, size]);
            clientCore.sendMessage(["ShowOverlay", "ItemSettings"]);
        }
    }

    Button {
        id: windowSelectButton

        anchors.left: titleButton.right
        anchors.right: buttonLayout.left
        height: parent.height

        enabled: modelData ? modelData.children.length === 0 : false
        text: modelData ? (modelData.windowInfo !== null ? modelData.windowInfo.winTitle : "[Container]") : ""

        onClicked: {
            var size = Qt.size(windowSelectButton.width, 500)
            var pos = windowSelectButton.mapToGlobal(0, windowSelectButton.height)
            clientCore.setPendingWindowRecipient(modelData)
            clientCore.sendMessage(["SetPendingWindowInfoSocket"]);
            clientCore.sendMessage(["MoveOverlay", pos, size]);
            clientCore.sendMessage(["ShowOverlay", "WindowList"]);
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
        height: parent.height

        spacing: 0

        Button {
            id: flipButton
            objectName: "flipButton"

            Layout.minimumWidth: parent.height
            Layout.fillHeight: true

            ToolTip.visible: hovered
            ToolTip.delay: 500
            ToolTip.text: modelData ? qsTr(modelData.flow + " Flow") : ""

            Label {
                anchors.centerIn: parent
                font.family: "Segoe MDL2 Assets"
                text: "\uE174"
                rotation: hasModel ? (modelData.flow === "Horizontal" ? -90 : 0) : 0

                Behavior on rotation {
                    PropertyAnimation {
                        duration: clientCore.settingsContainer.itemAnimationDuration
                        easing.type: clientCore.settingsContainer.itemAnimationCurve
                    }
                }
            }

            visible: !hwndValid && !hasLaunchProperties

            onClicked: {
                if(!modelData) return

                if(!pressed) {
                    modelData.toggleFlow()
                }
            }
        }

        Button {
            id: layoutButton
            objectName: "layoutButton"

            Layout.minimumWidth: parent.height
            Layout.fillHeight: true

            ToolTip.visible: hovered
            ToolTip.delay: 500
            ToolTip.text: modelData ? qsTr(modelData.layout + " Layout") : ""

            RowLayout {
                anchors.fill: parent
                anchors.margins: layoutButton.padding

                spacing: hasModel ? (modelData.layout === "Split" ? 2 : 0) : 0
                Behavior on spacing {
                    NumberAnimation {
                        duration: clientCore.settingsContainer.itemAnimationDuration
                        easing.type: clientCore.settingsContainer.itemAnimationCurve
                    }
                }

                rotation: hasModel ? (modelData.flow === "Horizontal" ? 0 : 90) : 0
                Behavior on rotation {
                    PropertyAnimation {
                        duration: clientCore.settingsContainer.itemAnimationDuration
                        easing.type: clientCore.settingsContainer.itemAnimationCurve
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
                }
            }
        }

        Button {
            id: addButton
            objectName: "addButton"

            ToolTip.visible: hovered
            ToolTip.delay: 500
            ToolTip.text: qsTr("Add Child")

            Layout.maximumWidth: parent.height
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
                }
            }
        }

        Button {
            id: launchButton
            objectName: "launchButton"

            ToolTip.visible: hovered
            ToolTip.delay: 500
            ToolTip.text: qsTr("Launch")

            Layout.maximumWidth: parent.height
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

            Layout.maximumWidth: parent.height
            Layout.fillHeight: true

            font.family: "Segoe MDL2 Assets"
            text: "\uE8BB"

            visible: !isRoot

            onClicked: {
                if(windowInfo !== null)
                {
                    clientCore.sendMessage(["CloseWindow", windowInfo.hwnd]);
                }
                else if(modelData)
                {
                    modelData.remove()
                }
            }
        }
    }

    Incubator {
        id: rootItemIncubator

        active: isRoot

        anchors.top: parent.top
        anchors.right: parent.right
        width: item ? item.width : 0
        height: parent.height

        sourceComponent: Component {
            RowLayout {
                id: rootLayout

                height: parent.height

                spacing: 0

                Button {
                    id: trayIconWidget
                    objectName: "trayIconWidget"

                    ToolTip.visible: hovered
                    ToolTip.delay: 500
                    ToolTip.text: qsTr("Tray")

                    Layout.maximumWidth: parent.height
                    Layout.fillHeight: true

                    font.family: "Segoe MDL2 Assets"
                    text: "\uE8A5"

                    onClicked: {
                        var pos = mapToGlobal(0, rootItemIncubator.height)
                        clientCore.sendMessage(["ToggleTray", pos]);
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
                        size: parent.height
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
                        clientCore.sendMessage(["MoveOverlay", pos, size]);
                        clientCore.sendMessage(["ShowOverlay", "Config"]);
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
                        clientCore.sendMessage(["MoveOverlay", pos, size]);
                        clientCore.sendMessage(["ShowOverlay", "PowerMenu"]);
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
        height: parent.height

        ToolTip.visible: hovered
        ToolTip.delay: 500
        ToolTip.text: qsTr("Activate")

        font.family: "Segoe MDL2 Assets"

        visible: {
            if(isRoot) return false
            if(!modelData) return false
            if(!modelData.treeParent) return false
            if(modelData.treeParent.layout === "Split") return false
            if(modelData.index === modelData.treeParent.activeIndex) return false
            return true
        }

        onClicked: {
            if(!modelData) return
            modelData.setActive();
        }
    }

    MouseArea {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: parent.height

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
            }
        }
        onPressed: mouse.accepted = false
    }
}
