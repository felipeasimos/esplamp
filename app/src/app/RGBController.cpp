#include "./RGBController.h"
#include <QDebug>
#include <qhostaddress.h>
#include <qudpsocket.h>
#include <QtNetwork/QUdpSocket>
#include <QObject>
#include <QtNetwork/QNetworkDatagram>

void RGBController::setText(const QString& newText) {
    if (m_text != newText) {
        m_text = newText;
        qDebug() << "Hello World!";
        emit textChanged();
    }
}

void RGBController::setLampAddressFound(const bool found) {
    if(!m_lampAddressFound) {
        m_lampAddressFound = found;
        emit lampAddressFoundChanged();
    }
}

void RGBController::requestLampAddress() {
    if (this->lampAddressFound()) {
        return;
    }
    this->udpSocket = new QUdpSocket(this);
    this->udpSocket->bind(QHostAddress::AnyIPv4, this->lampPort);
    this->udpSocket->writeDatagram(
        this->discoveryRequestContent,
        QHostAddress::Broadcast,
        this->lampPort
    );
    qDebug() << "datagram sent\n";
    qDebug() << "socket binded\n";

    QObject::connect(
        udpSocket,
        &QUdpSocket::readyRead,
        this,
        [=]() {
            while(udpSocket->hasPendingDatagrams()) {
                QNetworkDatagram datagram = udpSocket->receiveDatagram();
                if (datagram.data() == this->discoveryResponseContent) {
                    this->lampAddress = datagram.senderAddress();
                    this->setLampAddressFound(true);
                    qDebug() << this->lampAddress;
                }
            }
            qDebug() << "stopped listening\n";
        }
    );

    qDebug() << "waiting for response\n";
}

void RGBController::changeText(const QString& newText) {
    setText(newText);
}
