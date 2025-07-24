#pragma once

#include "gui/config.hpp"

/*!
 * @brief         UI 初始化接口类
 *
 * Interface for UI initialization sequence
 *
 * @attention     提供统一的初始化流程接口，适用于多窗口结构中的 UI 构建、连接、国际化和懒加载逻辑。
 */
class IUiInitializable
{
public:
    /*!
     * @brief         虚析构函数
     *
     * Virtual destructor
     */
    virtual ~IUiInitializable() = default;

    /*!
     * @brief         初始化 UI 控件（不含连接与文本）
     *
     * Initialize UI widgets and layout (excluding signals and text)
     */
    virtual void initUi() = 0;

    /*!
     * @brief         设置信号与槽连接
     *
     * Set up signal-slot connections
     */
    virtual void initConnect() = 0;

    /*!
     * @brief         设置 UI 文本内容（支持国际化）
     *
     * Initialize translatable UI strings (for i18n support)
     */
    virtual void initUiText() = 0;

    /*!
     * @brief         延迟初始化操作
     *
     * Perform delayed initialization such as loading settings, restoring state, etc.
     */
    virtual void initLazy() = 0;
};
