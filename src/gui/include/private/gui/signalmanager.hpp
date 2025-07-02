/*!
 * @file          signalmanager.hpp
 * @brief         Global Signal Manager
 * @author        Tian_sj
 * @date          2024-12-18
 */
#pragma once

#include <QObject>

class SignalManager : public QObject {
    Q_OBJECT

public:
    static SignalManager &instance();

signals:
    void language_changed();

private:
    explicit SignalManager(QObject *parent = nullptr);
    Q_DISABLE_COPY(SignalManager)
};
