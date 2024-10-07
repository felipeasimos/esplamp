#include <QObject>
#include <QQmlEngine>

class RGBController : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
public:
    Q_INVOKABLE void doStuff();
};

