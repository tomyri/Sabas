import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.sabas.sabasplugin 1.0

Dialog {
    property alias dir: folderSelector.currentDir
    id: dialog
    DialogHeader {
        id: header
        acceptText: qsTr("Use this path")
    }
    FolderSelector {
        id: folderSelector
    }
    Label {
        id: currentDirLabel
        text: qsTr("Path: ")
        elide: Text.ElideLeft
        anchors.top: header.bottom
        font.pixelSize: Theme.fontSizeSmall
    }
    Label {
        text: dialog.dir
        width: parent.width
        elide: Text.ElideLeft
        font.pixelSize: Theme.fontSizeSmall
        anchors {
            left: currentDirLabel.right
            right: parent.right
            top: header.bottom
        }
    }
    SilicaListView {
        id: listView
        model: folderSelector.folders
        width: parent.width
        anchors {
            bottom: parent.bottom
            top: currentDirLabel.bottom
        }
        focus: true
        boundsBehavior: Flickable.DragOverBounds
        clip: true
        delegate: ListItem {
            id: delegate
            width: parent.width
            height: Theme.itemSizeSmall
            Label {
                text: "%1".arg(modelData)
                width: parent.width
                font.pixelSize: Theme.fontSizeSmall
                anchors.verticalCenter: parent.verticalCenter
            }
            onClicked: {
                folderSelector.goTo(modelData)
            }
        }
    }
}
