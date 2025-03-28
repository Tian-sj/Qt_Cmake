#ifndef MAINOBJECT_H
#define MAINOBJECT_H

#include <gui/guiext.hpp>
#include "gui/config.h"

#include <QWidget>

class MainObject
{
public:
    explicit MainObject();
    ~MainObject();

    void invoke_init_ui_text();

protected:
    virtual void init_ui() = 0;
    virtual void init_connect() = 0;
    virtual void init_ui_text() = 0;

    QString stdtm_to_QString(std::tm tm_value);

    void loadCSS(QWidget* parrent, const QString &cssFile);


};

#endif // MAINOBJECT_H
