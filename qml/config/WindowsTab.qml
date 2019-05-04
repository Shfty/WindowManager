import QtQuick 2.11
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.4

Flickable {
	contentHeight: windowsColumn.height
	
	ColumnLayout {
		id: windowsColumn
		anchors.left: parent.left
		anchors.right: parent.right
		
		Repeater {
			model: windowManager.windowList
			delegate: RowLayout {
				Layout.fillWidth: true

				Button {
					text: modelData.hwnd.toString()
					Layout.fillWidth: false
					Layout.fillHeight: true
					Layout.minimumWidth: 200
				}
				Button {
					text: modelData.winTitle
					Layout.fillWidth: true
					Layout.fillHeight: true
				}
				Button {
					text: modelData.winClass
					Layout.fillWidth: false
					Layout.fillHeight: true
					Layout.minimumWidth: 200
				}
				Button {
					text: modelData.winProcess
					Layout.fillWidth: false
					Layout.fillHeight: true
					Layout.minimumWidth: 200
				}
			}
		}
	}
}