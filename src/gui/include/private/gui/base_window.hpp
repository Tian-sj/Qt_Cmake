#pragma once

#include "gui/i_ui_initializable.hpp"

#include <QFile>
#include <QWidget>
#include <QEvent>

template<typename Derived, typename UiClass, typename Super = QWidget>
class BaseWindow : public Super, public IUiInitializable
{
public:
    explicit BaseWindow(QWidget* parent = nullptr)
        : Super(parent)
        , ui(new UiClass)
    {
        ui->setupUi(this);

        QMetaObject::invokeMethod(this, [this] {
            IUiInitializable* iface = this;
            QMetaObject::connectSlotsByName(this);
            iface->initUi();
            iface->initConnect();
            iface->initUiText();
        }, Qt::QueuedConnection);
    }

    ~BaseWindow() override {
        delete ui;
    }

protected:
    UiClass* ui;

    /// 基础初始化，非虚，派生可 override
    // virtual void baseInitUi() { /* e.g. 设置固定大小、connect 信号槽… */ }

    /*!
     * @brief
     *
     * @attention
     */
    virtual void initUi() override = 0;

    /*!
     * @brief
     *
     * @attention
     */
    virtual void initConnect() override = 0;

    /*!
     * @brief
     *
     * @attention
     */
    virtual void initUiText() override = 0;

    void changeEvent(QEvent* ev) override {
        if (ev->type() == QEvent::LanguageChange) {
            // 调用接口的 retranslateUi，多态分发到 Derived
            IUiInitializable* iface = this;
            iface->initUiText();
        }
        Super::changeEvent(ev);
    }


    /*!
     * @brief
     *
     * @param         tm_value
     * @return        QString
     * @attention
     */
    static inline QString stdTmToQString(std::tm tm_value) noexcept {
        return QDateTime::fromSecsSinceEpoch(mktime(&tm_value)).toString("yyyy-MM-dd hh:mm:ss");
    }

    /*!
     * @brief
     *
     * @param         parrent
     * @param         cssFile
     * @attention
     */
    static void loadCSS(QWidget* parrent, const QString &css_file) noexcept {
        QFile file(css_file);
        if (!file.open(QFile::ReadOnly | QFile::Text)) {
            return;
        }

        QTextStream stream(&file);
        QString style = stream.readAll();

        parrent->setStyleSheet(style);

        file.close();
    }

};
