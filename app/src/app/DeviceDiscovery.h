#ifndef DEVICE_DISCOVERY_H
#define DEVICE_DISCOVERY_H

#include <QtCore/QObject>
#include <QQmlEngine>
#include <qscopedpointer.h>
#include <qstringview.h>
#include <qudpsocket.h>
#include <stdint.h>
#include <qhostaddress.h>
#include <qtmetamacros.h>
#include <QHostAddress>
#include <QNetworkInterface>

class DeviceDiscovery : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QHostAddress deviceAddress READ deviceAddress WRITE setDeviceAddress NOTIFY deviceAddressChanged)
    Q_PROPERTY(QString deviceAddressStr READ deviceAddressStr NOTIFY deviceAddressChanged)
public:
    explicit DeviceDiscovery(QObject *parent = nullptr) : 
        QObject(parent),
        m_deviceAddress(QHostAddress::Null) { 
        setupDeviceDiscoveryServer();
    }

    QString deviceAddressStr() const;
    QHostAddress deviceAddress() const;
    void setDeviceAddress(const QHostAddress&);

    void processPendingDatagrams();

    Q_INVOKABLE void requestAddress();
    signals:
        void deviceAddressChanged();
private:
    void setupDeviceDiscoveryServer();
    QHostAddress m_deviceAddress;

    QUdpSocket* udpSocket = nullptr;

    static const quint32 port = 12345;
    const QByteArray discoveryRequestContent = QByteArrayLiteral("whatstheesplampipagain?");
    const QByteArray discoveryResponseContent = QByteArrayLiteral("openupitsme");
};

#endif
