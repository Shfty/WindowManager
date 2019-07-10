import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.5

Rectangle {
    id: overlayWindow
    objectName: "PowerMenuOverlay"

    color: "black"

    ListView {
        id: powerMenu
        spacing: 0

        anchors.fill: parent

        model: ListModel {
            ListElement {
                name: "Exit"
                category: "Window Manager"
                func: function() {
                    appWindow.quitRequested()
                }
            }
            ListElement {
                name: "Reload"
                category: "Window Manager"
                func: function() {
                    appWindow.reloadRequested()
                }
            }
            ListElement {
                name: "Shut Down"
                category: "Power"
                func: function() {
                    serverCore.winShellController.shutdown();
                }
            }
            ListElement {
                name: "Reboot"
                category: "Power"
                func: function() {
                    serverCore.winShellController.reboot();
                }
            }
            ListElement {
                name: "Sleep"
                category: "Power"
                func: function() {
                    serverCore.winShellController.sleep();
                }
            }
            ListElement {
                name: "Log Out"
                category: "User"
                func: function() {
                    serverCore.winShellController.logout();
                }
            }
        }

        delegate: ItemDelegate {
            width: parent.width

            text: name

            onClicked: func()
        }

        section.property: "category"
        section.criteria: ViewSection.FullString
        section.delegate: Label {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 10

            height: contentHeight + anchors.margins

            verticalAlignment: Text.AlignVCenter

            font.bold: true

            text: section
        }
    }
}