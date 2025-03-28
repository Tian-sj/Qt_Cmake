#include "gui/signalmanager.h"

SignalManager &SignalManager::instance()
{
    static SignalManager instance;
    return instance;
}

SignalManager::SignalManager(QObject *parent) : QObject(parent)
{

}
