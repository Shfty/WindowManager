import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.5
import QtQuick.Controls.Universal 2.3

Window {
    title: "Window Manager"

    property int baseFlags: Qt.FramelessWindowHint | Qt.WindowDoesNotAcceptFocus
    property int extraFlags: 0
    flags: baseFlags | extraFlags

    x: Screen.virtualX
    y: Screen.virtualY
    width: Screen.width
    height: Screen.height

    readonly property int minSize: Math.min(width, height)

    color: "transparent"

    Universal.theme: Universal.Dark
    Universal.accent: "white"
}