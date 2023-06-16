#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class main_window;
}
QT_END_NAMESPACE

class main_window : public QMainWindow {
    Q_OBJECT

public:
    main_window(QWidget* parent = nullptr);
    ~main_window();

private slots:
    void on_pushButton_clicked();

private:
    Ui::main_window* ui;
};
#endif  // MAIN_WINDOW_H
