import QtQuick 2.11
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.4

RowLayout {
    Flickable {
        Layout.fillWidth: true
        Layout.fillHeight: true

        contentHeight: windowsColumn.height

        ColumnLayout {
            id: windowsColumn
            anchors.left: parent.left
            anchors.right: parent.right

            Repeater {
                model: appCore.windowView.screenList
                delegate: RowLayout {
                    Layout.fillWidth: true

                    Button {
                        text: modelData.name
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        onClicked: {
                            detailView.detailObject = modelData
                        }
                    }
                }
            }
        }
    }
    DetailView {
        id: detailView
        Layout.fillWidth: true
        Layout.fillHeight: true
    }
}