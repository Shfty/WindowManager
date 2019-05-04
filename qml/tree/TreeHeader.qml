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

    property var hwnd: treeItem ? treeItem.hwnd : null
    onHwndChanged: {
        if(!treeItem) return

        hwndValid = treeItem.isHwndValid()
    }

    property bool hwndValid: false
    property bool isRoot: treeItem ? treeItem.depth == 1 : false

    Rectangle {
        width: parent.width
        height: settings.headerSize

        color: {
            if(!treeItem) return settings.colorActiveHeader

            if(treeItem.parent.layout === "Split") {
                return settings.colorActiveHeader
            }

            if(treeItem.parent.activeIndex === treeItem.index) {
                return settings.colorActiveHeader
            }

            return settings.colorInactiveHeader
        }
    }
    
    TextField {
        id: titleTextField

        width: 100
        height: settings.headerSize

        background: Rectangle {
            color: "#50000000"
            border.width: 2
            border.color: "#65FFFFFF"
        }

        color: "white"

        text: treeItem ? treeItem.objectName : ""
        placeholderText: qsTr("Title")

        onEditingFinished: {
            if(!treeItem) return

            treeItem.objectName = titleTextField.text
        }
    }

    ComboBox {
        id: windowComboBox

        anchors.left: titleTextField.right
        anchors.right: buttonLayout.left
        height: settings.headerSize

        property var windowList: windowManager.windowList

        model: {
            var strlist = []
            for(var i in windowManager.windowList)
            {
                var wi = windowManager.windowList[i]
                strlist.push(wi.winTitle);
            }
            return strlist;
        }

        currentIndex: {
            if(!treeItem) return 0

            for(var i in windowManager.windowList)
            {
                var wi = windowManager.windowList[i]
                if(wi.hwnd == treeItem.hwnd) return i
            }

            return -1
        }

        onActivated: function(selectedIndex) {
            if(!treeItem) return

            treeItem.hwnd = windowManager.windowList[selectedIndex].hwnd
        }

        Component.onCompleted: {
            windowComboBox.popup.parent = popupWrapper
            windowComboBox.popup.topMargin = 0
            windowComboBox.popup.bottomMargin = 0
        }

        OverlayWindow {
            property point rootTransform: {
                // Dependencies
                windowComboBox.popup.opened

                return mapToGlobal(windowComboBox.x, windowComboBox.y + windowComboBox.height)
            }

            x: rootTransform.x
            y: rootTransform.y
            width: windowComboBox.popup.width
            height: windowComboBox.popup.height

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

    OverlayWindow {
        id: configWindow

        property point rootTransform: {
            // Dependencies
            windowComboBox.popup.opened

            return mapToGlobal(nestedHeader.x, nestedHeader.y + nestedHeader.height)
        }

        width: treeItem ? treeItem.contentBounds.width : 0
        height: treeItem ? treeItem.contentBounds.height : 0

        function toggle() {
            this.visible = !this.visible

            var pos = mapToGlobal(nestedHeader.x, nestedHeader.y + nestedHeader.height)
            this.x = pos.x
            this.y = pos.y
        }

        Rectangle {
            anchors.fill: parent
            color: "#80000000"

            ColumnLayout {
                anchors.fill: parent

                RowLayout {
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    Button {
                        Layout.minimumWidth: 100
                        Layout.fillWidth: false
                        height: settings.headerSize

                        text: "Launch URI"
                    }
                    TextField {
                        id: launchUriTextField

                        Layout.fillWidth: true
                        height: settings.headerSize

                        background: Rectangle {
                            color: "#50000000"
                            border.width: 2
                            border.color: "#65FFFFFF"
                        }

                        color: "white"

                        text: treeItem ? treeItem.launchUri : ""
                        placeholderText: qsTr("Launch URI")

                        onEditingFinished: {
                            if(!treeItem) return

                            treeItem.launchUri = launchUriTextField.text
                        }
                    }
                }

                RowLayout {
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    Button {
                        Layout.minimumWidth: 100
                        Layout.fillWidth: false
                        height: settings.headerSize

                        text: "Launch Params"
                    }
                    TextField {
                        id: launchParamsTextField

                        Layout.fillWidth: true
                        height: settings.headerSize

                        background: Rectangle {
                            color: "#50000000"
                            border.width: 2
                            border.color: "#65FFFFFF"
                        }

                        color: "white"

                        text: treeItem ? treeItem.launchParams : ""
                        placeholderText: qsTr("Launch Params")

                        onEditingFinished: {
                            if(!treeItem) return

                            treeItem.launchParams = launchParamsTextField.text
                        }
                    }
                }
            }
        }
    }

    RowLayout {
        id: buttonLayout

        anchors.right: parent.right
        height: settings.headerSize

        spacing: 0

        Button {
            id: flipButton
            objectName: "flipButton"

            width: settings.headerSize
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

            width: settings.headerSize
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

            width: settings.headerSize
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
            id: launchButton
            objectName: "launchButton"

            ToolTip.visible: hovered
            ToolTip.delay: 500
            ToolTip.text: qsTr("Launch")

            Layout.fillHeight: true

            font.family: "Segoe MDL2 Assets"
            text: "\uE768"

            onClicked: {
                if(!treeItem) return

                treeItem.launch()
            }
        }

        Button {
            id: configButton
            objectName: "configButton"

            ToolTip.visible: hovered
            ToolTip.delay: 500
            ToolTip.text: qsTr("Configure")

            Layout.fillHeight: true

            font.family: "Segoe MDL2 Assets"
            text: "\uE713"

            enabled: treeItem ? treeItem.children.length == 0 : false

            onClicked: {
                configWindow.toggle()
            }
        }

        Button {
            id: moveLeftButton
            objectName: "moveLeftButton"

            ToolTip.visible: hovered
            ToolTip.delay: 500
            ToolTip.text: qsTr("Move Left")

            width: settings.headerSize
            Layout.fillHeight: true

            font.family: "Segoe MDL2 Assets"
            text: {
                if(!treeItem) return "\uE76B"

                if(treeItem.parent)
                {
                    return treeItem.parent.flow === "Horizontal" ? "\uE76B" : "\uE70E"
                }

                return "\uE76B"
            }

            visible: !isRoot
            enabled: {
                if(!treeItem) return false

                return treeItem.parent.children.length > 0 && treeItem.index > 0
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

            width: settings.headerSize
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
            enabled: {
                if(!treeItem) return false

                return treeItem.parent.children.length > 0 && treeItem.index < treeItem.parent.children.length - 1
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

            width: settings.headerSize
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

            width: settings.headerSize
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
                var pos = mapToGlobal(0, settings.headerSize)
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
            if(!treeItem) return
            
            treeItem.setActive();
        }
    }
}