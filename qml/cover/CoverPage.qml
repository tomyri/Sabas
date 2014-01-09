import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.sabas.sabasplugin 1.0

CoverBackground {
    Label {
        id: label
        anchors.centerIn: parent
        width: parent.width
        text: SabasLibrary.selectedBook && SabasLibrary.selectedBook.coverPath === "" ? SabasLibrary.selectedBook.name : ""
        font.pixelSize: Theme.fontSizeSmall
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.WrapAnywhere
    }
    Image {
        id: img
        source: SabasLibrary.selectedBook ? SabasLibrary.selectedBook.coverPath : "qrc:/harbour-sabas.png"
        fillMode: SabasLibrary.selectedBook ? Image.PreserveAspectCrop : Image.Pad
        anchors.fill: parent
        onStatusChanged: {
            if (status === Image.Error) {
                label.text = SabasLibrary.selectedBook.name
            }
        }
    }
    CoverActionList {
        id: coverAction
        enabled: SabasLibrary.selectedBook
        CoverAction {
            iconSource: SabasLibrary.isPlaying ? "image://theme/icon-cover-pause" : "image://theme/icon-cover-play"
            onTriggered: SabasLibrary.toggle()
        }
    }
}
