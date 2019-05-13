import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.5
import QtQuick.Controls.Universal 2.3

import ".."

Item {
    id: configWindow

    anchors.fill: parent

    Universal.theme: Universal.Dark

    Rectangle {
        anchors.top: bar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        color: "black"

        StackLayout {
            currentIndex: bar.currentIndex
            anchors.fill: parent
            anchors.margins: 10

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

    TabBar {
        id: bar
        width: parent.width

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
}