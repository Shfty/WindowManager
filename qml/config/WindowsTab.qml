import QtQuick 2.11
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.4

Item {
	Layout.fillWidth: true
	Layout.fillHeight: true

	ColumnLayout {
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.margins: 10
		
		Repeater {
			model: windowManager.windowList.titles
			delegate: Button {
				text: modelData
				Layout.fillWidth: true
			}
		}
	}
}