#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <memory>

class FramelessWidget;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    static std::unique_ptr<MainWindow> create(int role, QString username);

protected:
    virtual void initUI();
    virtual void initUIText();
    virtual void initConnect();

    virtual void loadCSS(const QString &cssFile);

private:
    FramelessWidget* frameless_widget;
};

#endif // MAINWINDOW_H
