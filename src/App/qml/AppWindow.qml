import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.5
import QtQuick.Controls.Universal 2.3

Window {
    title: "Window Manager"

    flags: Qt.FramelessWindowHint | Qt.WindowDoesNotAcceptFocus | Qt.WindowStaysOnBottomHint

    x: Screen.virtualX
    y: Screen.virtualY
    width: Screen.width
    height: Screen.height

    readonly property int minSize: Math.min(width, height)

    color: "transparent"

    Universal.theme: Universal.Dark
    Universal.accent: "white"
}