#ifndef REGISTRATIONCODEDIALOG_H
#define REGISTRATIONCODEDIALOG_H

#include "gui/mainobject.h"

#include <QDialog>

namespace Ui {
class RegistrationCodeDialog;
}

class RegistrationCodeDialog : public QDialog, public MainObject
{
    Q_OBJECT

public:
    explicit RegistrationCodeDialog(QWidget *parent = nullptr);
    ~RegistrationCodeDialog();

private:
    virtual void init_ui() override;

    virtual void init_ui_text() override;

    virtual void init_connect() override;

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_btn_copy_clicked();

    void on_btn_activate_clicked();

private:
    Ui::RegistrationCodeDialog *ui;
};

#endif // REGISTRATIONCODEDIALOG_H
