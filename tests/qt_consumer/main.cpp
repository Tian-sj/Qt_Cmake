#include "cppproject/qt_status/status.hpp"

#include <QString>

int main() {
    cppproject::qt_status::Status status;
    status.set_message(QStringLiteral("installed package"));
    status.set_busy(true);

    return status.message() == QStringLiteral("installed package") && status.is_busy() ? 0 : 1;
}
