/*!
 * @file          guiapp.hpp
 * @brief         Interface class provided to main.cpp to implement some initialization operations
 * @author        Tian_sj
 * @date          2024-12-18
 */
#pragma once

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
     * 运行 Qt 窗口并进入消息循环
     *
     * @return        int
     * @attention
     */
    int run();

private:
    /*!
     * @brief         Set the default style object
     *
     * 设置默认样式对
     *
     * @attention
     */
    void set_default_style();

private:
    std::unique_ptr<QApplication> app;
    std::unique_ptr<QWidget> main_window;
};
