#include "cppproject/qt_status/status.hpp"

#include <utility>

namespace cppproject::qt_status {

Status::Status(QObject* parent) : QObject{parent} {}

QString Status::message() const {
    return message_;
}

bool Status::is_busy() const noexcept {
    return busy_;
}

void Status::set_message(QString message) {
    if (message_ == message) {
        return;
    }

    message_ = std::move(message);
    emit message_changed(message_);
}

void Status::set_busy(const bool busy) {
    if (busy_ == busy) {
        return;
    }

    busy_ = busy;
    emit busy_changed(busy_);
}

} // namespace cppproject::qt_status
