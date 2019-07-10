import QtQuick 2.12
import QtQuick.Dialogs 1.2

import ".."

DetailView {
    property bool hasServerCore: (typeof serverCore != 'undefined')
    detailObject: hasServerCore ? serverCore.settingsContainer : null

    blacklistProperties: [
        "objectName"
    ]

    property var fileProperties: [
        "wallpaperUrl",
    ]

    property var colorProperties: [
        "colorInactiveHeader",
        "colorActiveHeader",
        "colorContainerPlaceholder",
        "colorContainerBorder"
    ]

    onPropertyClicked: function(key, value) {
        if(fileProperties.indexOf(key) != -1) {
            fileDialog.targetProperty = key
            fileDialog.visible = true;
        }

        if(colorProperties.indexOf(key) != -1) {
            colorDialog.targetProperty = key
            colorDialog.visible = true;
        }
    }

    FileDialog {
        id: fileDialog
        title: "Please choose a file"

        nameFilters: [ "Image files (*.jpg *.png)", "All files (*)" ]

        property string targetProperty: null

        onAccepted: {
            detailObject[targetProperty] = fileDialog.fileUrl
            targetProperty = null
        }
    }

    ColorDialog {
        id: colorDialog
        title: "Please choose a color"

        showAlphaChannel: true
        color: targetProperty ? detailObject[targetProperty] : "#00000000"

        onCurrentColorChanged: {
            if(targetProperty == null) return

            detailObject[targetProperty] = colorDialog.currentColor
        }

        property var targetProperty: null
        property color cachedColor: "#00000000"

        onTargetPropertyChanged: {
            if(!targetProperty) return

            cachedColor = detailObject[targetProperty]
        }

        onAccepted: {
            targetProperty = null
        }

        onRejected: {
            detailObject[targetProperty] = cachedColor
            cachedColor = "#00000000"
            targetProperty = null
        }
    }
}