#pragma once

#include <QValidator>
#include <limits>

/*!
 * @brief         无符号 16 位整数输入验证器
 *
 * Validator for unsigned 16-bit integer (0 ~ 65535)
 *
 * @attention     用于 QLineEdit 等输入控件，限制用户只能输入合法的 uint16 数值。
 */
class UInt16Validator : public QValidator {
public:
    /*!
     * @brief         构造函数
     *
     * @param         parent   父 QObject 指针（可选）
     */
    explicit UInt16Validator(QObject* parent = nullptr)
        : QValidator(parent) {}

    /*!
     * @brief         验证输入是否合法（uint16 范围）
     *
     * 验证输入文本是否是 0~65535 的合法无符号整数
     *
     * @param         input   当前输入内容（可修改）
     * @param         pos     当前光标位置（可修改）
     * @return        验证状态（Acceptable / Intermediate / Invalid）
     *
     * @attention
     * - 返回 Intermediate 表示用户仍在输入中（如空串、部分数字）；
     * - 返回 Acceptable 表示验证通过；
     * - 返回 Invalid 表示输入无效或越界。
     */
    State validate(QString &input, int &pos) const override {
        Q_UNUSED(pos);

        // 1) 允许空串以便继续输入
        if (input.isEmpty())
            return Intermediate;

        // 2) 只要有非数字字符就拒绝
        for (QChar c : input) {
            if (!c.isDigit())
                return Invalid;
        }

        // 3) 超过 5 位数字一定越界 (最大 65535 共 5 位)
        if (input.size() > 5)
            return Invalid;

        // 4) 转成无符号数并判断上限
        bool ok = false;
        quint32 v = input.toUInt(&ok, 10);
        if (!ok)
            return Invalid;

        return (v <= std::numeric_limits<quint16>::max())
                   ? Acceptable
                   : Invalid;
    }
};
