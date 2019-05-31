import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.5
import QtQuick.Controls.Universal 2.3
import ".."

Item {
    id: nestedHeader

    Universal.theme: Universal.Dark

    property var model: null

    property var windowInfo: model.windowInfo
    property var hwnd: windowInfo !== null ? windowInfo.hwnd : null

    property bool hwndValid: hwnd !== null
    property bool isRoot: model ? model.depth <= 1 : false
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
            var pos = titleButton.mapToGlobal(0, titleButton.height)
            appCore.itemSettingsOverlay.toggle(model, pos.x, pos.y)
        }
    }

    Button {
        id: launchButton
        objectName: "launchButton"

        ToolTip.visible: hovered
        ToolTip.delay: 500
        ToolTip.text: qsTr("Launch")

        anchors.left: titleButton.right
        width: appCore.settingsContainer.headerSize
        height: appCore.settingsContainer.headerSize

        font.family: "Segoe MDL2 Assets"
        text: "\uE768"

        enabled: !hwndValid

        onClicked: {
            if(!model) return

            model.launch()
        }
    }

    Button {
        id: windowSelectButton

        anchors.left: launchButton.right
        anchors.right: buttonLayout.left
        height: appCore.settingsContainer.headerSize

        enabled: model.children.length === 0

        contentItem: Label {
            text: model.windowInfo !== null ? model.windowInfo.winTitle : "[Container]"
            elide: Text.ElideRight
            horizontalAlignment: Text.AlignHLeft
            verticalAlignment: Text.AlignVCenter
        }

        onClicked: {
            var pos = windowSelectButton.mapToGlobal(0, windowSelectButton.height)
            appCore.windowListOverlay.toggle(model, pos.x, pos.y, windowSelectButton.width)
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

            Layout.minimumWidth: 80
            Layout.fillHeight: true

            ToolTip.visible: hovered
            ToolTip.delay: 500
            ToolTip.text: qsTr("Flip Orientation")

            font.family: "Segoe MDL2 Assets"
            text: model ? model.flow : ""

            visible: !hwndValid && !hasLaunchProperties

            onClicked: {
                if(!model) return

                if(!pressed) {
                    model.toggleFlow()
                }
            }
        }

        Button {
            id: layoutButton
            objectName: "layoutButton"

            Layout.minimumWidth: 80
            Layout.fillHeight: true

            ToolTip.visible: hovered
            ToolTip.delay: 500
            ToolTip.text: qsTr("Switch Layout")

            font.family: "Segoe MDL2 Assets"
            text: model ? model.layout : ""

            visible: !hwndValid && !hasLaunchProperties

            onClicked: {
                if(!model) return

                if(!pressed) {
                    model.toggleLayout()
                }
            }
        }

        Button {
            id: addButton
            objectName: "addButton"

            ToolTip.visible: hovered
            ToolTip.delay: 500
            ToolTip.text: qsTr("Add Child")

            width: appCore.settingsContainer.headerSize
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
                }
            }
        }

        Button {
            id: moveLeftButton
            objectName: "moveLeftButton"

            ToolTip.visible: hovered
            ToolTip.delay: 500
            ToolTip.text: qsTr("Move Left")

            width: appCore.settingsContainer.headerSize
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
            }
        }

        Button {
            id: moveRightButton
            objectName: "moveRightButton"

            ToolTip.visible: hovered
            ToolTip.delay: 500
            ToolTip.text: qsTr("Move Right")

            width: appCore.settingsContainer.headerSize
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
            }
        }

        Button {
            id: swapButton
            objectName: "swapButton"

            ToolTip.visible: hovered
            ToolTip.delay: 500
            ToolTip.text: qsTr("Swap")

            width: appCore.settingsContainer.headerSize
            Layout.fillHeight: true

            font.family: "Segoe MDL2 Assets"
            text: "\uE895"
            visible: isRoot

            onClicked: {
                swapMenu.visible = !swapMenu.visible
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

        Button {
            id: closeButton
            objectName: "closeButton"

            ToolTip.visible: hovered
            ToolTip.delay: 500
            ToolTip.text: qsTr("Close")

            width: appCore.settingsContainer.headerSize
            Layout.fillHeight: true

            font.family: "Segoe MDL2 Assets"
            text: "\uE8BB"

            visible: !isRoot

            onClicked: {
                if(!model) return

                model.remove()
            }
        }

        Button {
            id: trayIconWidget
            objectName: "trayIconWidget"

            ToolTip.visible: hovered
            ToolTip.delay: 500
            ToolTip.text: qsTr("Tray")

            Layout.fillHeight: true

            font.family: "Segoe MDL2 Assets"
            text: "\uE8A5"

            visible: isRoot

            onClicked: {
                var pos = mapToGlobal(0, appCore.settingsContainer.headerSize)
                appCore.winShellController.showTrayIconWindow(Qt.point(pos.x, pos.y), Qt.size(300, 300))
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

            visible: isRoot

            Timer {
                interval: 500
                running: true
                repeat: true
                onTriggered: {
                    var now = new Date()
                    dateWidget.text = now.toLocaleDateString(Qt.locale(), "dddd d MMMM yyyy")
                }
            }
        }

        Button {
            id: timeWidget
            objectName: "timeWidget"

            ToolTip.visible: hovered
            ToolTip.delay: 500
            ToolTip.text: qsTr("Time")

            Layout.fillHeight: true

            font.family: "Segoe MDL2 Assets"

            visible: isRoot

            Timer {
                interval: 500
                running: true
                repeat: true
                onTriggered: {
                    var now = new Date()
                    timeWidget.text = now.toLocaleTimeString()
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

            visible: isRoot

            onClicked: {
                appCore.configWindow.show()
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

            visible: isRoot

            onClicked: {
                var pos = powerButton.mapToGlobal(powerButton.width - appCore.powerMenuOverlay.width, powerButton.height)
                appCore.powerMenuOverlay.toggle(pos.x, pos.y)
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
                model.treeParent.scrollActiveIndex(delta)
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
        }
        onPressed: mouse.accepted = false
    }
}