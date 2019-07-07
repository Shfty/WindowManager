import QtQuick 2.12

Item {
    id: clippingRect
    clip: true

    readonly property alias contentItem: childContainer
    default property alias contents: childContainer.children

    Item {
        id: childContainer
        x: -clippingRect.x
        y: -clippingRect.y
        width: clippingRect.parent.width
        height: clippingRect.parent.height
    }
}
