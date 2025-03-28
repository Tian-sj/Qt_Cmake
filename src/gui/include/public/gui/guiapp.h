/*!
 * @file          guiapp.h
 * @brief         Interface class provided to main.cpp to implement some initialization operations
 * @author        Tian_sj
 * @date          2024-12-18
 */

#ifndef GUIAPP_H
#define GUIAPP_H

#include <memory>

class QApplication;
class QWidget;

class GuiApp {

public:
    GuiApp(int &argc, char **argv);
    ~GuiApp();

    /*!
     * @brief         Run Qt's window and enter the message loop
     * 
     * @return        int
     * @attention
     */
    int run();

private:
    /*!
     * @brief         Set the default style object
     * 
     * @attention
     */
    void set_default_style();

private:
    std::unique_ptr<QApplication> app;
    std::unique_ptr<QWidget> main_window;
};

#endif // GUIAPP_H
