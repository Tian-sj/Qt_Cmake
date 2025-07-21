#pragma once

#include "gui/base_window.hpp"

#include <QDialog>

namespace Ui {
class RegistrationExpirationReminder;
}

class RegistrationExpirationReminder : public BaseWindow<RegistrationExpirationReminder, Ui::RegistrationExpirationReminder, QDialog>
{
    Q_OBJECT

public:
    explicit RegistrationExpirationReminder(bool *is_show, QWidget *parent = nullptr);
    ~RegistrationExpirationReminder();

    virtual void initUi() override;

    virtual void initUiText() override;

    virtual void initConnect() override;

    virtual void initLazy() override;

private slots:
    void on_check_box_checkStateChanged(const Qt::CheckState &arg1);

private:
    bool* is_show_;
};
