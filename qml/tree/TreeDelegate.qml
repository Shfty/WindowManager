import QtQuick 2.11
import QtQuick.Window 2.11
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.4
import QtQuick.Controls.Universal 2.3

import ".."

Item {
	id: itemWrapper

	property var treeItem: model.modelData
	property var delegate: treeDelegate

	property var animationEasing: Easing.OutCubic
	property int animationDuration: 200 * treeWindow.animationRate

	property rect bounds: treeItem.bounds
	property rect headerBounds: treeItem.headerBounds
	property rect contentBounds: treeItem.contentBounds
	property var hwnd: treeItem.hwnd
	property bool hwndValid: false
	onHwndChanged: {
		hwndValid = treeItem.isHwndValid()
	}
	
	x: bounds.x
	y: bounds.y
	width: bounds.width
	height: bounds.height

	property var childItems: []
	property var incubators: []

	function addChild(treeItem) {
		var incubator = itemWrapper.delegate.incubateObject(childWrapper, {
			treeItem: treeItem
		});

		incubators.push(incubator)

		if (incubator.status != Component.Ready) {
			incubator.onStatusChanged = function(status) {
				if (status == Component.Ready) {
					childItems.push(incubator.object)
					incubators.splice(incubators.indexOf(incubator), 1)
				}
			}
		} else {
			childItems.push(incubator.object)
			incubators.splice(incubators.indexOf(incubator), 1)
		}
	}

	Component.onCompleted: {
		for(var i = 0; i < treeItem.children.length; ++i)
		{
			itemWrapper.addChild(treeItem.children[i])
		}
	}

	Connections {
		target: treeItem
		onChildAdded: function(index, child) {
			itemWrapper.addChild(child)
		}
		onChildRemoved: function(index, child) {
			for(var i = 0; i < itemWrapper.childItems.length; ++i)
			{
				var candidate = itemWrapper.childItems[i]
				if(candidate.treeItem === child)
				{
					itemWrapper.childItems.splice(i, 1)
					candidate.destroy()
				}
			}
		}
	}

	TreeHeader {
		id: itemHeader
	
		x: headerBounds.x
		y: headerBounds.y
		width: headerBounds.width
		height: headerBounds.height

		Behavior on x {
			NumberAnimation {
				duration: animationDuration
				easing.type: animationEasing
			}
		}

		Behavior on y {
			NumberAnimation {
				duration: animationDuration
				easing.type: animationEasing
			}
		}

		Behavior on width {
			NumberAnimation {
				duration: animationDuration
				easing.type: animationEasing
			}
		}

		Behavior on height {
			NumberAnimation {
				duration: animationDuration
				easing.type: animationEasing
			}
		}

		treeItem: itemWrapper.treeItem
	}

	Item {
		id: childWrapper

		clip: true
	
		x: contentBounds.x
		y: contentBounds.y
		width: contentBounds.width
		height: contentBounds.height

		visible: treeItem.isVisible

		Behavior on x {
			SequentialAnimation {
    			ScriptAction { script: treeItem.isAnimating = true }
				NumberAnimation {
					duration: animationDuration
					easing.type: animationEasing
				}
    			ScriptAction { script: treeItem.isAnimating = false }
			}
		}

		Behavior on y {
			SequentialAnimation {
    			ScriptAction { script: treeItem.isAnimating = true }
				NumberAnimation {
					duration: animationDuration
					easing.type: animationEasing
				}
    			ScriptAction { script: treeItem.isAnimating = false }
			}
		}

		Behavior on width {
			SequentialAnimation {
    			ScriptAction { script: treeItem.isAnimating = true }
				NumberAnimation {
					duration: animationDuration
					easing.type: animationEasing
				}
    			ScriptAction { script: treeItem.isAnimating = false }
			}
		}

		Behavior on height {
			SequentialAnimation {
    			ScriptAction { script: treeItem.isAnimating = true }
				NumberAnimation {
					duration: animationDuration
					easing.type: animationEasing
				}
    			ScriptAction { script: treeItem.isAnimating = false }
			}
		}

		Rectangle {
			id: itemBackground
			anchors.fill: parent

			visible: treeItem.depth > 1

			color: Options.containerPlaceholderColor
		}

		ManagedWindow {
			anchors.fill: parent
			hwnd: treeItem.hwnd
			parentHwnd: windowManager.getWindowHwnd(treeWindow.thumbnailOverlay)
		}
	}
}