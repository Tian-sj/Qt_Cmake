#ifndef CUSTOMTREEVIEW_H
#define CUSTOMTREEVIEW_H

#include <QTreeView>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QMessageBox>

class CustomTreeView : public QTreeView
{
    Q_OBJECT
public:
    CustomTreeView(QWidget *parent = nullptr);

    // test
    QStandardItemModel* createModel();

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
};

#endif // CUSTOMTREEVIEW_H