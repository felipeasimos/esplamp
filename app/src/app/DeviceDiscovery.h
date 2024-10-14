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
    QML_SINGLETON
    Q_PROPERTY(QString deviceAddressStr READ deviceAddressStr NOTIFY deviceAddressChanged)
public:
    explicit DeviceDiscovery(QObject *parent = nullptr) : 
        QObject(parent) { 
        setupDeviceDiscoveryServer();
    }

    QString deviceAddressStr() const;
    void processPendingDatagrams();

    Q_INVOKABLE void requestAddress();
    signals:
        void deviceAddressChanged();
private:
    void setupDeviceDiscoveryServer();
    QHostAddress deviceAddress;

    QUdpSocket* udpSocket = nullptr;

    static const quint32 port = 12345;
    const QByteArray discoveryRequestContent = QByteArrayLiteral("whatstheesplampipagain?");
    const QByteArray discoveryResponseContent = QByteArrayLiteral("openupitsme");
};

#endif
