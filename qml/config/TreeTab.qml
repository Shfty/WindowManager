import QtQuick 2.11
import QtQuick.Layouts 1.3

RowLayout {
	spacing: 10

	ConfigView {
		id: configView
		
		Layout.fillWidth: true
		Layout.fillHeight: true

		model: configWindow.model
	}
	DetailView {
		id: detailView

		Layout.fillWidth: true
		Layout.fillHeight: true

		detailObject: configView.selectedItem
	}
}