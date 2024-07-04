#ifndef DATAANALYSISWIDGET_H
#define DATAANALYSISWIDGET_H

#include <QWidget>

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

private:
    Ui::DataAnalysisWidget *ui;
};

}

#endif // DATAANALYSISWIDGET_H
