#ifndef LOGMANAGEMENTWIDGET_H
#define LOGMANAGEMENTWIDGET_H

#include <QWidget>

namespace Admin {

namespace Ui {
class LogManagementWidget;
}

class LogManagementWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LogManagementWidget(QWidget *parent = nullptr);
    ~LogManagementWidget();

private:
    void initUI();

    void initUIText();

    void initConnect();

public slots:
    void windows_close();

private:
    Ui::LogManagementWidget *ui;
};

}

#endif // LOGMANAGEMENTWIDGET_H
