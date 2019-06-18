import QtQuick 2.0
import QtQuick.Window 2.12
import QtQuick.Controls 2.5

import "."

ApplicationWindow {
    id: appWindow

    title: "Window Manager"

    flags: Qt.FramelessWindowHint | Qt.WindowDoesNotAcceptFocus | Qt.WindowStaysOnBottomHint

    x: Screen.virtualX
    y: Screen.virtualY
    width: Screen.width
    height: Screen.height

    readonly property int minSize: Math.min(width, height)
    readonly property int maxSize: Math.max(width, height)

    color: "transparent"

    readonly property bool hasAppCore: appCore ? true : false

    // Spinner
    Loader {
        id: spinnerLoader
        anchors.fill: parent
        active: true
        z: 1

        sourceComponent: Spinner {
            id: spinner

            anchors.centerIn: parent
            size: appWindow.minSize * 0.5
            dotScale: 0.9

            Component.onCompleted: {
                state = "here"
            }

            state: "below"
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
                        opacity: 0.8
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

                    NumberAnimation {
                        target: spinner
                        properties: "opacity, scale"
                        duration: 600
                        easing.type: Easing.OutQuad
                    }

                    onRunningChanged: {
                        if (!running) {
                            switch (spinner.state) {
                            case "above":
                                spinnerLoader.active = false
                                break
                            case "here":
                                //if(headerLoader.status === Loader.Ready && nodeLoader.status === Loader.Ready) {
                                spinner.state = "above"
                                //treeWrapper.state = "here"
                                //}
                                break
                            default:
                                break
                            }
                        }
                    }
                }
            ]
        }
    }

    // Tree Root
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

        NodeDelegate {
            anchors.fill: parent
        }
    }

    Component {
        id: headerDelegate

        HeaderDelegate {}
    }

    Item {
        id: nodeWrapper
        anchors.fill: parent

        property var ready: appCore && appCore.treeModel !== null
        onReadyChanged: {
            if (ready) {
                var incubator = recursiveDelegate.incubateObject(nodeWrapper, {
                                                                     model: appCore.treeModel,
                                                                     visualDelegate: nodeDelegate,
                                                                     boundsProperty: "contentBounds",
                                                                     childBoundsProperty: "nodeBounds",
                                                                     clipChildren: true
                                                                 })
            }
        }
    }

    Item {
        id: headerWrapper
        anchors.fill: parent

        property var ready: appCore && appCore.treeModel !== null
        onReadyChanged: {
            if (ready) {
                var incubator = recursiveDelegate.incubateObject(headerWrapper,
                                                                 {
                                                                     "model": appCore.treeModel,
                                                                     "visualDelegate": headerDelegate,
                                                                     "boundsProperty": "bounds"
                                                                 })
            }
        }
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

                    property var ready: appCore && appCore.treeModel !== null
                    onReadyChanged: {
                        if (ready) {
                            var incubator = recursiveDelegate.incubateObject(
                                        this, {
                                            "model": appCore.treeModel,
                                            "visualDelegate": boundsRectDelegate,
                                            "boundsProperty": "bounds"
                                        })
                        }
                    }
                }

                Item {
                    id: contentBoundsWrapper
                    anchors.fill: parent

                    property var ready: appCore && appCore.treeModel !== null
                    onReadyChanged: {
                        if (ready) {
                            var incubator = recursiveDelegate.incubateObject(
                                        this, {
                                            "model": appCore.treeModel,
                                            "visualDelegate": contentBoundsRectDelegate,
                                            "boundsProperty": "contentBounds"
                                        })
                        }
                    }
                }

                Item {
                    id: clippedBoundsWrapper
                    anchors.fill: parent

                    property var ready: appCore && appCore.treeModel !== null
                    onReadyChanged: {
                        if (ready) {
                            var incubator = recursiveDelegate.incubateObject(
                                        this, {
                                            "model": appCore.treeModel,
                                            "visualDelegate": headerBoundsRectDelegate,
                                            "boundsProperty": "clippedBounds"
                                        })
                        }
                    }
                }
            }
        }
    }


    /*
        Component {
            id: headerDelegate
            RecursiveDelegate {
                animationDuration: 300
                visualDelegate: HeaderDelegate {}
                recursiveDelegate: headerDelegate
            }
        }

        Loader {
            id: headerLoader
            active: hasAppCore && appCore.treeModel !== null

            anchors.fill: parent

            sourceComponent: headerDelegate

            onLoaded: {
                item.model = appCore.treeModel
            }
        }

        state: "below"
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
                    duration: 600
                    easing.type: Easing.OutQuad
                }
            }
        ]
    }
*/
}
