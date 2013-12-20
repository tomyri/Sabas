import QtQuick 2.0
import Sailfish.Silica 1.0

Dialog {
    property alias minutes: slider.value
    Column {
        spacing: 10
        anchors.fill: parent
        DialogHeader {
            acceptText: qsTr("Start")
        }
        Slider {
            id: slider
            width: parent.width
            label: qsTr("Stop playing after %1 minutes").arg(value)
            minimumValue: 1
            maximumValue: 120
            stepSize: 1
        }
    }
}
