import QtQuick 2.11
import QtQuick.Window 2.11

// Monitor window
Window {
	id: monitorWindow

    property var treeItem: model.modelData
    property rect bounds: treeItem ? treeItem.bounds : Qt.rect(0, 0, 0, 0)
    property real animationRate: treeItem ? treeItem.refreshRate / 60 : 1
	property point offset: Qt.point(0, 0)

	onBoundsChanged: {
		var pos = Qt.point(bounds.x + offset.x, bounds.y + offset.y)
		var sz = Qt.size(bounds.width, bounds.height)

		windowManager.beginMoveWindows()
		windowManager.moveWindow(this, pos, sz)
		windowManager.endMoveWindows()
	}
}