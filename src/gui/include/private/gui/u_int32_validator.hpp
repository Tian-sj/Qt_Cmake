#pragma once

#include <QValidator>
#include <limits>

/*!
 * @brief         无符号 32 位整数输入验证器
 *
 * Validator for unsigned 32-bit integer (0 ~ 4294967295)
 *
 * @attention     该类继承自 QValidator，用于验证 QLineEdit 等控件中的文本是否为合法 uint32_t 数值。
 */
class UInt32Validator : public QValidator {
public:
    /*!
     * @brief         构造函数
     *
     * @param         parent   父对象（可为空）
     */
    explicit UInt32Validator(QObject* parent = nullptr)
        : QValidator(parent) {}

    /*!
     * @brief         验证用户输入是否符合 uint32 范围
     *
     * @param         input   用户当前输入的字符串（可被修改）
     * @param         pos     当前光标位置（可被修改）
     * @return        验证状态（Acceptable / Intermediate / Invalid）
     *
     * @attention     输入为空或部分数字时返回 Intermediate；
     *                所有字符为数字且值在 [0, 4294967295] 范围内返回 Acceptable；
     *                否则返回 Invalid。
     */
    State validate(QString& input, int& pos) const override {
        // 允许用户继续输入空串或“部分”数字
        if (input.isEmpty())
            return Intermediate;

        // 只要每个字符都是数字，就先允许
        for (QChar c : input) {
            if (!c.isDigit())
                return Invalid;
        }

        // 超过 10 位肯定越界
        if (input.size() > 10)
            return Invalid;

        bool ok = false;
        quint64 val = input.toULongLong(&ok);
        if (!ok)
            return Invalid;  // 转换失败

        // 最终判断是否落入 uint32_t 范围
        return (val <= std::numeric_limits<uint32_t>::max())
                   ? Acceptable
                   : Invalid;
    }
};
