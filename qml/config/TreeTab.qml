import QtQuick 2.11
import QtQuick.Layouts 1.3

RowLayout {
    id: treeTab

    spacing: 10

    property var hasAppCore: typeof (appCore != 'undefined')
    property var model: hasAppCore ? appCore.treeModel : null

    ConfigView {
        id: configView

        Layout.fillWidth: true
        Layout.fillHeight: true

        model: treeTab.model
    }
    DetailView {
        id: detailView

        Layout.fillWidth: true
        Layout.fillHeight: true

        detailObject: configView.selectedItem
    }
}