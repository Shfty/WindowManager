import QtQuick 2.0
import QtQuick.Window 2.12
import QtQuick.Controls 2.12

import "."

AppWindow {
    id: appWindow

    //extraFlags: Qt.WindowStaysOnTopHint

    // Animation
    property real animationDuration: 1000
    property int animationCurve: Easing.OutQuint

    // Tree Root
    property var appCore: null
    onAppCoreChanged: {
        print("App Core:", appCore)
    }

    property bool showTree: false

    property var treeModel: appCore ? appCore.treeModel : null

    property var treeRoot: treeModel ? treeModel.rootItem : null
    property bool treeRootReady: treeRoot ? true : false

    property var settingsContainer: appCore ? appCore.settingsContainer : null
    property bool hasSettingsContainer: settingsContainer ? true : false

    property var windowView: appCore ? appCore.windowView : null
    property bool hasWindowView: windowView ? true: false

    // Wallpaper
    ShaderEffectSource {
        id: wallpaper

        anchors.fill: parent

        property rect virtualGeometry: hasWindowView ? windowView.screenList[0].virtualGeometry : Qt.rect(0, 0, 0, 0)

        sourceRect: Qt.rect(
            Screen.virtualX - virtualGeometry.x,
            Screen.virtualY - virtualGeometry.y,
            Screen.width,
            Screen.height
        )

        sourceItem: Image {
            id: wallpaperImage
            asynchronous: true
            source: hasSettingsContainer ? settingsContainer.wallpaperUrl : ""
            width: wallpaper.virtualGeometry.width
            height: wallpaper.virtualGeometry.height
            fillMode: Image.PreserveAspectCrop
        }

        readonly property bool ready: wallpaperImage.status === Image.ready &&
                                      spinner.stableState !== ""

        state: wallpaperImage.status === Image.Ready ? "visible" : "hidden"
        property string stableState: ""

        states: [
            State {
                name: "hidden"
                PropertyChanges {
                    target: wallpaper
                    opacity: 0
                }
            },
            State {
                name: "visible"
                PropertyChanges {
                    target: wallpaper
                    opacity: 1
                }
            }
        ]

        transitions: [
            Transition {
                id: wallpaperTransition

                NumberAnimation {
                    target: wallpaper
                    property: "opacity"
                }

                onRunningChanged: {
                    if(!running) {
                        wallpaper.stableState = wallpaper.state
                    }
                }
            }
        ]
    }

    // Tree Items
    Component {
        id: itemDelegateComponent
        ItemDelegate {}
    }

    Incubator {
        id: itemWrapper

        asynchronous: false

        anchors.top: rootHeader.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        sourceComponent: itemDelegateComponent
        active: appWindow ? (appWindow.treeRoot ? true : false) : false
        properties: ({
            modelData: Qt.binding(function() { return appWindow.treeRoot })
        })

        // Animation
        readonly property bool ready: wallpaper.stableState === "visible" &&
                                      rootHeader.stableState === "visible" &&
                                      appWindow.treeRoot.startupComplete &&
                                      item

        state: ready ? "here" : "below"

        states: [
            State {
                name: "below"
                PropertyChanges {
                    target: itemWrapper
                    opacity: 0
                    scale: 0.9
                }
            },
            State {
                name: "here"
                PropertyChanges {
                    target: itemWrapper
                    opacity: 1
                    scale: 1
                }
            },
            State {
                name: "above"
                PropertyChanges {
                    target: itemWrapper
                    opacity: 0
                    scale: 1.1
                }
            }
        ]

        transitions: [
            Transition {
                NumberAnimation {
                    target: itemWrapper
                    properties: "opacity, scale"
                    duration: animationDuration
                    easing.type: animationCurve
                }

                onRunningChanged: {
                    if(!running) {
                        if(itemWrapper.state === "here")
                        {
                            itemWrapper.item.updateWindows()
                        }
                    }
                }
            }
        ]
    }

    // Root Header
    Incubator {
        id: rootHeader

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: hasSettingsContainer ? settingsContainer.headerSize : 30

        sourceComponent: Component {
            HeaderDelegate {
                anchors.fill: parent
                modelData: appWindow.treeRoot
            }
        }

        readonly property bool ready: wallpaper.stableState !== "" &&
                                      spinner.stableState !== "" &&
                                      item
        state: ready ? "visible" : "hidden"
        property string stableState: ""

        transform: Translate {
            id: rootHeaderTransform
        }

        states: [
            State {
                name: "hidden"
                PropertyChanges {
                    target: rootHeaderTransform
                    y: -rootHeader.height
                }
            },
            State {
                name: "visible"
                PropertyChanges {
                    target: rootHeaderTransform
                    y: 0
                }
            }
        ]

        transitions: [
            Transition {
                id: rootHeaderTransition

                PropertyAnimation {
                    target: rootHeaderTransform
                    properties: "y"
                    duration: animationDuration
                    easing.type: animationCurve
                }

                onRunningChanged: {
                    if(!running) {
                        rootHeader.stableState = rootHeader.state
                    }
                }
            }
        ]
    }

    // Spinner
    Item {
        id: spinner

        anchors.centerIn: parent
        width: appWindow.minSize * 0.5
        height: width

        BusyIndicator {
            layer.enabled: true
            anchors.fill: parent
            running: spinner.opacity > 0
        }

        readonly property bool ready: wallpaper.stableState === "visible" &&
                                      rootHeader.stableState === "visible" &&
                                      itemWrapper.state === "here"
        state: "below"
        property var stableState: ""
        Component.onCompleted: {
            state = Qt.binding(function() { return ready ? "above" : "here" })
        }

        states: [
            State {
                name: "below"
                PropertyChanges {
                    target: spinner
                    opacity: 0
                    scale: 0.9
                }
            },
            State {
                name: "here"
                PropertyChanges {
                    target: spinner
                    opacity: 1
                    scale: 1
                }
            },
            State {
                name: "above"
                PropertyChanges {
                    target: spinner
                    opacity: 0
                    scale: 1.1
                }
            }
        ]

        transitions: [
            Transition {
                id: spinnerTransition

                PropertyAnimation {
                    target: spinner
                    properties: "opacity, scale"
                    duration: animationDuration
                    easing.type: animationCurve
                }

                onRunningChanged: {
                    if(!running) {
                        spinner.stableState = spinner.state
                    }
                }
            }
        ]
    }
}