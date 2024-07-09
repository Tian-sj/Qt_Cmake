#ifndef DATAANALYSISWIDGET_H
#define DATAANALYSISWIDGET_H

#include <QWidget>
#include <QCalendarWidget>

namespace Admin {

namespace Ui {
class DataAnalysisWidget;
}

class DataAnalysisWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DataAnalysisWidget(QWidget *parent = nullptr);
    ~DataAnalysisWidget();

private:
    void initUI();

    void initUIText();

    void initConnect();

    void initCalendarWidget(QCalendarWidget* calendar);

    void loadCSS(const QString &cssFile, QWidget* w = nullptr);

private slots:
    void on_page_change(int index);
private:
    Ui::DataAnalysisWidget *ui;
};

}

#endif // DATAANALYSISWIDGET_H
