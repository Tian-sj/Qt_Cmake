#pragma once

#include "gui/base_window.hpp"

#include <QDialog>

namespace Ui {
class RegistrationExpirationReminder;
}

class RegistrationExpirationReminder final : public BaseWindow<RegistrationExpirationReminder, Ui::RegistrationExpirationReminder, QDialog>
{
    Q_OBJECT

public:
    explicit RegistrationExpirationReminder(bool *is_show, QWidget *parent = nullptr);
    ~RegistrationExpirationReminder() override;

    void initUi() override;

    void initUiText() override;

    void initConnect() override;

    void initLazy() override;

private slots:
    void on_check_box_checkStateChanged(const Qt::CheckState &arg1) const;

private:
    bool* is_show_;
};
