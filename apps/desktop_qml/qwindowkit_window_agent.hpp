#pragma once

#include <QObject>
#include <QQuickItem>
#include <QQuickWindow>
#include <QWKQuick/quickwindowagent.h>
#include <QtQml/qqmlregistration.h>

namespace cppproject::qml {

class QWindowKitWindowAgent : public QObject {
    Q_OBJECT
    QML_NAMED_ELEMENT(WindowAgent)

public:
    enum SystemButton {
        Unknown = QWK::WindowAgentBase::Unknown,
        WindowIcon = QWK::WindowAgentBase::WindowIcon,
        Help = QWK::WindowAgentBase::Help,
        Minimize = QWK::WindowAgentBase::Minimize,
        Maximize = QWK::WindowAgentBase::Maximize,
        Close = QWK::WindowAgentBase::Close,
    };
    Q_ENUM(SystemButton)

    explicit QWindowKitWindowAgent(QObject* parent = nullptr);

    Q_INVOKABLE bool setup(QQuickWindow* window);
    Q_INVOKABLE void setTitleBar(QQuickItem* item);
    Q_INVOKABLE void setSystemButton(SystemButton button, QQuickItem* item);

private:
    QWK::QuickWindowAgent agent_;
};

} // namespace cppproject::qml
