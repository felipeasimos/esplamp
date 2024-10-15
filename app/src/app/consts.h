#ifndef ESPLAMP_CONSTS_H
#define ESPLAMP_CONSTS_H

#include <QByteArray>
#include <qtypes.h>

const quint32 lampDiscoveryPort = 12345;
const quint32 lampAPIPort = 80;
const QByteArray discoveryRequestContent = QByteArrayLiteral("whatstheesplampipagain?");
const QByteArray discoveryResponseContent = QByteArrayLiteral("openupitsme");

#endif
