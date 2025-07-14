#pragma once

#include "gui/config.hpp"

class IUiInitializable
{
public:
    virtual ~IUiInitializable() = default;

    /*!
     * @brief         
     * 
     * @attention
     */
    void invokeInitUiText() noexcept {
        initUiText();
    }

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
};
