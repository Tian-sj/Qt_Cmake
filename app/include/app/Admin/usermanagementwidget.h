#ifndef USERMANAGEMENTWIDGET_H
#define USERMANAGEMENTWIDGET_H

#include <QWidget>

namespace Admin {

namespace Ui {
class UserManagementWidget;
}

class UserManagementWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UserManagementWidget(QWidget *parent = nullptr);
    ~UserManagementWidget();

private:
    Ui::UserManagementWidget *ui;
};

}

#endif // USERMANAGEMENTWIDGET_H
