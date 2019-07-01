import QtQuick 2.0
import QtQuick.Window 2.12
import QtQuick.Controls 2.12

import "."

AppWindow {
    id: appWindow

    extraFlags: Qt.WindowStaysOnTopHint

    // Animation
    property real animationDuration: 1000
    property int animationCurve: Easing.OutQuint

    // Tree Root
    property bool showTree: false

    property var treeModel: appCore ? appCore.treeModel : null
    property var treeRoot: treeModel ? treeModel.rootItem : null
    property bool treeRootReady: treeRoot ? true : false

    property var settingsContainer: appCore ? appCore.settingsContainer : null
    property bool hasSettingsContainer: settingsContainer ? true: false

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

        opacity: wallpaperImage.status === Image.Ready ? 1.0 : 0.0
        Behavior on opacity {
            NumberAnimation {}
        }
    }

    // Tree
    Item {
        id: treeWrapper
        anchors.fill: parent

        Component {
            id: recursiveDelegate
            RecursiveDelegate {
            }
        }

        Component {
            id: nodeDelegate
            NodeDelegate {}
        }

        Component {
            id: headerDelegate
            HeaderDelegate {}
        }

        // Loading Wrappers
        Incubator {
            id: nodeWrapper
            anchors.fill: parent
            sourceComponent: recursiveDelegate
            properties: ({
                modelData: Qt.binding(function() { return appWindow.treeRoot }),
                visualDelegate: nodeDelegate,
                boundsProperty: "contentBounds",
                childBoundsProperty: "nodeBounds",
                clipChildren: true
            })
        }

        Incubator {
            id: headerWrapper
            anchors.fill: parent
            sourceComponent: recursiveDelegate
            properties: ({
                modelData: Qt.binding(function() { return appWindow.treeRoot }),
                visualDelegate: headerDelegate,
                boundsProperty: "bounds",
                clipChildren: true
            })
        }

        // Animation
        state: appWindow.showTree ? "here" : "below"

        states: [
            State {
                name: "below"
                PropertyChanges {
                    target: treeWrapper
                    opacity: 0
                    scale: 0.9
                }
            },
            State {
                name: "here"
                PropertyChanges {
                    target: treeWrapper
                    opacity: 1
                    scale: 1
                }
            },
            State {
                name: "above"
                PropertyChanges {
                    target: treeWrapper
                    opacity: 0
                    scale: 1.1
                }
            }
        ]

        transitions: [
            Transition {
                NumberAnimation {
                    target: treeWrapper
                    properties: "opacity, scale"
                    duration: animationDuration
                    easing.type: animationCurve
                }

                onRunningChanged: {
                    if(!running) {
                        if(treeWrapper.state == "here")
                        {
                            appWindow.treeRoot.updateWindowPosition()
                        }
                    }
                }
            }
        ]
    }

    // Spinner
    readonly property bool nodesLoaded: nodeWrapper.itemInstance ? nodeWrapper.itemInstance.loadComplete : false
    readonly property bool headersLoaded: headerWrapper.itemInstance ? headerWrapper.itemInstance.loadComplete : false
    readonly property bool treeReady: appWindow.treeRoot.startupComplete
    readonly property bool qmlReady: nodesLoaded && headersLoaded && treeReady
    onQmlReadyChanged: {
        if(qmlReady)
        {
            spinnerWrapper.setState("above")
        }
    }

    Item {
        id: spinnerWrapper

        anchors.centerIn: parent
        width: appWindow.minSize * 0.5
        height: appWindow.minSize * 0.5

        Component.onCompleted: {
            setState("here")
        }

        BusyIndicator {
            layer.enabled: true
            anchors.fill: parent
            running: spinnerWrapper.opacity > 0
        }

        state: "below"
        property var stateQueue: []
        function setState(newState) {
            if(spinnerTransition.running)
            {
                stateQueue.unshift(newState)
            }
            else
            {
                state = newState
            }
        }

        onStateChanged: {
            if(state === "above")
            {
                showTree = true
            }
        }

        states: [
            State {
                name: "below"
                PropertyChanges {
                    target: spinnerWrapper
                    opacity: 0
                    scale: 0.9
                }
            },
            State {
                name: "here"
                PropertyChanges {
                    target: spinnerWrapper
                    opacity: 1
                    scale: 1
                }
            },
            State {
                name: "above"
                PropertyChanges {
                    target: spinnerWrapper
                    opacity: 0
                    scale: 1.1
                }
            }
        ]

        transitions: [
            Transition {
                id: spinnerTransition

                NumberAnimation {
                    target: spinnerWrapper
                    properties: "opacity, scale"
                    duration: animationDuration
                    easing.type: animationCurve
                }

                onRunningChanged: {
                    if(!running) {
                        if(spinnerWrapper.stateQueue.length > 0)
                        {
                            spinnerWrapper.state = spinnerWrapper.stateQueue.pop()
                        }
                    }
                }
            }
        ]
    }
}