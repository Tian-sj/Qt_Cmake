#include "gui/dragdropplaintexteditrc.hpp"

#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QFile>
#include <QFileInfo>
#include <QDataStream>

DragDropPlainTextEditRC::DragDropPlainTextEditRC(QWidget *parent) 
    : QPlainTextEdit(parent) 
{
    setAcceptDrops(true);
}

void DragDropPlainTextEditRC::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void DragDropPlainTextEditRC::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();

    if (mimeData->hasUrls()) {
        QList<QUrl> urlList = mimeData->urls();

        if (urlList.size() > 0) {
            QString filePath = urlList.at(0).toLocalFile();
            if (isValidFile(filePath)) {
                loadFile(filePath);
            } else {
                setPlainText(tr("Invalid file type. Only .cqrc files are allowed."));
            }
        }
    }
}

void DragDropPlainTextEditRC::loadFile(const QString &filePath)
{
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream fileIn(&file);
        QByteArray byteArray;
        fileIn >> byteArray;
        file.close();

        QDataStream in(&byteArray, QIODevice::ReadOnly);
        QString text;
        in >> text;
        setPlainText(text);
    }
}

bool DragDropPlainTextEditRC::isValidFile(const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    return fileInfo.suffix().toLower() == "cqrc";
}
