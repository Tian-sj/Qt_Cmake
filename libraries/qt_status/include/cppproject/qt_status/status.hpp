#pragma once

#include "cppproject/qt_status/export.hpp"

#include <QObject>
#include <QString>

namespace cppproject::qt_status {

class CPPPROJECT_QT_STATUS_EXPORT Status final : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString message READ message WRITE set_message NOTIFY message_changed)
    Q_PROPERTY(bool busy READ is_busy WRITE set_busy NOTIFY busy_changed)

public:
    explicit Status(QObject* parent = nullptr);

    [[nodiscard]] QString message() const;
    [[nodiscard]] bool is_busy() const noexcept;

public slots:
    void set_message(QString message);
    void set_busy(bool busy);

signals:
    void message_changed(const QString& message);
    void busy_changed(bool busy);

private:
    QString message_;
    bool busy_{false};
};

} // namespace cppproject::qt_status
