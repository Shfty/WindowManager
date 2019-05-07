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
	property var blacklistProperties: []

	signal propertyClicked(var key, var value)

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
						if(blacklistProperties.indexOf(prop) != -1) continue

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

						onClicked: {
							var prop = model.modelData["key"]
							detailView.propertyClicked(prop, detailObject[prop])
						}
					}

					Button {
						Layout.fillWidth: true
						Layout.fillHeight: true

						text: model.modelData["value"]
						ToolTip.visible: hovered
						ToolTip.delay: 500
						ToolTip.text: text

						onClicked: {
							var prop = model.modelData["key"]
							detailView.propertyClicked(prop, detailObject[prop])
						}
					}
				}
			}
		}
	}
}
