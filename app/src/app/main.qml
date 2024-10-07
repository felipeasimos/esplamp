import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import esplamp 1.0

ApplicationWindow {
    visible: true
    RowLayout {
        Text {
            text: RGBController.text
        }
        TextInput {
            id: inputField
            width: 200
            text: "Enter new text"
            font.pixelSize: 18
        }
        Button {
            text: "Connect"
            onClicked: RGBController.changeText(inputField.text)
        }
    }
    // Image {
    //     id: triangle
    //     source: "qrc:/qt/qml/esplamp/assets/images/logo_scaled.png"
    // } 
}
