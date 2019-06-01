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
            Image {
                id: iconImage

                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter

                sourceSize: Qt.size(parent.height, parent.height)

                source: "image://treeIcon/" + modelData.winProcess
            }
            Label {
                id: titleText

                anchors.left: iconImage.right
                anchors.leftMargin: itemControl.padding

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
