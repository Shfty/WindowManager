import QtQuick 2.11
import QtQuick.Layouts 1.3
import ManagedWindow 1.0

ManagedWindow {
    id: managedWindow

    thumbnailOpacity: {
        var candidate = this;
        var opacity = 1.0
        while(candidate.parent !== null)
        {
            candidate = candidate.parent
            opacity *= candidate.opacity
        }
        return opacity
    }
}
