#include "gui/signal_manager_singleton.hpp"

SignalManagerSingleton &SignalManagerSingleton::getInstance() {
    static SignalManagerSingleton instance;
    return instance;
}

SignalManagerSingleton::SignalManagerSingleton(QObject *parent) : QObject(parent) {}
