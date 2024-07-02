#ifndef TREEITEM_H
#define TREEITEM_H

#include <QWidget>
#include <QToolButton>
#include <QFrame>
#include <QVBoxLayout>
#include <QString>
#include <QList>

class TreeItem : public QWidget {
    Q_OBJECT

public:
    explicit TreeItem(const QString &text, QWidget *parent = nullptr);

    void addChild(TreeItem *child);
    void removeChild(TreeItem *child);
    QList<TreeItem*> getChildren() const;
    TreeItem* getParentItem() const;
    QString getText() const;
    void expandAll();
    void collapseAll();

private:
    QToolButton *button;
    QFrame *contentFrame;
    QVBoxLayout *contentLayout;
};

#endif // TREEITEM_H
