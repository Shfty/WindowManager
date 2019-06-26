import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.5

RowLayout {
    ListView {
        id: windowsList

        Layout.fillWidth: true
        Layout.fillHeight: true

        ScrollIndicator.vertical: ScrollIndicator { }

        model: launcherCore.windowView.screenList

        delegate: ItemDelegate {
            text: modelData.name
            width: parent.width
            onClicked: {
                detailView.detailObject = modelData
            }
        }
    }
    DetailView {
        id: detailView
        Layout.fillWidth: true
        Layout.fillHeight: true
    }
}