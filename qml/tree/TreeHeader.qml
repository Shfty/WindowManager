import QtQuick 2.11
import QtQuick.Window 2.11
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0

import ".."

Item {
    id: nestedHeader
    
    property var treeItem: null

    property var hwnd: treeItem.hwnd
    onHwndChanged: {
        hwndValid = treeItem.isHwndValid()
    }

    property bool hwndValid: false
    property bool isRoot: treeItem.depth == 1

    Rectangle {
        width: parent.width
        height: Options.headerSize

        color: {
            if(treeItem.parent.layout === "Split") {
                return Options.activeHeaderColor
            }

            if(treeItem.parent.activeIndex === treeItem.index) {
                return Options.activeHeaderColor
            }

            return Options.inactiveHeaderColor
        }
    }
    
    TextField {
        id: titleTextField

        width: 200
        height: Options.headerSize

        background: Rectangle {
            color: "#50000000"
            border.width: 2
            border.color: "#65FFFFFF"
        }

        color: "white"

        text: treeItem ? treeItem.title : ""
        placeholderText: qsTr("Title")

        onEditingFinished: {
            treeItem.title = titleTextField.text
        }
    }

    ComboBox {
        id: windowComboBox

        anchors.left: titleTextField.right
        anchors.right: buttonLayout.left
        height: Options.headerSize

        property var windowList: windowManager.windowList
        property string containerString: "[" + treeItem.flow + " " + treeItem.layout + " Container]"

        model: {
            var stringModel = [ containerString ];
            stringModel.push.apply(stringModel, windowList.titles)
            return stringModel
        }

        currentIndex: {
            return windowList.hwnds.indexOf(treeItem.hwnd) + 1
        }

        onActivated: function(selectedIndex) {
            if(selectedIndex == 0)
            {
                treeItem.resetHwnd()
            }
            else
            {
                treeItem.hwnd = windowList.hwnds[currentIndex - 1]
            }
        }

        Component.onCompleted: {
            windowComboBox.popup.parent = popupWrapper
            windowComboBox.popup.topMargin = 0
            windowComboBox.popup.bottomMargin = 0
        }

        Window {
            property var rootTransform: {
                // Dependencies
                windowComboBox.popup.opened

                return mapToGlobal(windowComboBox.x, windowComboBox.y + windowComboBox.height)
            }

            x: rootTransform.x
            y: rootTransform.y
            width: windowComboBox.popup.width
            height: windowComboBox.popup.height
            flags: Qt.FramelessWindowHint | Qt.WA_TranslucentBackground | Qt.WindowStaysOnTopHint | Qt.Dialog
            color: "#00000000"
            visible: windowComboBox.popup.visible

            onActiveChanged: {
                if(!active)
                {
                    windowComboBox.popup.close()
                }
            }

            Item {
                id: popupWrapper
                anchors.fill: parent
            }
        }
    }

    RowLayout {
        id: buttonLayout

        anchors.right: parent.right
        height: Options.headerSize

        spacing: 0

        Button {
            id: addButton
            objectName: "addButton"

            ToolTip.visible: hovered
            ToolTip.delay: 500
            ToolTip.text: qsTr("Add Child")

            width: Options.headerSize
            Layout.fillHeight: true

            font.family: "Segoe MDL2 Assets"
            text: "\uE710"

            visible: !hwndValid

            onClicked: {
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
            id: flipButton
            objectName: "flipButton"

            ToolTip.visible: hovered
            ToolTip.delay: 500
            ToolTip.text: qsTr("Flip Orientation")

            width: Options.headerSize
            Layout.fillHeight: true

            font.family: "Segoe MDL2 Assets"
            text: "\uE8EB"

            visible: !hwndValid

            onClicked: {
                if(!pressed) {
                    treeItem.toggleFlow()
                }
            }
        }

        Button {
            id: layoutButton
            objectName: "layoutButton"

            ToolTip.visible: hovered
            ToolTip.delay: 500
            ToolTip.text: qsTr("Switch Layout")

            width: Options.headerSize
            Layout.fillHeight: true

            font.family: "Segoe MDL2 Assets"
            text: "\uE7FD"

            visible: !hwndValid

            onClicked: {
                if(!pressed) {
                    treeItem.toggleLayout()
                }
            }
        }

        Button {
            id: launchButton
            objectName: "launchButton"

            ToolTip.visible: hovered
            ToolTip.delay: 500
            ToolTip.text: qsTr("Launch")

            Layout.fillHeight: true

            font.family: "Segoe MDL2 Assets"
            text: "\uE713"

            visible: treeItem.hwnd ? true : false

            property string itemLaunchProgram: "C:/Program Files/Mozilla Firefox/firefox.exe"
            property string itemLaunchParams: "-new-window https://www.google.com"

            onClicked: {
                var paramArray = itemLaunchParams.split(" ")
                rootObject.process.start(itemLaunchProgram, paramArray)
            }
        }

        Button {
            id: moveLeftButton
            objectName: "moveLeftButton"

            ToolTip.visible: hovered
            ToolTip.delay: 500
            ToolTip.text: qsTr("Move Left")

            width: Options.headerSize
            Layout.fillHeight: true

            font.family: "Segoe MDL2 Assets"
            text: {
                if(treeItem.parent)
                {
                    return treeItem.parent.flow === "Horizontal" ? "\uE76B" : "\uE70E"
                }

                return "\uE76B"
            }

            visible: !isRoot
            enabled: treeItem.parent.children.length > 0 && treeItem.index > 0

            onClicked: {
                treeItem.moveUp()
            }
        }

        Button {
            id: moveRightButton
            objectName: "moveRightButton"

            ToolTip.visible: hovered
            ToolTip.delay: 500
            ToolTip.text: qsTr("Move Right")

            width: Options.headerSize
            Layout.fillHeight: true

            font.family: "Segoe MDL2 Assets"
            text: {
                if(treeItem.parent)
                {
                    return treeItem.parent.flow === "Horizontal" ? "\uE76C" : "\uE70D"
                }

                return "\uE76C"
            }

            visible: !isRoot
            enabled: treeItem.parent.children.length > 0 && treeItem.index < treeItem.parent.children.length - 1

            onClicked: {
                treeItem.moveDown()
            }
        }

        Button {
            id: swapButton
            objectName: "swapButton"

            ToolTip.visible: hovered
            ToolTip.delay: 500
            ToolTip.text: qsTr("Swap")

            width: Options.headerSize
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
                property string parentTitle: treeItem.title

                property bool hasParentBounds: treeItem.bounds ? true : false
                property rect parentBounds: hasParentBounds ? treeItem.bounds : Qt.rect(-1, -1, -1, -1)

                property rect cachedItemBounds
                property rect cachedParentBounds

                Repeater {
                    id: swapRepeater
                    model: nestedModel
                    MenuItem {
                        text: treeItem.title
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

            width: Options.headerSize
            Layout.fillHeight: true

            font.family: "Segoe MDL2 Assets"
            text: "\uE8BB"

            visible: !isRoot

            onClicked: {
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
                var pos = mapToGlobal(0, Options.headerSize)
                rootObject.showTrayIconWindow(Qt.point(pos.x, pos.y))
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
                rootObject.showConfigWindow()
            }
        }

        Button {
            id: quitButton
            objectName: "quitButton"

            ToolTip.visible: hovered
            ToolTip.delay: 500
            ToolTip.text: qsTr("Quit")

            Layout.fillHeight: true

            font.family: "Segoe MDL2 Assets"
            text: "\uE8BB"

            visible: isRoot

            onClicked: {
                Qt.quit()
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

        visible: !isRoot && treeItem.parent.layout !== "Split" && treeItem.index != treeItem.parent.activeIndex

        onClicked: {
            treeItem.setActive();
        }
    }
}