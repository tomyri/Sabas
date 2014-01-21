import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.sabas.sabasplugin 1.0

Dialog {
    property int mediaIndex
    property var book
    id: dialog
    DialogHeader {
        id: header
        acceptText: qsTr("Play selected")
        z: 3
    }
    SilicaListView {
        id: listView
        model: book.mediaCount()
        width: parent.width
        anchors {
            bottom: parent.bottom
            top: header.bottom
        }
        focus: true
        boundsBehavior: Flickable.DragOverBounds
        clip: true
        delegate: ListItem {
            id: delegate
            width: parent.width
            height: Theme.itemSizeSmall
            Label {
                id: indexLabel
                text: "%1: ".arg(index + 1)
                color: mediaIndex == index ? Theme.highlightColor : Theme.primaryColor
                anchors.verticalCenter: parent.verticalCenter
            }

            Label {
                text: "%1".arg(book.mediaToDisplayStringAt(index))
                color: mediaIndex == index ? Theme.highlightColor : Theme.primaryColor
                width: parent.width
                elide: Text.ElideLeft
                font.pixelSize: Theme.fontSizeSmall
                anchors {
                    left: indexLabel.right
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                }
            }
            onClicked: {
                mediaIndex = index
            }
        }
    }
    Component.onCompleted: {
        mediaIndex = SabasLibrary.selectedBook.currentIndex
    }
}
