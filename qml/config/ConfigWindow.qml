import QtQuick 2.11
import QtQuick.Window 2.11
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.4
import QtQuick.Controls.Universal 2.3

import ".."

Window {
    id: configWindow

    visible: true

    title: "Window Manager Config"

    width: 1280
    height: 720

    Universal.theme: Universal.Dark

    property var model: null

    // Root item
    ConfigView {}
}