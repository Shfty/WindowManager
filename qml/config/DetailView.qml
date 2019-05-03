import QtQuick 2.11
import QtQuick.Window 2.11
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.4
import QtQuick.Controls.Universal 2.3

import ".."

// Root item
Item {
	id: detailView

	property var detailObject: null

	Flickable {
		anchors.fill: parent
		contentHeight: wrapperColumn.height

		ColumnLayout {
			id: wrapperColumn
			anchors.left: parent.left
			anchors.right: parent.right

			Repeater {
				model: {
					var arr = []

					for (var prop in detailObject) {
						if(typeof detailObject[prop] == "function") continue
						
						arr.push({
							"key": prop,
							"value": detailObject[prop].toString()
						})
					}
					
					return arr
				}
				delegate: RowLayout {
					Layout.fillWidth: true
					Layout.fillHeight: false
					Layout.minimumHeight: 30

					Button {
						Layout.fillWidth: false
						Layout.fillHeight: true
						Layout.minimumWidth: 200

						text: model.modelData["key"]
					}

					Button {
						Layout.fillWidth: true
						Layout.fillHeight: true

						text: model.modelData["value"]
					}
				}
			}
		}
	}
}
