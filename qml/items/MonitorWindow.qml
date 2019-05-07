import QtQuick 2.11
import QtQuick.Window 2.11

// Monitor window
Window {
	id: monitorWindow

    property var treeItem: model.modelData
    property rect bounds: treeItem.bounds
    property real animationRate: treeItem ? treeItem.monitor.refreshRate / 60 : 1

    Component.onCompleted: {
		windowManager.setBackgroundWindow(monitorWindow)
    }

	onBoundsChanged: {
		var pos = Qt.point(bounds.x, bounds.y)
		var sz = Qt.size(bounds.width, bounds.height)

		windowManager.beginMoveWindows()
		windowManager.moveWindow(this, pos, sz)
		windowManager.endMoveWindows()
	}
}