import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.5
import QtQuick.Controls.Universal 2.3

Rectangle {
    id: overlayWindow
    objectName: "ItemSettingsOverlay"

    Universal.theme: Universal.Dark

    anchors.fill: parent

    color: "black"
    property bool isVisible: false

    property var windowActive: Window.active
    onWindowActiveChanged: {
        if(isVisible && !windowActive) hide()
    }

    property var targetItem: null

    function show(newTargetItem, x, y) {
        targetItem = newTargetItem

        Window.window.x = x
        Window.window.y = y
        Window.window.height = settingsMenu.contentHeight
        Window.window.requestActivate()

        isVisible = true
    }

    function hide() {
        var offscreen = appCore.windowView.getOffscreenArea()

        if(Window.window) {
            Window.window.x = offscreen.x
            Window.window.y = offscreen.y
        }

        isVisible = false
        targetItem = null
    }

    function toggle(newTargetItem, x, y, width) {
        if(isVisible && targetItem === newTargetItem) {
            hide()
        }
        else {
            show(newTargetItem, x, y)
        }
    }

    ListView {
        id: settingsMenu

        anchors.fill: parent

        model: ListModel {
            ListElement {
                name: "Title"
                category: "Node"
                target: "objectName"
                isEnabled: function() {
                    return true
                }
            }
            ListElement {
                name: "Borderless"
                category: "Node"
                target: "borderless"
                isEnabled: function() {
                    return true
                }
            }
            ListElement {
                name: "URI"
                category: "Launch"
                target: "launchUri"
                isEnabled: function() {
                    if(!targetItem) return false

                    return targetItem.children.length === 0
                }
            }
            ListElement {
                name: "Parameters"
                category: "Launch"
                target: "launchParams"
                isEnabled: function() {
                    if(!targetItem) return false

                    return targetItem.children.length === 0
                }
            }
            ListElement {
                name: "Auto Launch"
                category: "Launch"
                target: "autoLaunch"
                isEnabled: function() {
                    if(!targetItem) return false

                    return targetItem.children.length === 0
                }
            }
            ListElement {
                name: "Window Title"
                category: "Auto Grab"
                target: "autoGrabTitle"
                isEnabled: function() {
                    if(!targetItem) return false

                    return targetItem.children.length === 0
                }
            }
            ListElement {
                name: "Window Class"
                category: "Auto Grab"
                target: "autoGrabClass"
                isEnabled: function() {
                    if(!targetItem) return false

                    return targetItem.children.length === 0
                }
            }
        }

        delegate: ItemDelegate {
            width: parent.width

            text: name

            contentItem: Label {
                anchors.fill: parent

                Label {
                    id: nameLabel

                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.bottom: parent.bottom
                    width: 200

                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter

                    text: name
                }

                Loader {
                    active: {
                        if(!targetItem) return false
                        return typeof targetItem[target] === "boolean"
                    }

                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.left: nameLabel.right

                    sourceComponent: CheckBox {
                        id: nameField

                        enabled: isEnabled()

                        checked: targetItem ? targetItem[target] : false
                        onCheckedChanged: {
                            if(!targetItem) return
                            targetItem[target] = checked
                        }
                    }
                }

                Loader {
                    active: {
                        if(!targetItem) return false
                        return typeof targetItem[target] === "string"
                    }

                    anchors.top: parent.top
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    anchors.left: nameLabel.right

                    sourceComponent: TextField {
                        id: nameField

                        enabled: isEnabled()

                        text: targetItem ? targetItem[target] : ""
                        onEditingFinished: {
                            if(!targetItem) return
                            targetItem[target] = text
                        }
                    }
                }
            }
        }

        section.property: "category"
        section.criteria: ViewSection.FullString
        section.delegate: Label {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 10

            height: contentHeight + anchors.margins * 2

            verticalAlignment: Text.AlignVCenter

            font.bold: true

            text: section
        }
    }
}