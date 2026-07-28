#include "cppproject/qt_status/status.hpp"
#include "test_support.hpp"

#include <QString>

int main() {
    cppproject::test::Suite suite;

    suite.add("properties and signals", [] {
        cppproject::qt_status::Status status;
        QString observed_message;
        bool observed_busy = false;
        int message_changes = 0;
        int busy_changes = 0;

        QObject::connect(
            &status,
            &cppproject::qt_status::Status::message_changed,
            [&observed_message, &message_changes](const QString& message) {
                observed_message = message;
                ++message_changes;
            });
        QObject::connect(
            &status,
            &cppproject::qt_status::Status::busy_changed,
            [&observed_busy, &busy_changes](const bool busy) {
                observed_busy = busy;
                ++busy_changes;
            });

        status.set_message(QStringLiteral("ready"));
        status.set_busy(true);
        status.set_message(QStringLiteral("ready"));
        status.set_busy(true);

        cppproject::test::require_equal(status.message(), QStringLiteral("ready"), "message");
        cppproject::test::require(status.is_busy(), "busy state");
        cppproject::test::require_equal(observed_message, QStringLiteral("ready"), "message signal");
        cppproject::test::require(observed_busy, "busy signal");
        cppproject::test::require_equal(message_changes, 1, "duplicate message signal");
        cppproject::test::require_equal(busy_changes, 1, "duplicate busy signal");
    });

    return suite.run();
}
