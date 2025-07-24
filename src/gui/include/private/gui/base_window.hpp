#pragma once

#include "gui/i_ui_initializable.hpp"

#include <QFile>
#include <QWidget>
#include <QEvent>
#include <QTimer>

/*!
 * @brief         基础窗口模板类
 *
 * Base class for UI windows using Qt's setupUi pattern with CRTP and lazy initialization.
 *
 * @tparam        Derived   子类类型（CRTP）
 * @tparam        UiClass   自动生成的 UI 类
 * @tparam        Super     父类类型（通常为 QWidget 或其子类）
 *
 * @attention     使用 CRTP（Curiously Recurring Template Pattern）模式进行初始化封装。
 *                自动执行 UI 初始化、信号槽连接、本地化更新、延迟初始化等。
 */
template<typename Derived, typename UiClass, typename Super = QWidget>
class BaseWindow : public Super, public IUiInitializable
{
public:
    /*!
     * @brief         构造函数
     *
     * 初始化 UI、连接信号槽、调用初始化函数
     *
     * @param         parent   父级 QWidget（默认 nullptr）
     */
    explicit BaseWindow(QWidget* parent = nullptr)
        : Super(parent)
        , ui(new UiClass)
    {
        ui->setupUi(this);

        // 延迟初始化（确保界面构建完成后再触发 init）
        QMetaObject::invokeMethod(this, [self = static_cast<Derived*>(this)] {
            QMetaObject::connectSlotsByName(self);
            self->initUi();
            self->initConnect();
            self->initUiText();

            QTimer::singleShot(0, self, [self]() {
                self->initLazy();
            });
        }, Qt::QueuedConnection);
    }

    /*!
     * @brief         析构函数
     *
     * 释放 UI 对象
     */
    ~BaseWindow() override {
        delete ui;
    }

protected:
    UiClass* ui;  //!< Qt 自动生成的 UI 类指针

    /*!
     * @brief         初始化 UI 布局与控件状态
     *
     * Initialize UI components layout and states
     */
    virtual void initUi() override = 0;

    /*!
     * @brief         初始化信号与槽的连接
     *
     * Connect UI signals to corresponding slots
     */
    virtual void initConnect() override = 0;

    /*!
     * @brief         初始化界面文字（支持语言切换）
     *
     * Set all UI text strings (for internationalization)
     */
    virtual void initUiText() override = 0;

    /*!
     * @brief         延迟初始化（界面显示后触发）
     *
     * Lazy operations that require the UI to be fully rendered
     */
    virtual void initLazy() override = 0;

    /*!
     * @brief         语言切换事件处理
     *
     * 处理语言变化，自动调用 initUiText 刷新界面文本
     *
     * @param         ev   Qt 事件指针
     */
    void changeEvent(QEvent* ev) override {
        if (ev->type() == QEvent::LanguageChange) {
            IUiInitializable* iface = this;
            iface->initUiText();
        }
        Super::changeEvent(ev);
    }

    /*!
     * @brief         将 std::tm 时间转换为 QString
     *
     * Convert std::tm to formatted QString
     *
     * @param         tm_value   std::tm 时间结构体
     * @return        格式化后的时间字符串（yyyy-MM-dd hh:mm:ss）
     */
    static inline QString stdTmToQString(std::tm tm_value) noexcept {
        return QDateTime::fromSecsSinceEpoch(mktime(&tm_value)).toString("yyyy-MM-dd hh:mm:ss");
    }

    /*!
     * @brief         加载外部 CSS 样式表
     *
     * Load and apply an external CSS stylesheet to a QWidget
     *
     * @param         parent     要设置样式的 QWidget 对象
     * @param         css_file   样式表文件路径
     */
    static void loadCSS(QWidget* parent, const QString &css_file) noexcept {
        QFile file(css_file);
        if (!file.open(QFile::ReadOnly | QFile::Text)) {
            return;
        }

        QTextStream stream(&file);
        QString style = stream.readAll();

        parent->setStyleSheet(style);

        file.close();
    }

};
