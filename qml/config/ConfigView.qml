import QtQuick 2.11
import QtQuick.Window 2.11
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.4
import QtQuick.Controls.Universal 2.3

import ".."

// Root item
Rectangle {
	id: configView
	anchors.fill: parent
	color: "black"

	Flickable {
		anchors.fill: parent
		contentHeight: {
			var h = 0
			for(var i = 0; i < rootRepeater.count; ++i) {
				var item = rootRepeater.itemAt(i);
				h += item.height
			}
			return h
		}

		Component {
			id: recursiveDelegate
			ConfigDelegate {}
		}

		Item {
			id: rootWrapper
			anchors.left: parent.left
			anchors.right: parent.right
			Repeater {
				id: rootRepeater
				model: configWindow.model
				delegate: recursiveDelegate
			}
		}
	}
}
