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
    void invokeInitUiText();

protected:
    /*!
     * @brief         
     * 
     * @attention
     */
    virtual void initUi() = 0;

    /*!
     * @brief         
     * 
     * @attention
     */
    virtual void initConnect() = 0;

    /*!
     * @brief         
     * 
     * @attention
     */
    virtual void initUiText() = 0;

    /*!
     * @brief         
     * 
     * @param         tm_value
     * @return        QString
     * @attention
     */
    QString stdTmToQString(std::tm tm_value);

    /*!
     * @brief         
     * 
     * @param         parrent
     * @param         cssFile
     * @attention
     */
    void loadCSS(QWidget* parrent, const QString &css_file);
};
