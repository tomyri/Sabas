import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.sabas.sabasplugin 1.0

Dialog {
    property int mediaIndex
    property var book
    id: dialog
    DialogHeader {
        acceptText: qsTr("Play selected")
    }
    SilicaListView {
        id: listView
        model: book.mediaCount()
        anchors.fill: parent
        focus: true
        delegate: ListItem {
            id: delegate
            width: parent.width
            height: Theme.itemSizeSmall
            Label {
                id: indexLabel
                text: "%1: ".arg(index + 1)
                color: mediaIndex == index ? Theme.highlightColor : Theme.primaryColor
            }

            Label {
                text: "%1".arg(book.mediaToDisplayStringAt(index))
                color: mediaIndex == index ? Theme.highlightColor : Theme.primaryColor
                width: parent.width
                elide: Text.ElideLeft
                font.pixelSize: Theme.fontSizeSmall
                anchors.left: indexLabel.right
                anchors.right: parent.right
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
