#pragma once

#include "gui/guiext.hpp"
#include "gui/config.hpp"

#include <QWidget>

class MainObject
{
public:
    explicit MainObject();
    ~MainObject();

    /*!
     * @brief         
     * 
     * @attention
     */
    void invoke_init_ui_text();

protected:
    /*!
     * @brief         
     * 
     * @attention
     */
    virtual void init_ui() = 0;

    /*!
     * @brief         
     * 
     * @attention
     */
    virtual void init_connect() = 0;

    /*!
     * @brief         
     * 
     * @attention
     */
    virtual void init_ui_text() = 0;

    /*!
     * @brief         
     * 
     * @param         tm_value
     * @return        QString
     * @attention
     */
    QString stdtm_to_QString(std::tm tm_value);

    /*!
     * @brief         
     * 
     * @param         parrent
     * @param         cssFile
     * @attention
     */
    void loadCSS(QWidget* parrent, const QString &cssFile);
};
