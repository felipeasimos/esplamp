#include <DeviceDiscovery.h>
#include <qnamespace.h>
#include <qudpsocket.h>
#include <QObject>
#include <QNetworkDatagram>
#include "./consts.h"

QString DeviceDiscovery::deviceAddressStr() const {
    return m_deviceAddress.toString();
}

QHostAddress DeviceDiscovery::deviceAddress() const {
    return m_deviceAddress;
}

void DeviceDiscovery::setDeviceAddress(const QHostAddress& deviceAddress) {
    if (m_deviceAddress != deviceAddress) {
        m_deviceAddress = deviceAddress;
        emit deviceAddressChanged();
    }
}

void DeviceDiscovery::requestAddress() {
    if(!udpSocket) {
        udpSocket = new QUdpSocket{this};
    }
    udpSocket->writeDatagram(
        discoveryRequestContent,
        QHostAddress::Broadcast,
        lampDiscoveryPort
    );

}

void DeviceDiscovery::setupDeviceDiscoveryServer() {
    if(!udpSocket) {
        udpSocket = new QUdpSocket{this};
    }
    udpSocket->bind(QHostAddress::Any, lampDiscoveryPort);
    connect(
        udpSocket,
        &QUdpSocket::readyRead,
        this,
        &DeviceDiscovery::processPendingDatagrams
    );
}

void DeviceDiscovery::processPendingDatagrams() {
    while(udpSocket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = udpSocket->receiveDatagram();
        if (datagram.data() == discoveryResponseContent) {
            setDeviceAddress(datagram.senderAddress());
        }
    }
}
