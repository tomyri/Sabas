import QtQuick 2.0
import Sailfish.Silica 1.0
import Sabas 1.0

Page {
    id: page
    SilicaListView {
        PullDownMenu {
            MenuItem {
                text: qsTr("Change library path")
                onClicked: {
                    var dialog = pageStack.push("RenameDialog.qml", {"label": qsTr("Library Path"),"text" : SabasLibrary.libraryRootPath})
                    dialog.accepted.connect(function() {
                        SabasLibrary.libraryRootPath = dialog.text
                    })
                }
            }
            MenuItem {
                text: qsTr("Help")
                onClicked: pageStack.push("HelpPage.qml")
            }
        }
        id: listView
        model: SabasLibrary.count()
        anchors.fill: parent
        header: PageHeader {
            title: qsTr("Library")
        }
        delegate: ListItem {
            id: delegate
            x: Theme.paddingLarge
            menu: ContextMenu {
                MenuItem {
                    text: qsTr("Start from beginning")
                    onClicked: {
                        var book = SabasLibrary.at(index)
                        SabasLibrary.play(book, true)
                        pageStack.push("PlayerPage.qml", {"book":book})
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
            }
            Row {
                spacing: 10
                anchors.verticalCenter: parent.verticalCenter
                Image {
                    id: cover
                    source: SabasLibrary.at(index).coverPath
                    fillMode: Image.PreserveAspectFit
                    height: parent.height
                    width: 50
                }
                Label {
                    x: Theme.paddingLarge
                    text: SabasLibrary.at(index).name
                    color: delegate.highlighted ? Theme.highlightColor : Theme.primaryColor
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
            onClicked: {
                var book = SabasLibrary.at(index)
                SabasLibrary.play(book)
                pageStack.push("PlayerPage.qml", {"book":book})
            }
        }
    }
}





