#include <app/customtreeview.h>
#include <QMouseEvent>

CustomTreeView::CustomTreeView(QWidget *parent) : QTreeView(parent)
{
    setSelectionMode(QAbstractItemView::MultiSelection);
}

QStandardItemModel *CustomTreeView::createModel()
{
    QStandardItemModel* model = new QStandardItemModel();
    model->setHorizontalHeaderLabels({"Series"});

    struct DeviceModel {
        QString name;
        QStringList devices;
    };

    struct Series {
        QString name;
        QList<DeviceModel> models;
    };

    QList<Series> seriesList = {
        {"CQAS", {{"CQAS100-200", {"Device 1", "Device 2", "Device 3"}},
                  {"CQAS200-300", {"Device 4", "Device 5", "Device 6"}}}},
        {"CQBS", {{"CQBS100-200", {"Device 7", "Device 8", "Device 9"}},
                  {"CQBS200-300", {"Device 10", "Device 11", "Device 12"}}}}
    };

    for (const auto& series : seriesList) {
        QStandardItem* seriesItem = new QStandardItem(series.name);
        seriesItem->setSelectable(false);  // Series items are not selectable

        for (const auto& modelInfo : series.models) {
            QStandardItem* modelItem = new QStandardItem(modelInfo.name);
            modelItem->setSelectable(false);  // Model items are not selectable

            for (const auto& device : modelInfo.devices) {
                QStandardItem* deviceItem = new QStandardItem(device);
                modelItem->appendRow(deviceItem);
            }

            seriesItem->appendRow(modelItem);
        }

        model->appendRow(seriesItem);
    }

    return model;
}

void CustomTreeView::mouseMoveEvent(QMouseEvent *event)
{
    event->ignore();
}

void CustomTreeView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    for (const QModelIndex& index : deselected.indexes()) {
        if (!selectedIndexes().contains(index)) {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, tr("Confirm"), tr("Do you want to deselect this item?"), QMessageBox::Yes | QMessageBox::No);

            if (reply == QMessageBox::No) {
                selectionModel()->select(index, QItemSelectionModel::Select);
            }
        }
    }
    QTreeView::selectionChanged(selected, deselected);
}

void CustomTreeView::mouseDoubleClickEvent(QMouseEvent *event)
{
    QModelIndex index = indexAt(event->pos());
    if (index.isValid()) {
        if (isExpanded(index)) {
            collapse(index);
        } else {
            expand(index);
        }
    } else {
        event->ignore();
    }
}
