#ifndef TEXTDOCUMENTINTEGRATION_H
#define TEXTDOCUMENTINTEGRATION_H

#include <QWidget>
#include <QTextDocument>
#include <QFileDialog>
#include <text-document-integration/qcpdocumentobject.h>

namespace Ui {
class TextDocumentIntegration;
}

class TextDocumentIntegration : public QWidget
{
    Q_OBJECT

public:
    explicit TextDocumentIntegration(QWidget *parent = 0);
    ~TextDocumentIntegration();

    void setupPlot();

private slots:
    void on_actionInsert_Plot_clicked();

    void on_actionSave_Document_clicked();

private:
    Ui::TextDocumentIntegration *ui;
};

#endif // TEXTDOCUMENTINTEGRATION_H
