#include "gui/signalmanager.hpp"

SignalManager &SignalManager::instance()
{
    static SignalManager instance;
    return instance;
}

SignalManager::SignalManager(QObject *parent) : QObject(parent)
{

}
