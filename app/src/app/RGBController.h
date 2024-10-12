#ifndef RGBCONTROLLER_H
#define RGBCONTROLLER_H

#include <QObject>
#include <QQmlEngine>
#include <qstringview.h>
#include <qhostaddress.h>
#include <qtmetamacros.h>
#include <QHostAddress>
#include <QNetworkInterface>
#include <qudpsocket.h>

class RGBController : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(bool lampAddressFound READ lampAddressFound WRITE setLampAddressFound NOTIFY lampAddressFoundChanged)
public:
    explicit RGBController(QObject *parent = nullptr) : 
        QObject(parent),
        m_text("#000000"),
        m_lampAddressFound(false) { }


    QString text() const { return m_text; }
    bool lampAddressFound() const { return m_lampAddressFound; }

    void setLampAddressFound(const bool);
    void setText(const QString &newText);

    Q_INVOKABLE void changeText(const QString& newText);
    Q_INVOKABLE void requestLampAddress();

    void processUdpDatagram();

    signals:
        void textChanged();
        void lampAddressFoundChanged();
private:
    QString m_text;
    bool m_lampAddressFound;
    QHostAddress lampAddress;
    QUdpSocket* udpSocket;
    static const quint32 lampPort = 12345;
    const QByteArray discoveryRequestContent = QByteArrayLiteral("whatstheesplampipagain?");
    const QByteArray discoveryResponseContent = QByteArrayLiteral("openupitsme");
};

#endif
