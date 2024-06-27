#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>

class FramelessWidget;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    static MainWindow* create(int role);

protected:
    virtual void initUI();
    virtual void initUIText();

    virtual void loadCSS(const QString &cssFile);

private:
    FramelessWidget* frameless_widget;
};

#endif // MAINWINDOW_H
