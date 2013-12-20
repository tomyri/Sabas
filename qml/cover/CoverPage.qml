import QtQuick 2.0
import Sailfish.Silica 1.0
import Sabas 1.0

CoverBackground {
    Label {
        id: label
        anchors.centerIn: parent
        width: parent.width
        text: SabasLibrary.selectedBook.coverPath === "" ? SabasLibrary.selectedBook.name : ""
        font.pixelSize: Theme.fontSizeSmall
        wrapMode: Text.WrapAnywhere
    }
    Image {
        id: img
        source: SabasLibrary.selectedBook.coverPath
        fillMode: Image.PreserveAspectCrop
        anchors.fill: parent
        z: -2
    }
    CoverActionList {
        id: coverAction

        CoverAction {
            iconSource: "image://theme/icon-cover-pause"
            onTriggered: SabasLibrary.toggle()
        }
    }
}


