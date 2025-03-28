/*!
 * @file          signalmanager.h
 * @brief         Global Signal Manager
 * @author        Tian_sj
 * @date          2024-12-18
 */

#ifndef SIGNALMANAGER_H
#define SIGNALMANAGER_H

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

#endif // SIGNALMANAGER_H
