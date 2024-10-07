#ifndef RGBCONTROLLER_H
#define RGBCONTROLLER_H

#include <QObject>
#include <QQmlEngine>
#include <qtmetamacros.h>

class RGBController : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
public:
    explicit RGBController(QObject *parent = nullptr) : QObject(parent), m_text("#000000") {}

    QString text() const { return m_text; }

    void setText(const QString &newText);

    Q_INVOKABLE void changeText(const QString& newText);

    signals:
        void textChanged();
private:
    QString m_text;
};

#endif
