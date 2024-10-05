#include <QtWidgets/QApplication>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QPushButton>
#include <QtQml/QQmlApplicationEngine>

int main(int argc, char** argv) {

    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(u"qrc:/qt/qml/esplamp/app/main.qml"_qs);
    engine.load(url);


    return app.exec();
}
