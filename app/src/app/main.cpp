#include <QApplication>
#include <QMessageBox>
#include <QPushButton>
// #include <QQmlApplicationEngine>
// #include <QDir>
// #include <QQuickView>

int main(int argc, char** argv) {

    QApplication app(argc, argv);
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(nullptr, "Test", "Quit?", QMessageBox::Yes|QMessageBox::No);
    if(reply == QMessageBox::Yes) {
        qDebug() << "Yes was clicked";
        QApplication::instance()->quit();
    } else {
        qDebug() << "Yes was *not* clicked";
    }

    QPushButton button;
    button.setText("Click here to quit");
    button.setToolTip("A tooltip");
    button.show();

    QObject::connect(&button, SIGNAL (clicked()), QApplication::instance(), SLOT (quit()));

    // QQmlApplicationEngine engine;
    // engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    return app.exec();
}

