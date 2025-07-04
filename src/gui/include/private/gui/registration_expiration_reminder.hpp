#pragma once

#include "gui/main_object.hpp"

#include <QDialog>

namespace Ui {
class RegistrationExpirationReminder;
}

class RegistrationExpirationReminder : public QDialog, public MainObject
{
    Q_OBJECT

public:
    explicit RegistrationExpirationReminder(bool *is_show, QWidget *parent = nullptr);
    ~RegistrationExpirationReminder();

private:
    virtual void initUi() override;

    virtual void initUiText() override;

    virtual void initConnect() override;

private slots:
    void on_check_box_checkStateChanged(const Qt::CheckState &arg1);

private:
    Ui::RegistrationExpirationReminder *ui;

    bool* is_show_;
};
