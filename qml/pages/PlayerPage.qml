import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.sabas.sabasplugin 1.0

Page {
    function prettyTimeFromMsec(msec) {
        return ("%1:%2").arg(parseInt(msec / 1000 / 60)).arg(("0" + parseInt(msec  / 1000 % 60)).slice(-2))
    }
    id: page
    property var book
    SilicaFlickable {
        anchors.fill: parent
        PullDownMenu {
            MenuItem {
                text: qsTr("Stop sleep timer")
                onClicked: {
                    SabasLibrary.stopSleepTimer()
                }
                visible: SabasLibrary.sleepTimerActive
            }
            MenuItem {
                text: qsTr("Sleep timer")
                onClicked: {
                    var dialog = pageStack.push("SleepTimeDialog.qml", {"minutes":1})
                    dialog.accepted.connect(function() {
                        SabasLibrary.startSleepTimer(dialog.minutes)
                    })
                }
            }
            MenuItem {
                text: qsTr("Select track")
                onClicked: {
                    var dialog = pageStack.push("SelectTrackDialog.qml", {"book":book})
                    dialog.accepted.connect(function() {
                        book.currentIndex = dialog.mediaIndex
                    })
                }
            }
        }
        contentHeight: column.height
        Column {
            id: column
            width: page.width
            spacing: Theme.paddingLarge
            PageHeader {
                title: book.name
            }
            Image {
                id: img
                source: book.coverPath === "" ? "qrc:/img/nocover.png" : book.coverPath
                fillMode: Image.PreserveAspectFit
                height: 500
                anchors.horizontalCenter: parent.horizontalCenter
                onStatusChanged: {
                    if (status === Image.Error)
                        source = "qrc:/img/nocover.png"
                }
            }
            Slider {
                minimumValue: 0
                maximumValue: SabasLibrary.trackDuration
                value: SabasLibrary.trackPosition
                width: parent.width
                valueText: prettyTimeFromMsec(value)
                label: qsTr("Track: %1, Duration: %2").arg(book.currentIndex + 1).arg(prettyTimeFromMsec(SabasLibrary.trackDuration))
                onPressedChanged: {
                    if (!pressed) {
                        SabasLibrary.trackPosition = value
                        value  = Qt.binding(function() {return SabasLibrary.trackPosition});
                    }
                }
            }
            Row {
                id: buttonRow
                spacing: Theme.paddingMedium
                anchors.horizontalCenter: parent.horizontalCenter
                IconButton {
                    icon.source: "image://theme/icon-m-previous"
                    onClicked: book.previous()
                }
                IconButton {
                    icon.source: "image://theme/icon-l-left"
                    onClicked: SabasLibrary.skip(-10000)
                }
                IconButton {
                    icon.source: SabasLibrary.playing ? "image://theme/icon-l-pause" : "image://theme/icon-l-play"
                    onClicked: SabasLibrary.toggle()
                }
                IconButton {
                    icon.source: "image://theme/icon-l-right"
                    onClicked: SabasLibrary.skip(10000)
                }
                IconButton {
                    icon.source: "image://theme/icon-m-next"
                    onClicked: book.next()
                }
            }
        }
    }
    Component.onDestruction: SabasLibrary.stop()
    Component.onCompleted: SabasLibrary.play(book)
}
