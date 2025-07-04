#pragma once

#include <QObject>

class SignalManagerSingleton : public QObject {
    Q_OBJECT

public:
    static SignalManagerSingleton &getInstance();

signals:
    void languageChanged();

private:
    explicit SignalManagerSingleton(QObject *parent = nullptr);
    Q_DISABLE_COPY(SignalManagerSingleton)
};
