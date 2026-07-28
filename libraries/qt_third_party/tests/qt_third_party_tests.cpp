#include "qcustomplot.h"
#include "test_support.hpp"
#include "xlsxdocument.h"

#include <QWKWidgets/widgetwindowagent.h>
#include <QString>
#include <QVariant>

int main() {
    cppproject::test::Suite suite;

    suite.add("QCustomPlot version", [] {
        cppproject::test::require_equal(
            QString::fromLatin1(QCUSTOMPLOT_VERSION_STR),
            QStringLiteral("2.1.1"),
            "QCustomPlot version");
    });

    suite.add("QXlsx in-memory document", [] {
        QXlsx::Document document;
        cppproject::test::require(
            document.write(1, 1, QStringLiteral("ready")),
            "QXlsx write");
        cppproject::test::require_equal(
            document.read(1, 1).toString(),
            QStringLiteral("ready"),
            "QXlsx read");
    });

    suite.add("QWindowKit widgets module", [] {
        cppproject::test::require_equal(
            QString::fromLatin1(QWK::WidgetWindowAgent::staticMetaObject.className()),
            QStringLiteral("QWK::WidgetWindowAgent"),
            "QWindowKit widgets target");
    });

    return suite.run();
}
