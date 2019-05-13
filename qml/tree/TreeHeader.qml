import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.5
import QtQuick.Controls.Universal 2.3
import ".."

Item {
    id: nestedHeader

    Universal.theme: Universal.Dark

    property var treeItem: null

    property var windowInfo: treeItem.windowInfo
    property var hwnd: windowInfo !== null ? windowInfo.hwnd : null

    property bool hwndValid: hwnd !== null
    property bool isRoot: treeItem ? treeItem.depth <= 1 : false

    Rectangle {
        width: parent.width
        height: appCore.settingsContainer.headerSize

        color: {
            if(!treeItem) return appCore.settingsContainer.colorActiveHeader

            if(isRoot) return appCore.settingsContainer.colorActiveHeader

            if(treeItem.treeParent.layout === "Split") {
                return appCore.settingsContainer.colorActiveHeader
            }

            if(treeItem.treeParent.activeIndex === treeItem.index) {
                return appCore.settingsContainer.colorActiveHeader
            }

            return appCore.settingsContainer.colorInactiveHeader
        }
    }

    Button {
        id: titleButton

        width: 100
        height: appCore.settingsContainer.headerSize

        text: treeItem.objectName !== "" ? treeItem.objectName : ""

        onClicked: {
            var pos = titleButton.mapToGlobal(0, titleButton.height)
            appCore.itemSettingsOverlay.toggle(treeItem, pos.x, pos.y)
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

        enabled: treeItem.launchUri !== "" || treeItem.children.length > 0

        onClicked: {
            if(!treeItem) return

            treeItem.launch()
        }
    }

    Button {
        id: windowSelectButton

        anchors.left: launchButton.right
        anchors.right: buttonLayout.left
        height: appCore.settingsContainer.headerSize

        enabled: treeItem.children.length === 0

        contentItem: Label {
            text: treeItem.windowInfo !== null ? treeItem.windowInfo.winTitle : "[Container]"
            elide: Text.ElideRight
            horizontalAlignment: Text.AlignHLeft
            verticalAlignment: Text.AlignVCenter
        }

        onClicked: {
            var pos = windowSelectButton.mapToGlobal(0, windowSelectButton.height)
            appCore.windowListOverlay.toggle(treeItem, pos.x, pos.y, windowSelectButton.width)
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
            text: treeItem ? treeItem.flow : ""

            visible: !hwndValid

            onClicked: {
                if(!treeItem) return

                if(!pressed) {
                    treeItem.toggleFlow()
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
            text: treeItem ? treeItem.layout : ""

            visible: !hwndValid

            onClicked: {
                if(!treeItem) return

                if(!pressed) {
                    treeItem.toggleLayout()
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

            visible: !hwndValid

            onClicked: {
                if(!treeItem) return

                if(!pressed) {
                    treeItem.addChild(
                        "",
                        treeItem.flow === "Horizontal" ? "Vertical" : "Horizontal",
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
                if(!treeItem) return "\uE76B"

                if(treeItem.treeParent)
                {
                    return treeItem.treeParent.flow === "Horizontal" ? "\uE76B" : "\uE70E"
                }

                return "\uE76B"
            }

            visible: !isRoot
            enabled: {
                if(!treeItem) return false
                if(!treeItem.treeParent) return false

                return treeItem.treeParent.children.length > 0 && treeItem.index > 0
            }

            onClicked: {
                if(!treeItem) return

                treeItem.moveUp()
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
                if(treeItem.treeParent)
                {
                    return treeItem.treeParent.flow === "Horizontal" ? "\uE76C" : "\uE70D"
                }

                return "\uE76C"
            }

            visible: !isRoot
            enabled: {
                if(!treeItem) return false
                if(!treeItem.treeParent) return false

                return treeItem.treeParent.children.length > 0 && treeItem.index < treeItem.treeParent.children.length - 1
            }

            onClicked: {
                if(!treeItem) return

                treeItem.moveDown()
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

                property int parentIndex: treeItem.index
                property string parentTitle: treeItem.objectName

                property bool hasParentBounds: treeItem.bounds ? true : false
                property rect parentBounds: hasParentBounds ? treeItem.bounds : Qt.rect(-1, -1, -1, -1)

                property rect cachedItemBounds
                property rect cachedParentBounds

                Repeater {
                    id: swapRepeater
                    model: nestedModel
                    MenuItem {
                        text: treeItem.objectName
                        enabled: treeItem.index != swapMenu.parentIndex
                        width: parent.width
                        onTriggered: {
                            var fromIndex = nestedModel.index(index, 3)
                            var toIndex = nestedModel.index(swapMenu.parentIndex, 3)

                            swapMenu.cachedItemBounds = treeItem.bounds
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
                if(!treeItem) return

                treeItem.remove()
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
                appCore.winShellController.showTrayIconWindow(Qt.point(pos.x, pos.y))
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

        anchors.fill: parent

        ToolTip.visible: hovered
        ToolTip.delay: 500
        ToolTip.text: qsTr("Activate")

        font.family: "Segoe MDL2 Assets"

        visible: !isRoot && treeItem.treeParent.layout !== "Split" && treeItem.index != treeItem.treeParent.activeIndex

        onClicked: {
            if(!treeItem) return

            treeItem.setActive();
        }
    }
}