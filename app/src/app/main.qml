import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import esplamp 1.0

ApplicationWindow {
    visible: true
    GridLayout {
        rows: 7
        flow: GridLayout.TopToBottom
        Text {
            id: logger
            text: DeviceDiscovery.deviceAddressStr
        }
        Button {
            text: "Find Lamp"
            onClicked: DeviceDiscovery.requestAddress()
        }
        Text {
            text: RGBController.text
        }
        TextInput {
            id: inputField
            width: 200
            text: "#000000"
            font.pixelSize: 18
        }
        Button {
            text: "Set Color"
            onClicked: RGBController.changeText(inputField.text)
        }
        // Timer {
        //     interval: 500
        //     running: RGController.lampAddressFound;
        //     repeat: true;
        //     onTriggered: RGBController.getLampAddress()
        // }
    }
    // Image {
    //     id: triangle
    //     source: "qrc:/qt/qml/esplamp/assets/images/logo_scaled.png"
    // } 
}
