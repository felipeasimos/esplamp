#include <DeviceDiscovery.h>
#include <qnamespace.h>
#include <qudpsocket.h>
#include <QObject>
#include <QNetworkDatagram>

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
        this->discoveryRequestContent,
        QHostAddress::Broadcast,
        this->port
    );

}

void DeviceDiscovery::setupDeviceDiscoveryServer() {
    if(!udpSocket) {
        udpSocket = new QUdpSocket{this};
    }
    udpSocket->bind(QHostAddress::Any, this->port);
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
        if (datagram.data() == this->discoveryResponseContent) {
            setDeviceAddress(datagram.senderAddress());
        }
    }
}
