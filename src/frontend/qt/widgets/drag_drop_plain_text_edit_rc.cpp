#include "widgets/drag_drop_plain_text_edit_rc.hpp"

#include <QCoreApplication>
#include <QDataStream>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFile>
#include <QFileInfo>
#include <QMimeData>

namespace {
constexpr qint64 maximum_license_file_size = 1024 * 1024;
}

DragDropPlainTextEditRC::DragDropPlainTextEditRC(QWidget* parent) : QPlainTextEdit(parent) {
    setAcceptDrops(true);
}

void DragDropPlainTextEditRC::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void DragDropPlainTextEditRC::dropEvent(QDropEvent* event) {
    if (const QMimeData* mime_data = event->mimeData(); mime_data->hasUrls()) {
        if (const QList<QUrl> url_list = mime_data->urls(); !url_list.empty()) {
            if (const QString file_path = url_list.at(0).toLocalFile(); isValidFile(file_path)) {
                loadFile(file_path);
            } else {
                setPlainText(QCoreApplication::translate(
                    "DragDropPlainTextEditRC", "Invalid file type. Only .cqrc files are allowed."));
            }
        }
    }
}

void DragDropPlainTextEditRC::loadFile(const QString& file_path) {
    QFile file(file_path);
    if (file.size() <= maximum_license_file_size && file.open(QIODevice::ReadOnly)) {
        QDataStream file_in(&file);
        QByteArray byte_array;
        file_in >> byte_array;

        QDataStream in(&byte_array, QIODevice::ReadOnly);
        QString text;
        in >> text;
        if (in.status() == QDataStream::Ok) {
            setPlainText(text);
        }
    }
}

bool DragDropPlainTextEditRC::isValidFile(const QString& file_path) {
    QFileInfo file_info(file_path);
    return file_info.suffix().toLower() == "cqrc";
}
