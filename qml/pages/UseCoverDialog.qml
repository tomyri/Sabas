import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.sabas.sabasplugin 1.0

Dialog {
    property alias url: image.source
    id: dialog
    DialogHeader {
        id: header
        acceptText: qsTr("Use this cover")
        z: 3
    }
    Image {
        id: image
        fillMode: Image.PreserveAspectFit
        height: 500
        width:  500
        anchors {
            centerIn: parent
        }
    }
}
