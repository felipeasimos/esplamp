#include <DeviceDiscovery.h>
#include <qudpsocket.h>
#include <QObject>
#include <QNetworkDatagram>

QString DeviceDiscovery::deviceAddressStr() const {
    return deviceAddress.toString();
}

void DeviceDiscovery::requestAddress() {
    udpSocket = new QUdpSocket{this};
    udpSocket->bind(QHostAddress::Any, this->port);
    udpSocket->writeDatagram(
        this->discoveryRequestContent,
        QHostAddress::Broadcast,
        this->port
    );
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
