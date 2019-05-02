import QtQuick 2.11
import QtQuick.Window 2.11

import ".."

// Monitor window
MonitorWindow {
	id: thumbnailUnderlayWindow

	flags: Qt.FramelessWindowHint | Qt.WA_TranslucentBackground
	color: "#00000000"
	visible: true

	Component.onCompleted: {
		windowManager.setBackgroundWindow(thumbnailUnderlayWindow)
	}
}