#include "./RGBController.h"
#include <QDebug>
#include <QObject>
#include <qhostaddress.h>
#include <qudpsocket.h>
#include <QtNetwork/QUdpSocket>
#include <QObject>
#include <QtNetwork/QNetworkDatagram>
#include <QString>
#include "./consts.h"


QString toZeroPaddedStr(quint32 n, quint32 pad = 2, quint32 base = 16) {
    return QStringLiteral("%1")
        .arg(n, pad, base, QLatin1Char('0'));
}

QString RGBController::colorStr() const {
    return QStringLiteral("#") +
    toZeroPaddedStr(color.red()) +
    toZeroPaddedStr(color.green()) + 
    toZeroPaddedStr(color.blue());
}

QHostAddress RGBController::lampAddress() const {
    return m_lampAddress;
}

void RGBController::setColor(const QColor& newColor) {
    qDebug() << "setColor";
    if (color != newColor) {
        color = newColor;
        colorIsDirty = true;
        requestColor();
        emit colorChanged();
    } else if(colorIsDirty) {
        requestColor();
    }
}

void RGBController::setLampAddress(const QHostAddress& lampAddress) {
    if (m_lampAddress != lampAddress) {
        m_lampAddress = lampAddress;
        setupTcpSocket();
        colorIsDirty = true;
        requestColor();
        emit lampAddressChanged();
    }
}

void RGBController::setupTcpSocket() {
    if(tcpSocket) tcpSocket->deleteLater();
    tcpSocket = new QTcpSocket{this};
    connect(tcpSocket, &QTcpSocket::connected, this, &RGBController::onConnected);
    connect(tcpSocket, &QTcpSocket::readyRead, this, &RGBController::onReadyRead);
    connect(tcpSocket, &QTcpSocket::disconnected, this, &RGBController::onDisconnected);
    connect(tcpSocket, &QTcpSocket::errorOccurred, this, &RGBController::onErrorOccurred);
}

void RGBController::requestColor() {
    qDebug() << "requestColor\n";
    m_lampAddress = QHostAddress("192.168.0.100");
    setupTcpSocket();
    qDebug() << m_lampAddress;
    if (m_lampAddress == QHostAddress::Null) return;
    if (tcpSocket->state() != QAbstractSocket::ConnectedState) {
        tcpSocket->connectToHost(m_lampAddress, lampAPIPort);
    }
}

void RGBController::onConnected() {
    QByteArray request{"\x06P\x00\x00\x00\x00", 6};
    request[2] = 100;
    request[3] = color.red();
    request[4] = color.green();
    request[5] = color.blue();
    tcpSocket->write(request);
    qDebug() << "connected\n";
}

void RGBController::onReadyRead() {
    QByteArray response = tcpSocket->readAll();
    bool success = response[1] == 1;
    qDebug() << "response received!";
    qDebug() << response;
    if(success) {
        colorIsDirty = false;
    } else {
        qDebug() << "color update failed";
    }
    tcpSocket->disconnect();
}

void RGBController::onDisconnected() {
    qDebug() << "Error occurred on disconnect";
}

void RGBController::onErrorOccurred() {
    qDebug() << "Error occurred on TCP socket";
    qDebug() << tcpSocket->errorString();
}

void RGBController::changeColor(const QString& newColorStr) {
    qDebug() << "changeColor";
    if(!newColorStr.startsWith("#") || newColorStr.trimmed().length() != 7) return;

    bool valid;
    quint32 red = newColorStr.sliced(1, 2).toUInt(&valid, 16);
    if(!valid) return;
    quint32 green = newColorStr.sliced(3, 2).toUInt(&valid, 16);
    if(!valid) return;
    quint32 blue = newColorStr.sliced(5, 2).toUInt(&valid, 16);
    if(!valid) return;

    setColor(QColor::fromRgb(red, green, blue));
}
