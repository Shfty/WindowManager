import QtQuick 2.11
import QtQuick.Window 2.11
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Universal 2.3
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
    property bool isRoot: treeItem ? treeItem.depth <= 1 : false

    function attachToOverlay(item, pos, size) {
        overlayWindow.x = pos.x
        overlayWindow.y = pos.y
        overlayWindow.width = size.width
        overlayWindow.height = size.height
        item.parent = overlayWindow.contentItem
    }

    function detachFromOverlay(item) {
        overlayWindow.x = 0
        overlayWindow.y = 0
        overlayWindow.width = 0
        overlayWindow.height = 0
        item.parent = nestedHeader
    }

    Rectangle {
        width: parent.width
        height: settings.headerSize

        color: {
            if(!treeItem) return settings.colorActiveHeader

            if(isRoot) return settings.colorActiveHeader

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
            windowComboBox.popup.topMargin = 0
            windowComboBox.popup.bottomMargin = 0
            windowComboBox.popup.y = settings.headerSize
        }

        Connections {
            target: windowComboBox.popup
            onOpened: {
                var globalPos = windowComboBox.mapToGlobal(0, settings.headerSize)
                var size = Qt.size(windowComboBox.popup.width, windowComboBox.popup.height)
                nestedHeader.attachToOverlay(windowComboBox.popup, globalPos, size)
                treeItem.moveWindowOffscreen()
            }
            onClosed: {
                nestedHeader.detachFromOverlay(windowComboBox.popup)
                treeItem.moveWindowOnscreen()
            }
        }
    }

    Item {
        id: configWindow

        width: treeItem ? treeItem.contentBounds.width : 0
        height: treeItem ? treeItem.contentBounds.height : 0

        visible: false

        parent: visible ? parent : overlayWindow.contentItem

        function toggle() {
            this.visible = !this.visible
            if(this.visible) {
                var globalPos = nestedHeader.parent.mapToGlobal(treeItem.contentBounds.x, treeItem.contentBounds.y)
                var size = Qt.size(width, height)
                nestedHeader.attachToOverlay(configWindow, globalPos, size)
                treeItem.moveWindowOffscreen()
            }
            else {
                nestedHeader.detachFromOverlay(configWindow)
                treeItem.moveWindowOnscreen()
            }
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
                if(!treeItem.parent) return false

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
                if(!treeItem.parent) return false

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
                powerWindow.toggle()
            }
            

            Item {
                id: powerWindow

                width: 100
                height: 150

                visible: false

                function toggle() {
                    visible = !visible
                    if(visible) {
                        var globalPos = powerButton.mapToGlobal(powerButton.width - width, powerButton.height)
                        var size = Qt.size(powerWindow.width, powerWindow.height)
                        nestedHeader.attachToOverlay(powerWindow, globalPos, size)
                        treeItem.moveWindowOffscreen()
                    }
                    else {
                        nestedHeader.detachFromOverlay(powerWindow)
                        treeItem.moveWindowOnscreen()
                    }
                }

                Rectangle {
                    color: "white"
                    anchors.fill: parent
                }

                ColumnLayout {
                    id: powerMenu
                    spacing: 0

                    anchors.fill: parent

                    Button {
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        text: "Exit"
                        onClicked: {
                            Qt.quit();
                        }
                    }
                    Button {
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        text: "Shut Down"
                        onClicked: {
                            appCore.shutdown()
                        }
                    }
                    Button {
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        text: "Restart"
                        onClicked: {
                            appCore.restart()
                        }
                    }
                    Button {
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        text: "Sleep"
                        onClicked: {
                            appCore.sleep()
                        }
                    }
                }
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