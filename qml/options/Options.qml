pragma Singleton
import QtQuick 2.0

QtObject {
    property string wallpaperUrl: "file:///C:/Users/Josh/Pictures/space-1511293211222-8946.jpg"
    property int wallpaperFillMode: Image.PreserveAspectCrop
    property double wallpaperBlurRadius: 128

    property int headerSize: 30

    property string inactiveHeaderColor: "#00007acc"
    property string activeHeaderColor: "#80007acc"

    property string containerPlaceholderColor: "#80007acc"
}