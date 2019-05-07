import QtQuick 2.11
import QtQuick.Window 2.11
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.4
import QtQuick.Controls.Universal 2.3

import ".."

Item {
	id: itemWrapper

	property var parentItem: null

	property var model: null
	property var delegate: null

	property var animationEasing: Easing.OutCubic
	property int animationDuration: 200 * (model.monitor.refreshRate / 60)

	property rect bounds: model ? model.bounds : Qt.rect(0, 0, 0, 0)
	property rect headerBounds: model ? model.headerBounds : Qt.rect(0, 0, 0, 0)
	property rect contentBounds: model ? model.contentBounds : Qt.rect(0, 0, 0, 0)
	property var hwnd: model ? model.hwnd : null
	property bool hwndValid: false
	onHwndChanged: {
		hwndValid = model ? model.isHwndValid() : false
	}
	
	x: bounds.x
	y: bounds.y
	width: bounds.width
	height: bounds.height

	TreeHeader {
		id: itemBackground

		x: headerBounds.x
		y: headerBounds.y
		width: headerBounds.width
		height: headerBounds.height

		Behavior on x {
			SequentialAnimation {
				ScriptAction { script: model.isAnimating = true }
				NumberAnimation {
					duration: animationDuration
					easing.type: animationEasing
				}
				ScriptAction { script: model.isAnimating = false }
			}
		}

		Behavior on y {
			SequentialAnimation {
				ScriptAction { script: model.isAnimating = true }
				NumberAnimation {
					duration: animationDuration
					easing.type: animationEasing
				}
				ScriptAction { script: model.isAnimating = false }
			}
		}

		Behavior on width {
			SequentialAnimation {
				ScriptAction { script: model.isAnimating = true }
				NumberAnimation {
					duration: animationDuration
					easing.type: animationEasing
				}
				ScriptAction { script: model.isAnimating = false }
			}
		}

		Behavior on height {
			SequentialAnimation {
				ScriptAction { script: model.isAnimating = true }
				NumberAnimation {
					duration: animationDuration
					easing.type: animationEasing
				}
				ScriptAction { script: model.isAnimating = false }
			}
		}
			
		treeItem: itemWrapper.model
	}
	
	Item {
		id: childWrapper

		clip: model ? model.layout == "Tabbed" : false

		x: contentBounds.x
		y: contentBounds.y
		width: contentBounds.width
		height: contentBounds.height

		Behavior on x {
			SequentialAnimation {
				ScriptAction { script: model.isAnimating = true }
				NumberAnimation {
					duration: animationDuration
					easing.type: animationEasing
				}
				ScriptAction { script: model.isAnimating = false }
			}
		}

		Behavior on y {
			SequentialAnimation {
				ScriptAction { script: model.isAnimating = true }
				NumberAnimation {
					duration: animationDuration
					easing.type: animationEasing
				}
				ScriptAction { script: model.isAnimating = false }
			}
		}

		Behavior on width {
			SequentialAnimation {
				ScriptAction { script: model.isAnimating = true }
				NumberAnimation {
					duration: animationDuration
					easing.type: animationEasing
				}
				ScriptAction { script: model.isAnimating = false }
			}
		}

		Behavior on height {
			SequentialAnimation {
				ScriptAction { script: model.isAnimating = true }
				NumberAnimation {
					duration: animationDuration
					easing.type: animationEasing
				}
				ScriptAction { script: model.isAnimating = false }
			}
		}

		Component.onCompleted: {
			if(!model) return

			for(var i = 0; i < model.children.length; ++i)
			{
				childWrapper.addChild(model.children[i])
			}
		}

		property var model: itemWrapper.model
		property var delegate: null

		property var childItems: []
		property var itemBuffer: []
		property var incubator: null

		function addChild(model) {
			if(incubator == null)
			{
				incubateItem(model);
			}
			else
			{
				itemBuffer.push(model);
			}
		}

		function incubateItem(model) {
			incubator = headerDelegate.incubateObject(
				childWrapper,
				{
					model: model
				}
			);

			if (incubator.status != Component.Ready) {
				incubator.onStatusChanged = function(status) {
					if (status == Component.Ready) {
						incubatorReady();
					}
				}
			} else {
				incubatorReady();
			}
		}

		function incubatorReady() {
			childItems.push(incubator.object)
			if(itemBuffer.length > 0) {
				var item = itemBuffer.shift()
				incubateItem(item)
			}
		}

		Connections {
			target: model
			onChildAdded: function(index, child) {
				childWrapper.addChild(child)
			}
			onChildRemoved: function(index, child) {
				for(var i = 0; i < childWrapper.childItems.length; ++i)
				{
					var candidate = childWrapper.childItems[i]
					if(candidate.model === child)
					{
						childWrapper.childItems.splice(i, 1)
						candidate.destroy()
					}
				}
			}
		}
	}
}