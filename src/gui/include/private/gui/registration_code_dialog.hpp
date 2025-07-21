#pragma once

#include "gui/base_window.hpp"

#include <QDialog>

namespace Ui {
class RegistrationCodeDialog;
}

class RegistrationCodeDialog : public BaseWindow<RegistrationCodeDialog, Ui::RegistrationCodeDialog, QDialog>
{
    Q_OBJECT

public:
    explicit RegistrationCodeDialog(QWidget *parent = nullptr);
    ~RegistrationCodeDialog();

    void initUi() override;

    void initUiText() override;

    void initConnect() override;

    void initLazy() override;

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_btn_copy_clicked();

    void on_btn_activate_clicked();
};
