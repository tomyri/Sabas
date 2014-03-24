import QtQuick 2.0
import Sailfish.Silica 1.0

Dialog {
    property alias label: label.text
    property alias text: textField.text
    Column {
        spacing: 10
        anchors.fill: parent
        DialogHeader {
            acceptText: qsTr("Save")
        }
        Label {
            id: label
            x: Theme.paddingLarge
        }
        TextField {
            id: textField
            width: parent.width
        }
    }
    canAccept: textField.text !== ""
}
