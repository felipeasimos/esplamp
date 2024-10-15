#include <DeviceDiscovery.h>
#include <qnamespace.h>
#include <qudpsocket.h>
#include <QObject>
#include <QNetworkDatagram>

QString DeviceDiscovery::deviceAddressStr() const {
    if(deviceAddress == QHostAddress::Null) {
        return "";
    }
    return deviceAddress.toString();
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
            this->deviceAddress = datagram.senderAddress();
            emit deviceAddressChanged();
        }
    }
}
