#include <QApplication>
#include <QMessageBox>
#include <QPushButton>
#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char** argv) {

    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(u"qrc:/esplamp/main.qml"_qs);
    engine.load(url);

    // QMessageBox::StandardButton reply;
    // reply = QMessageBox::question(nullptr, "Test", "Quit?", QMessageBox::Yes|QMessageBox::No);
    // if(reply == QMessageBox::Yes) {
    //     qDebug() << "Yes was clicked";
    //     QApplication::instance()->quit();
    // } else {
    //     qDebug() << "Yes was *not* clicked";
    // }
    //
    // QPushButton button;
    // button.setText("Click here to quit");
    // button.setToolTip("A tooltip");
    // button.show();
    //
    // QObject::connect(&button, SIGNAL (clicked()), QApplication::instance(), SLOT (quit()));

    // engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    return app.exec();
}

