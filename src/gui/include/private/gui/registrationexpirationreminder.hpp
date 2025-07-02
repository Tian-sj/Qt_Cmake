#pragma once

#include "gui/mainobject.hpp"

#include <QDialog>
#include <QTimer>

namespace Ui {
class RegistrationExpirationReminder;
}

class RegistrationExpirationReminder : public QDialog, public MainObject
{
    Q_OBJECT

public:
    explicit RegistrationExpirationReminder(bool *is_show, QWidget *parent = nullptr);
    ~RegistrationExpirationReminder();

private slots:
    void on_checkBox_stateChanged(int arg1);

private:
    virtual void init_ui() override;

    virtual void init_ui_text() override;

    virtual void init_connect() override;

private:
    Ui::RegistrationExpirationReminder *ui;

    bool* m_is_show;
};
