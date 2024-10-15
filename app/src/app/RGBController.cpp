#include "./RGBController.h"
#include <QDebug>
#include <qhostaddress.h>
#include <qudpsocket.h>
#include <QtNetwork/QUdpSocket>
#include <QObject>
#include <QtNetwork/QNetworkDatagram>
#include <QString>


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

void RGBController::setColor(const QColor& newColor) {
    if (color != newColor) {
        color = newColor;
        emit colorChanged();
    }
}

void RGBController::changeColor(const QString& newColorStr) {
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
