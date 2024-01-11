import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Window Enumerator")

    flags: Qt.WindowStaysOnTopHint

    property var windowList: []

    function windowListUpdated(newWindowList)
    {
        windowList = newWindowList
    }

    ListView {
        anchors.fill: parent
        model: windowList

        delegate: Text {
            text: modelData
        }
    }
}
