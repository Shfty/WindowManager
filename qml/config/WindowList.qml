import QtQuick 2.12
import QtQuick.Controls 2.5

ListView {
    id: windowList

    signal windowSelected(var windowInfo)

    model: appCore.windowView.windowList

    delegate: ItemDelegate {
        id: itemControl

        width: parent.width

        contentItem: Label {
            Label {
                id: titleText

                anchors.left: parent.left

                text: modelData.winTitle
                font: itemControl.font
            }
            Label {
                id: classText

                anchors.right: parent.right

                text: modelData.winClass
                font: itemControl.font
            }
        }

        onClicked: {
            if(index == 0) {
                windowSelected(null);
            }
            else {
                windowSelected(modelData);
            }
        }
    }

    ScrollIndicator.vertical: ScrollIndicator { }
}
