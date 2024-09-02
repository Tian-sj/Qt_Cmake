#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QTreeView>
#include <QStandardItemModel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    virtual void loadCSS(const QString &cssFile);

private:
    Ui::MainWindow *ui;

    QTreeView *treeView;
    QStandardItemModel *model;
};

#endif // MAINWINDOW_H
