#include <TreeItem/treeitem.h>

TreeItem::TreeItem(const QString &text, QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);

    button = new QToolButton(this);
    button->setText(text);
    button->setCheckable(true);
    button->setChecked(false);
    button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    button->setArrowType(Qt::RightArrow);

    connect(button, &QToolButton::toggled, this, [=](bool checked) {
        if (checked) {
            button->setArrowType(Qt::DownArrow);
            contentFrame->show();
        } else {
            button->setArrowType(Qt::RightArrow);
            contentFrame->hide();
        }
    });

    layout->addWidget(button);

    contentFrame = new QFrame(this);
    contentFrame->setFrameStyle(QFrame::Panel | QFrame::Raised);
    contentFrame->setVisible(false);

    contentLayout = new QVBoxLayout(contentFrame);
    layout->addWidget(contentFrame);

    setLayout(layout);
}

void TreeItem::addChild(TreeItem *child) {
    contentLayout->addWidget(child);
}

void TreeItem::removeChild(TreeItem *child) {
    contentLayout->removeWidget(child);
    delete child;
}

QList<TreeItem*> TreeItem::getChildren() const {
    QList<TreeItem*> children;
    for (int i = 0; i < contentLayout->count(); ++i) {
        QWidget *widget = contentLayout->itemAt(i)->widget();
        if (TreeItem *child = qobject_cast<TreeItem*>(widget)) {
            children.append(child);
        }
    }
    return children;
}

TreeItem* TreeItem::getParentItem() const {
    return qobject_cast<TreeItem*>(parentWidget());
}

QString TreeItem::getText() const {
    return button->text();
}

void TreeItem::expandAll() {
    button->setChecked(true);
    for (TreeItem *child : getChildren()) {
        child->expandAll();
    }
}

void TreeItem::collapseAll() {
    button->setChecked(false);
    for (TreeItem *child : getChildren()) {
        child->collapseAll();
    }
}
