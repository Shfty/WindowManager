import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import QtQuick.Controls.Universal 2.3

ApplicationWindow {
    id: appWindow

    signal reloadRequested()
    signal quitRequested()
    signal windowSelected(var hwnd)

    width: 1280
    height: 720

    color: "transparent"

    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint

    property string currentItem: ""

    Universal.theme: Universal.Dark

    onClosing: {
        close.accepted = false
        visibility = Window.Hidden
    }

    onActiveChanged: {
        if(!active && currentItem !== "Config")
        {
            close()
        }
    }

    StackLayout {
        id: stackLayout

        anchors.fill: parent

        currentIndex: {
            switch(appWindow.currentItem) {
            case "Config":
                return 0
            case "ItemSettings":
                return 1
            case "PowerMenu":
                return 2
            case "WindowList":
                return 3
            default:
                return -1
            }
        }

        ConfigOverlay {
            id: configOverlay
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        ItemSettingsOverlay {
            id: itemSettingsOverlay
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        PowerMenuOverlay {
            id: powerMenuOverlay
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        WindowListOverlay {
            id: windowListOverlay
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
}
