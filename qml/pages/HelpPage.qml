import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: page
    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height
        Column {
            id: column
            anchors {
                right: parent.right
                rightMargin: Theme.paddingLarge
                left: parent.left
                leftMargin: Theme.paddingLarge
            }
            spacing: Theme.paddingLarge
            PageHeader {
                title: qsTr("Help")
            }
            Label {
                width: parent.width
                color: Theme.secondaryHighlightColor
                font.pixelSize: Theme.fontSizeLarge
                text: qsTr("Usage")
            }
            Label {
                wrapMode: Text.WordWrap
                width: parent.width
                text: qsTr("Books are searched from 'Audiobooks' and 'Documents/Audiobooks' folders under user's home directory. Each folder under those paths is treat as a book. Each book-folder can contain multiple folders (CD 01, CD 02, etc). If there is an image, that is used as book cover.")
            }
            Label {
                wrapMode: Text.WordWrap
                width: parent.width
                text: qsTr("Playback position of each book is saved. If you want to start it from beginning, tab and hold that book and select that option from context menu. There is also an option for renameing the book or downloading a cover.")
            }
        }
    }
}
