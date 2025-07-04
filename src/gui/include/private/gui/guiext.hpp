/*!
 * @file          guiext.hpp
 * @brief         Some macro definitions or template functions that are globally visible to the front-end
 * @author        Tian_sj
 * @date          2024-12-18
 */
#pragma once

#include "gui/signal_manager_singleton.hpp"

#include <QObject>

/*!
 * @brief         Connection language modification of the template function that triggers the refresh of the text
 *
 * 对触发文本刷新的模板函数进行连接语言修改
 * 
 * @tparam        T
 * @param         obj
 * @attention
 */
template <typename T>
void connectLanguageChanged(T* obj) {
    QObject::connect(&SignalManagerSingleton::getInstance(), &SignalManagerSingleton::languageChanged, obj, [obj]() {
        obj->invokeInitUiText();
    });
}
