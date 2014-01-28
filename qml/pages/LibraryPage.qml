import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.sabas.sabasplugin 1.0

Page {
    id: page
    SilicaListView {
        PullDownMenu {
//            MenuItem {
//                text: qsTr("Change library path")
//                onClicked: {
//                    var dialog = pageStack.push("RenameDialog.qml", {"label": qsTr("Library Path"),"text" : SabasLibrary.libraryRootPath})
//                    dialog.accepted.connect(function() {
//                        SabasLibrary.libraryRootPath = dialog.text
//                    })
//                }
//            }
            MenuItem {
                text: qsTr("Help")
                onClicked: pageStack.push("HelpPage.qml")
            }
            MenuItem {
                id: missingCoverSearch
                text: qsTr("Find missing covers")
                enabled: false
                visible: SabasLibrary.isCoverSearchEnabled()
                onClicked: {
                    enabled  = false
                    SabasLibrary.searchMissingCovers()
                }
            }
        }
        id: listView
        model: SabasLibrary.books
        anchors.fill: parent
        header: PageHeader {
            title: qsTr("Library")
        }
        delegate: ListItem {
            id: delegate
            width: parent.width
            anchors.leftMargin: Theme.paddingLarge
            anchors.rightMargin: Theme.paddingLarge
            menu: ContextMenu {
                MenuItem {
                    text: qsTr("Start from beginning")
                    onClicked: {
                        remorse.execute(delegate, qsTr("Starting from beginning"), function() {
                            var book = SabasLibrary.at(index)
                            SabasLibrary.play(book, true)
                            pageStack.push("PlayerPage.qml", {"book":book})
                        })
                    }
                    RemorseItem {
                        id: remorse
                    }
                }
                MenuItem {
                    text: qsTr("Rename")
                    onClicked: {
                        var book = SabasLibrary.at(index)
                        var dialog = pageStack.push("RenameDialog.qml", {"label": qsTr("Book name"),"text" : book.name})
                        dialog.accepted.connect(function() {
                            book.name = dialog.text
                        })
                    }
                }
                MenuItem {
                    text: qsTr("Cover search")
                    onClicked: {
                        var book = SabasLibrary.at(index)
                        pageStack.push("SelectCoverPage.qml", {"book":book})
                    }
                    visible: SabasLibrary.isCoverSearchEnabled()
                }
                MenuItem {
                    text: qsTr("Rescan media")
                    onClicked: {
                        var book = SabasLibrary.at(index)
                        book.relocateMedia()
                    }
                }
                Component.onCompleted: {
                    if (SabasLibrary.at(index).coverPath === "") {
                        missingCoverSearch.enabled = true
                        console.debug("Cover missing")
                    }
                }
            }
            Row {
                id: bookLabel
                spacing: 10
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width
                Image {
                    id: cover
                    source: SabasLibrary.at(index).coverPath
                    fillMode: Image.PreserveAspectFit
                    height: parent.height
                    width: 50
                }
                Column {
                    anchors.verticalCenter: parent.verticalCenter
                    width: parent.width - cover.width
                    Label {
                        text: modelData
                        color: delegate.highlighted ? Theme.highlightColor : Theme.primaryColor
                        elide: Text.ElideRight
                        width: parent.width
                    }
                    Label {
                        text: qsTr("Current track: %1/%2").arg(SabasLibrary.at(index).currentIndex + 1).arg(SabasLibrary.at(index).trackCount)
                        color: delegate.highlighted ? Theme.highlightColor : Theme.primaryColor
                        font.pixelSize: Theme.fontSizeTiny
                        width: parent.width
                    }
                }
            }

            onClicked: {
                var book = SabasLibrary.at(index)
                pageStack.push("PlayerPage.qml", {"book":book})
            }
        }
    }
}
