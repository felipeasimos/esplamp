import QtQuick
import QtQuick.Controls
import esplamp 1.0

ApplicationWindow {
    visible: true
    Text {
        text: "Hello world"
    }
    Button {
        text: "Connect"
        onClicked: RGBController.doStuff()
    }
    // Image {
    //     id: triangle
    //     source: "qrc:/qt/qml/esplamp/assets/images/logo_scaled.png"
    // } 
}
