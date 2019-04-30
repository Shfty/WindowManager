import QtQuick 2.11
import QtQuick.Window 2.11

// Monitor window
MonitorWindow {
	id: thumbnailOverlayWindow

	flags: Qt.FramelessWindowHint | Qt.WA_TranslucentBackground
	color: "#00000000"
	visible: true

	Component.onCompleted: {
		windowManager.setBackgroundWindow(thumbnailOverlayWindow)
	}
}