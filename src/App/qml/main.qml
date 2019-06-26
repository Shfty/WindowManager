import QtQuick 2.0
import QtQuick.Window 2.12
import QtQuick.Controls 2.12

import "."

AppWindow {
    id: appWindow

    // Animation
    property real animationDuration: 600
    property int animationCurve: Easing.OutQuad

    // Tree Root
    property bool showTree: false

    property var treeModel: appCore ? appCore.treeModel : null
    property var treeRoot: treeModel ? treeModel.rootItem : null
    property bool treeRootReady: treeRoot ? true : false

    // Wallpaper
    ShaderEffectSource {
        id: wallpaper

        anchors.fill: parent

        property rect virtualGeometry: appCore.windowView.screenList[0].virtualGeometry

        sourceRect: Qt.rect(
            Screen.virtualX - virtualGeometry.x,
            Screen.virtualY - virtualGeometry.y,
            Screen.width,
            Screen.height
        )

        sourceItem: Image {
            id: wallpaperImage
            asynchronous: true
            source: appCore.settingsContainer.wallpaperUrl
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

        // Loading Wrappers
        Item {
            id: nodeWrapper
            anchors.fill: parent

            readonly property bool ready: appWindow.treeRootReady
            property var incubator: null
            property bool loaded: false
            onReadyChanged: {
                if(ready) {
                    incubator = recursiveDelegate.incubateObject(
                        this,
                        {
                            model: appWindow.treeRoot,
                            visualDelegate: nodeDelegate,
                            boundsProperty: "contentBounds",
                            childBoundsProperty: "nodeBounds",
                            clipChildren: true
                        }
                    )

                    incubator.onStatusChanged = function(status) {
                        nodeWrapper.loaded = true
                    }
                }
            }
        }

        Item {
            id: headerWrapper
            anchors.fill: parent

            readonly property bool ready: appWindow.treeRootReady
            property var incubator: null
            property bool loaded: false
            onReadyChanged: {
                if (ready) {
                    incubator = recursiveDelegate.incubateObject(
                        this,
                        {
                            model: appWindow.treeRoot,
                            visualDelegate: headerDelegate,
                            boundsProperty: "bounds",
                            clipChildren: true
                        }
                    )

                    incubator.onStatusChanged = function(status) {
                        headerWrapper.loaded = true
                    }
                }
            }
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
                            console.log("Transition in complete")
                            appWindow.treeRoot.updateWindowPosition()
                        }
                    }
                }
            }
        ]
    }

    // Spinner
    readonly property bool incubatorsLoaded: nodeWrapper.loaded && headerWrapper.loaded
    onIncubatorsLoadedChanged: {
        if(incubatorsLoaded)
        {
            var startupBinding = Qt.binding(function() {
                return appWindow.treeRoot.startupComplete ? "above" : "here"
            })

            spinnerWrapper.setState(startupBinding)
        }
    }

    Item {
        id: spinnerWrapper

        anchors.centerIn: parent
        width: appWindow.minSize * 0.5
        height: appWindow.minSize * 0.5

        Component.onCompleted: {
            print("Easing Type:", Easing.OutCubic)
            setState("here")
        }

        BusyIndicator {
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

    // Components
    Component {
        id: recursiveDelegate
        RecursiveDelegate {
        }
    }

    Component {
        id: boundsRectDelegate

        Rectangle {
            anchors.fill: parent
            color: "transparent"
            border.color: "red"
            border.width: 2
        }
    }

    Component {
        id: headerBoundsRectDelegate

        Rectangle {
            anchors.fill: parent
            color: "transparent"
            border.color: "green"
            border.width: 2
        }
    }

    Component {
        id: contentBoundsRectDelegate

        Rectangle {
            anchors.fill: parent
            color: "transparent"
            border.color: "blue"
            border.width: 2
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

    // Debug
    Loader {
        id: debugLoader
        active: false
        sourceComponent: Component {
            Item {
                anchors.fill: parent

                Item {
                    id: boundsWrapper
                    anchors.fill: parent

                    readonly property bool ready: appWindow.treeRootReady
                    onReadyChanged: {
                        if (ready) {
                            var incubator = recursiveDelegate.incubateObject(
                                this,
                                {
                                    "model": appWindow.treeRoot,
                                    "visualDelegate": boundsRectDelegate,
                                    "boundsProperty": "bounds"
                                }
                            )
                        }
                    }
                }

                Item {
                    id: contentBoundsWrapper
                    anchors.fill: parent

                    readonly property bool ready: appWindow.treeRootReady
                    onReadyChanged: {
                        if (ready) {
                            var incubator = recursiveDelegate.incubateObject(
                                this,
                                {
                                    "model": appWindow.treeRoot,
                                    "visualDelegate": contentBoundsRectDelegate,
                                    "boundsProperty": "contentBounds"
                                }
                            )
                        }
                    }
                }
            }
        }
    }
}