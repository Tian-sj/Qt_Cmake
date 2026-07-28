#include "qwindowkit_window_agent.hpp"

namespace cppproject::qml {

QWindowKitWindowAgent::QWindowKitWindowAgent(QObject* parent) : QObject{parent} {}

bool QWindowKitWindowAgent::setup(QQuickWindow* window) {
    return agent_.setup(window);
}

void QWindowKitWindowAgent::setTitleBar(QQuickItem* item) {
    agent_.setTitleBar(item);
}

void QWindowKitWindowAgent::setSystemButton(const SystemButton button, QQuickItem* item) {
    agent_.setSystemButton(static_cast<QWK::WindowAgentBase::SystemButton>(button), item);
}

} // namespace cppproject::qml
