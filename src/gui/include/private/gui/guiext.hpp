/*!
 * @file          guiext.hpp
 * @brief         Some macro definitions or template functions that are globally visible to the front-end
 * @author        Tian_sj
 * @date          2024-12-18
 */

#ifndef GUIEXT_HPP
#define GUIEXT_HPP

#include "gui/signalmanager.h"

#include <QObject>

/*!
 * @brief         Connection language modification of the template function that triggers the refresh of the text
 * 
 * @tparam        T
 * @param         obj
 * @attention
 */
template <typename T>
void connect_language_changed(T* obj) {
    QObject::connect(&SignalManager::instance(), &SignalManager::language_changed, obj, [obj]() {
        obj->invoke_init_ui_text();
    });
}

#endif // GUIEXT_HPP
