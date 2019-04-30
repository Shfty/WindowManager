import QtQuick 2.11
import QtQuick.Window 2.11
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.4
import QtQuick.Controls.Universal 2.3

import ".."

Window {
    id: testWindow

    visible: true

    title: "Window Manager Test"

    width: 1280
    height: 720

    Universal.theme: Universal.Dark

    property var model: null

    // Root item
    Rectangle {
        id: rootItem
        anchors.fill: parent
        color: "black"

        Flickable {
            anchors.fill: parent
            contentHeight: {
                var h = 0
                for(var i = 0; i < rootRepeater.count; ++i) {
                    var item = rootRepeater.itemAt(i);
                    h += item.height
                }
                return h
            }

            Item {
                id: rootWrapper
                anchors.left: parent.left
                anchors.right: parent.right
                Repeater {
                    id: rootRepeater
                    model: testWindow.model
                    delegate: testDelegate
                }
            }
        }
    }

    // Item Delegate
    Component {
        id: testDelegate

        Item {
            id: wrapper

            anchors.left: parent ? parent.left : anchors.left
            anchors.right: parent ? parent.right : anchors.right

            anchors.top: {
                if(!parent) return anchors.top
                if(index == 0) return parent.top
                return parent.children[index - 1].bottom
            }
            height: totalHeight(submodelRepeater)

            property var treeItem: model.modelData

            property bool hasChildren: treeItem ? treeItem.children.length > 0 : false
            property bool hidden: false
            property bool childrenVisible: wrapper.hasChildren && !wrapper.hidden
            property var repeater: submodelRepeater

            function totalHeight(repeater) {
                var h = 32
                if(repeater.parentWrapper.childrenVisible) {
                    for(var i = 0; i < repeater.count; i++) {
                        h += totalHeight(repeater.itemAt(i).repeater)
                    }
                }
                return h
            }

            RowLayout {
                id: itemRow
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                height: 32
                spacing: 0

                Button {
                    Layout.minimumWidth: 32
                    Layout.fillHeight: true

                    enabled: wrapper.hasChildren

                    text: {
                        return wrapper.hidden ? "+" : "-"
                    }

                    onClicked: {
                        wrapper.hidden = !wrapper.hidden
                    }
                }

                TextField {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    text: treeItem ? treeItem.title : ""

                    onAccepted: {
                        treeItem.title = text
                    }
                }

                Button {
                    Layout.minimumWidth: 100
                    Layout.fillWidth: false
                    Layout.fillHeight: true
                    text: treeItem ? treeItem.flow : ""

                    onClicked: {
                        treeItem.toggleFlow()
                    }
                }

                Button {
                    Layout.minimumWidth: 100
                    Layout.fillWidth: false
                    Layout.fillHeight: true
                    text: treeItem ? treeItem.layout : ""

                    onClicked: {
                        treeItem.toggleLayout()
                    }
                }

                Button {
                    Layout.minimumWidth: 100
                    Layout.fillWidth: false
                    Layout.fillHeight: true
                    text: treeItem ? treeItem.bounds.toString() : ""
                }

                Button {
                    Layout.minimumWidth: 100
                    Layout.fillWidth: false
                    Layout.fillHeight: true
                    text: treeItem ? treeItem.headerBounds.toString() : ""
                }

                Button {
                    Layout.minimumWidth: 100
                    Layout.fillWidth: false
                    Layout.fillHeight: true
                    text: treeItem ? treeItem.contentBounds.toString() : ""
                }

                Button {
                    Layout.minimumWidth: 100
                    Layout.fillWidth: false
                    Layout.fillHeight: true
                    text: treeItem ? treeItem.refreshRate : ""
                }

                Button {
                    Layout.minimumWidth: 100
                    Layout.fillWidth: false
                    Layout.fillHeight: true
                    text: treeItem ? treeItem.hwnd.toString() : ""
                }

                Button {
                    Layout.minimumWidth: 100
                    Layout.fillWidth: false
                    Layout.fillHeight: true
                    text: "Add Child"

                    onClicked: {
                        treeItem.addChild("Child", "Horizontal", "Split");
                    }
                }

                Button {
                    Layout.minimumWidth: 100
                    Layout.fillWidth: false
                    Layout.fillHeight: true
                    text: "Move Up"

                    onClicked: {
                        treeItem.moveUp()
                    }
                }

                Button {
                    Layout.minimumWidth: 100
                    Layout.fillWidth: false
                    Layout.fillHeight: true
                    text: "Move Down"

                    onClicked: {
                        treeItem.moveDown()
                    }
                }

                Button {
                    Layout.minimumWidth: 100
                    Layout.fillWidth: false
                    Layout.fillHeight: true
                    text: "Remove"

                    onClicked: treeItem.remove()
                }
            }

            Item {
                id: submodelWrapper
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: itemRow.bottom

                anchors.leftMargin: 32

                property bool hidden: false
                visible: wrapper.childrenVisible

                Repeater {
                    id: submodelRepeater
                    model: treeItem ? treeItem.children : null

                    delegate: testDelegate
                    property var parentWrapper: wrapper
                }
            }
        }
    }
}