import QtQuick 2.11
import QtQuick.Layouts 1.3

import ".."

Item {
	Layout.fillWidth: true
	Layout.fillHeight: true

	DetailView {
		anchors.fill: parent
		detailObject: Options
	}
}