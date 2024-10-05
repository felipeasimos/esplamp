import QtQuick
import Qt.labs.folderlistmodel
import QtQuick.Controls

ApplicationWindow {
    visible: true
    Image {
        id: triangle
        source: "qrc:/qt/qml/esplamp/assets/images/logo_scaled.png"
    }
    ListView {
        width: 200; height: 400

        FolderListModel {
            id: folderModel
            folder: "qrc:/"
            nameFilters: ["*"]
        }

        Component {
            id: fileDelegate
            Text { text: fileName }
        }

        model: folderModel
        delegate: fileDelegate
    }
}
