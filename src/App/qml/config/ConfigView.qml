import QtQuick 2.12
import QtQuick.Controls 2.5

import ".."

// Root item
Item {
    id: configView

    property var model: null
    property var selectedItem: null

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

        ScrollIndicator.vertical: ScrollIndicator { }

        Component {
            id: recursiveDelegate
            ConfigDelegate {}
        }

        Item {
            id: rootWrapper
            anchors.left: parent.left
            anchors.right: parent.right
            Repeater {
                id: rootRepeater
                model: configView.model ? configView.model.children : null
                delegate: recursiveDelegate
            }
        }
    }
}
