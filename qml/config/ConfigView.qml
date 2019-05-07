import QtQuick 2.11
import QtQuick.Window 2.11
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.4
import QtQuick.Controls.Universal 2.3

import ".."

// Root item
Item {
	id: configView

	property var model: null
	property var selectedItem: null

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
				model: configView.model ? configView.model.children : null
				delegate: recursiveDelegate
			}
		}
	}
}
