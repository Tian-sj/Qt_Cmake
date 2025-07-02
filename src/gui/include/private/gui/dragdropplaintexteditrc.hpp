#pragma once

#include <QPlainTextEdit>

class DragDropPlainTextEditRC : public QPlainTextEdit {
    Q_OBJECT

public:
    DragDropPlainTextEditRC(QWidget *parent = nullptr);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;

    void dropEvent(QDropEvent *event) override;

private:
    void loadFile(const QString &filePath);

    bool isValidFile(const QString &filePath);
};
