#pragma once

#include "gui/config.hpp"

class IUiInitializable
{
public:
    virtual ~IUiInitializable() = default;

    /*!
     * @brief 初始化 UI 控件（不含连接与文本）
     */
    virtual void initUi() = 0;

    /*!
     * @brief 设置信号与槽连接
     */
    virtual void initConnect() = 0;

    /*!
     * @brief 设置 UI 文本内容（支持国际化）
     */
    virtual void initUiText() = 0;

    /*!
     * @brief 延迟初始化操作，如加载配置文件、状态还原等
     */
    virtual void initLazy() = 0;
};
