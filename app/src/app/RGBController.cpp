#include "./RGBController.h"
#include <QDebug>

void RGBController::setText(const QString& newText) {
    if (m_text != newText) {
        m_text = newText;
        qDebug() << "Hello World!";
        emit textChanged();
    }
}

void RGBController::changeText(const QString& newText) {
    setText(newText);
}
