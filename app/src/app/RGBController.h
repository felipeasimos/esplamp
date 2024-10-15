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
    Q_PROPERTY(QHostAddress lampAddress READ lampAddress WRITE setLampAddress NOTIFY lampAddressChanged)
public:
    explicit RGBController(QObject *parent = nullptr) : 
        QObject(parent),
        m_lampAddress(QHostAddress::Null) { }

    QString colorStr() const;
    QHostAddress lampAddress() const;

    void setColor(const QColor&);
    void setLampAddress(const QHostAddress&);

    Q_INVOKABLE void changeColor(const QString& newText);

    signals:
        void colorChanged();
        void lampAddressChanged();
private:
    QColor color{0,0,0};
    QHostAddress m_lampAddress;
};

#endif
