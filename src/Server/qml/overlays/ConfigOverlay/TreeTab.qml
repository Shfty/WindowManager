import QtQuick 2.12
import QtQuick.Layouts 1.12

RowLayout {
    id: treeTab

    spacing: 10

    property var hasserverCore: typeof (serverCore !== 'undefined')
    property var model: null //hasserverCore ? serverCore.treeModel : null

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