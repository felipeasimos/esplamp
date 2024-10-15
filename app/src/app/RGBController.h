#ifndef RGBCONTROLLER_H
#define RGBCONTROLLER_H

#include <QObject>
#include <QQmlEngine>
#include <qcolor.h>
#include <qstringview.h>
#include <qhostaddress.h>
#include <qtmetamacros.h>
#include <QHostAddress>
#include <QNetworkInterface>
#include <qudpsocket.h>

class RGBController : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QString colorStr READ colorStr NOTIFY colorChanged)
public:
    explicit RGBController(QObject *parent = nullptr) : 
        QObject(parent) { }

    QString colorStr() const;
    void setColor(const QColor&);
    void setupColor();

    Q_INVOKABLE void changeColor(const QString& newText);

    void processUdpDatagram();

    signals:
        void colorChanged();
private:
    QColor color{0,0,0};
};

#endif
