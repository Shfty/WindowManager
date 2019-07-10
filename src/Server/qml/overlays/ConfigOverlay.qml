import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.5

import ".."

Rectangle {
    id: overlayWindow
    objectName: "ConfigOverlay"

    color: "black"

    Rectangle {
        id: titleBar

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 32

        color: "#20FFFFFF"

        Button {
            id: closeButton

            anchors.top: parent.top
            anchors.right: parent.right
            anchors.bottom: parent.bottom

            font.family: "Segoe MDL2 Assets"
            text: "\uE8BB"

            onClicked: {
                Window.window.close()
            }
        }

        MouseArea {
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: closeButton.left
            anchors.bottom: parent.bottom
            property point lastMousePos: Qt.point(0, 0)
            onPressed: { lastMousePos = Qt.point(mouseX, mouseY); }
            onMouseXChanged: appWindow.x += (mouseX - lastMousePos.x)
            onMouseYChanged: appWindow.y += (mouseY - lastMousePos.y)
        }
    }

    TabBar {
        id: bar

        anchors.top: titleBar.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        TabButton {
            text: qsTr("Tree")
        }
        TabButton {
            text: qsTr("Windows")
        }
        TabButton {
            text: qsTr("Monitors")
        }
        TabButton {
            text: qsTr("Settings")
        }
    }

    StackLayout {
        anchors.top: bar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        currentIndex: bar.currentIndex
        anchors.margins: 10

        clip: true

        TreeTab {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
        WindowList {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
        MonitorsTab {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
        SettingsTab {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
}