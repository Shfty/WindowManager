import QtQuick 2.12
import QtQuick.Controls 2.5

// Root item
Item {
    id: detailView

    property var detailObject: null
    property var blacklistProperties: []

    signal propertyClicked(var key, var value)

    ListView {
        anchors.fill: parent

        ScrollIndicator.vertical: ScrollIndicator { }

        model: {
            if(!detailObject) return null

            var arr = []

            for (var prop in detailObject) {
                if(typeof detailObject[prop] == "function") continue
                if(blacklistProperties.indexOf(prop) != -1) continue

                arr.push({
                    "key": prop,
                    "value": detailObject[prop] ? detailObject[prop].toString() : null
                })
            }

            return arr
        }

        delegate: ItemDelegate {
            id: itemControl

            width: parent.width
            text: modelData.key

            contentItem: Label {
                Label {
                    id: keyText

                    anchors.left: parent.left

                    text: modelData.key
                    font: itemControl.font
                }
                Label {
                    id: valueText

                    anchors.right: parent.right

                    text: modelData.value
                    font: itemControl.font
                }
            }

            onClicked: {
                var prop = model.modelData["key"]
                detailView.propertyClicked(prop, detailObject[prop])
            }
        }
    }
}
