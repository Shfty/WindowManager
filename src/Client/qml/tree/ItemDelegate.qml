import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import ".."

Item {
    id: itemDelegate
    anchors.fill: parent

    readonly property var settingsContainer: clientCore ? clientCore.settingsContainer : null
    readonly property bool hasSettings: settingsContainer ? true : false

    readonly property int itemMargin: hasSettings ? settingsContainer.itemMargin : 0
    readonly property int itemBorder: hasSettings ? settingsContainer.itemBorder : 0
    readonly property int headerSize: hasSettings ? settingsContainer.headerSize : 0
    readonly property int animationDuration: hasSettings ? settingsContainer.itemAnimationDuration : 0
    readonly property int animationCurve: hasSettings ? settingsContainer.itemAnimationCurve : 0

    anchors.margins: itemMargin

    property var modelData: null
    property var parentDelegate: null

    readonly property bool isHorizontal: modelData.flow === "Horizontal"
    readonly property bool isVertical: modelData.flow === "Vertical"

    readonly property bool isSplit: modelData.layout === "Split"
    readonly property bool isTabbed: modelData.layout === "Tabbed"

    readonly property bool isHorizontalSplit: isHorizontal && isSplit
    readonly property bool isVerticalSplit: isVertical && isSplit

    readonly property bool isHorizontalTabbed: isHorizontal && isTabbed
    readonly property bool isVerticalTabbed: isVertical && isTabbed

    readonly property int childCount: modelData.children.length

    property var firstHeader: headerIncubator.incubators.length > 0 ? headerIncubator.incubators[0].object : null
    property real headerHeight: isVerticalSplit ? headerSize : headerContainer.height

    property int animatingChildren: 0
    property int prevAnimatingChildren: 0

    onAnimatingChildrenChanged: {
        print("itemDelegate animatingChildren:", animatingChildren)
        var needsWindowUpdate = false
        needsWindowUpdate |= animatingChildren === 0 && prevAnimatingChildren > 0
        needsWindowUpdate |= animatingChildren === modelData.children.length && prevAnimatingChildren < modelData.children.length

        if(needsWindowUpdate) {
            updateWindows()
        }

        prevAnimatingChildren = animatingChildren;
    }

    function updateWindows()
    {
        updateWindows_internal();
        clientCore.sendMessage(["CommitWindowMove"])
    }

    function updateWindows_internal()
    {
        for(var incubators in nodeIncubator.incubators)
        {
            var instance = nodeIncubator.incubators[incubators]
            if(instance)
            {
                instance.object.updateWindow()
            }
        }
    }

    ClippingRect {
        id: nodeContainer

        anchors.fill: parent
        anchors.margins: -itemMargin + itemBorder
        anchors.topMargin: firstHeader ? (firstHeader.state === "anchored" ? headerHeight : headerSize) : 0

        Flickable {
            id: rowFlickable
            anchors.fill: parent
            anchors.topMargin: headerHeight

            readonly property real scrollX: (width + itemMargin) * modelData.activeIndex
            contentX: isTabbed ? scrollX : 0

            contentWidth: isTabbed ? (itemMargin * (childCount - 1)) + (width * childCount) : width
            contentHeight: height

            visible: isHorizontal

            RowLayout {
                id: nodeRowLayout
                anchors.fill: parent
                spacing: itemMargin

                Repeater {
                    id: nodeRowLayoutRepeater
                    model: itemDelegate.childCount

                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }
                }
            }
        }

        Flickable {
            id: columnFlickable
            anchors.fill: parent
            anchors.topMargin: headerHeight

            readonly property real scrollY: (height + itemMargin) * modelData.activeIndex

            contentWidth: width
            contentHeight: isTabbed ? (itemMargin * (childCount - 1)) + (height * childCount) : height
            contentY: isTabbed ? scrollY : 0

            visible: isVertical

            ColumnLayout {
                id: nodeColumnLayout
                anchors.fill: parent
                spacing: isSplit ? itemMargin + headerSize : itemMargin

                Repeater {
                    id: nodeColumnLayoutRepeater
                    model: itemDelegate.childCount

                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }
                }
            }
        }

        MultiIncubator {
            id: nodeIncubator

            active: itemDelegate.modelData
            model: itemDelegate.modelData ? itemDelegate.modelData.children : null

            sourceComponent: Component {
                NodeDelegate {
                    id: nodeDelegate

                    readonly property var childDelegate: childIncubator.item

                    readonly property int index: modelData ? modelData["index"] : -1

                    readonly property var targetFlickable: isHorizontal ? rowFlickable : columnFlickable
                    readonly property var targetRepeater: isHorizontal ? nodeRowLayoutRepeater : nodeColumnLayoutRepeater
                    readonly property bool repeaterReady: index !== -1 && targetRepeater.count > index
                    readonly property var targetItem: repeaterReady ? targetRepeater.itemAt(modelData.index) : null

                    property rect targetBounds: targetItem ? Qt.rect(targetItem.x - targetFlickable.contentX,
                                                                     targetItem.y - targetFlickable.contentY + headerHeight,
                                                                     targetItem.width,
                                                                     targetItem.height) : Qt.rect(0, 0, 0, 0)

                    property var targetWindowInfo: modelData.windowInfo
                    onTargetWindowInfoChanged: {
                        updateWindow()
                        clientCore.sendMessage(["CommitWindowMove"])
                    }

                    Behavior on targetBounds {
                        PropertyAnimation {
                            id: targetBoundsAnimation
                            duration: itemDelegate.animationDuration
                            easing.type: itemDelegate.animationCurve
                            onRunningChanged: {
                                if(running) {
                                    itemDelegate.animatingChildren++
                                }
                                else {
                                    itemDelegate.animatingChildren--
                                    if(state != "anchored")
                                    {
                                        state = "anchored"
                                    }
                                }
                            }
                        }
                    }

                    function getWindowVisible() {
                        if(targetBoundsAnimation.running) return false

                        var parentIncubator = itemDelegate.parent
                        if(parentIncubator)
                        {
                            var parentNode = parentIncubator.parent
                            if(parentNode && parentNode.getWindowVisible)
                            {
                                var parentVisible = parentNode.getWindowVisible()
                                if(!parentVisible) return false
                            }
                        }

                        if(nodeDelegate.modelData.treeParent.layout === "Tabbed")
                        {
                            return nodeDelegate.modelData.treeParent.activeIndex === nodeDelegate.modelData.index
                        }

                        return true
                    }

                    function updateWindow() {
                        if(nodeDelegate.modelData.windowInfo)
                        {
                            var visible = nodeDelegate.getWindowVisible()
                            var pos
                            if(visible)
                            {
                                pos = mapToGlobal(0, 0)
                            }
                            else
                            {
                                pos = nodeContainer.mapToGlobal(itemMargin, 0)
                            }

                            var rect = Qt.rect(pos.x, pos.y, width, height)
                            clientCore.sendMessage(["MoveWindow", modelData.windowInfo.hwnd, rect, visible])
                        }

                        childIncubator.item.updateWindows_internal()
                    }

                    Connections {
                        target: modelData.treeParent
                        onFlowChanged: nodeDelegate.state = "floating"
                        onLayoutChanged: nodeDelegate.state = "floating"
                        onChildrenChanged: nodeDelegate.state = "floating"
                        onActiveIndexChanged: nodeDelegate.state = "floating"
                    }

                    state: ""
                    //Component.onCompleted: state = "anchored"

                    states: [
                        State {
                            name: "anchored"
                            PropertyChanges {
                                target: nodeDelegate
                                parent: targetItem
                            }
                            AnchorChanges {
                                target: nodeDelegate
                                anchors.top: parent.top
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.bottom: parent.bottom
                            }
                        },
                        State {
                            name: "floating"
                            PropertyChanges {
                                target: nodeDelegate
                                x: targetBounds.x
                                y: targetBounds.y
                                width: targetBounds.width
                                height: targetBounds.height
                            }
                            PropertyChanges {
                                target: nodeDelegate
                                parent: nodeContainer.contentItem
                            }
                        }
                    ]

                    Incubator {
                        id: childIncubator

                        anchors.fill: parent
                        sourceComponent: itemDelegateComponent

                        properties: ({
                            modelData: nodeDelegate.modelData,
                            parentDelegate: itemDelegate
                        })
                    }
                }
            }
        }
    }

    Item {
        id: headerContainer
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        height: modelData.flow === "Horizontal" ? headerRowLayout.height : headerColumnLayout.height

        RowLayout {
            id: headerRowLayout

            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            height: clientCore.settingsContainer.headerSize

            spacing: isSplit ? itemMargin : 0

            Repeater {
                id: headerRowLayoutRepeater
                model: itemDelegate.childCount
                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
            }
        }

        ColumnLayout {
            id: headerColumnLayout

            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right

            property real verticalSplitItemSize: {
                var totalHeight = itemDelegate.height
                totalHeight -= itemMargin * (childCount - 1)
                totalHeight -= headerSize * childCount
                totalHeight /= childCount
                return totalHeight
            }

            height: isSplit ? itemDelegate.height - verticalSplitItemSize : clientCore.settingsContainer.headerSize * childCount
            spacing: isSplit ? verticalSplitItemSize + itemMargin : 0

            Repeater {
                id: headerColumnLayoutRepeater
                model: itemDelegate.childCount

                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
            }
        }

        MultiIncubator {
            id: headerIncubator

            active: itemDelegate.modelData
            model: itemDelegate.modelData ? itemDelegate.modelData.children : null
            sourceComponent: Component {
                HeaderDelegate {
                    id: headerDelegate

                    readonly property int index: modelData ? modelData["index"] : -1

                    readonly property var targetRepeater: isHorizontal ? headerRowLayoutRepeater : headerColumnLayoutRepeater
                    readonly property bool repeaterReady: index !== -1 && targetRepeater.count > index
                    readonly property var targetItem: repeaterReady ? targetRepeater.itemAt(modelData.index) : null
                    property rect targetBounds: targetItem ? Qt.rect(targetItem.x,
                                                                     targetItem.y,
                                                                     targetItem.width,
                                                                     targetItem.height) : Qt.rect(0,0,0,0)

                    Behavior on targetBounds {
                        PropertyAnimation {
                            duration: itemDelegate.animationDuration
                            easing.type: itemDelegate.animationCurve
                            onRunningChanged: {
                                if(!running) {
                                    state = "anchored"
                                }
                            }
                        }
                    }

                    Connections {
                        target: modelData.treeParent
                        onFlowChanged: headerDelegate.state = "floating"
                        onLayoutChanged: headerDelegate.state = "floating"
                        onChildrenChanged: headerDelegate.state = "floating"
                    }

                    state: ""
                    //Component.onCompleted: state = "anchored"

                    states: [
                        State {
                            name: "anchored"
                            PropertyChanges {
                                target: headerDelegate
                                parent: targetItem
                            }
                            AnchorChanges {
                                target: headerDelegate
                                anchors.top: parent.top
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.bottom: parent.bottom
                            }
                        },
                        State {
                            name: "floating"
                            PropertyChanges {
                                target: headerDelegate
                                x: targetBounds.x
                                y: targetBounds.y
                                width: targetBounds.width
                                height: targetBounds.height
                            }
                            PropertyChanges {
                                target: headerDelegate
                                parent: headerContainer
                            }
                        }
                    ]
                }
            }
        }
    }
}