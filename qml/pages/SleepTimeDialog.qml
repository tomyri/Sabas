import QtQuick 2.0
import Sailfish.Silica 1.0

Dialog {
    id: sleepDialog
    canAccept: !(picker.hour==0 && picker.minute==0)

    property int minutes: 1

    Column {
        spacing: 10
        anchors.fill: parent
        DialogHeader {
            acceptText: qsTr("Start")
        }

        Label {
            text: qsTr("Stop playing after %1 minutes").arg(minutes)
            anchors {
                left: parent.left
                right: parent.right
                margins: Theme.paddingLarge
            }
        }

        TimePicker {
            id: picker
            anchors.horizontalCenter: parent.horizontalCenter
            hourMode: DateTime.TwentyFourHours
            minute: 1

            onTimeChanged: {
                minutes = time.getHours()*60 + time.getMinutes();
            }
        }
    }
}
