import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import esplamp 1.0

ApplicationWindow {
    visible: true
    id: root
    GridLayout {
        anchors.horizontalCenter: parent.horizontalCenter
        rows: 7
        flow: GridLayout.TopToBottom
        DeviceDiscovery {
            id: deviceDiscovery
        }
        RGBController {
            id: rgbController
            lampAddress: deviceDiscovery.deviceAddress
        }
        Text {
            id: logger
            text: deviceDiscovery.deviceAddressStr
        }
        Button {
            text: "Find Lamp"
            onClicked: deviceDiscovery.requestAddress()
        }
        Text {
            text: rgbController.colorStr
        }
        TextInput {
            id: inputField
            width: 200
            text: "#000000"
            font.pixelSize: 18
        }
        Button {
            text: "Set Color"
            onClicked: rgbController.changeColor(inputField.text)
        }
    }
}
