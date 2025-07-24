#pragma once

#include <QValidator>
#include <QHostAddress>
#include <QAbstractSocket>

/*!
 * @brief         IPv4 地址格式输入验证器
 *
 * IPv4 address input validator
 *
 * @attention     用于 QLineEdit 等输入控件中，实时验证用户输入的 IPv4 地址格式合法性（例如：192.168.1.1）。
 */
class IPv4Validator : public QValidator {
public:
    /*!
     * @brief         构造函数
     *
     * @param         parent   父对象指针（可为空）
     */
    explicit IPv4Validator(QObject* parent = nullptr)
        : QValidator(parent) {}

    /*!
     * @brief         验证用户输入是否为合法 IPv4 地址格式
     *
     * @param         input   当前用户输入内容（可修改）
     * @param         pos     光标位置（未使用）
     * @return        验证状态（Acceptable / Intermediate / Invalid）
     *
     * @attention
     * - 空串或正在输入中的格式 → 返回 Intermediate；
     * - 格式错误或非法字符 → 返回 Invalid；
     * - 格式合法且为有效 IPv4 → 返回 Acceptable。
     */
    State validate(QString &input, int & /*pos*/) const override {
        // 1) 允许空串开始输入
        if (input.isEmpty())
            return Intermediate;

        // 2) 仅允许数字和点
        for (auto c : input) {
            if (!(c.isDigit() || c == '.'))
                return Invalid;
        }

        // 3) 点数量不超过 3
        if (input.count('.') > 3)
            return Invalid;

        // 4) 每段最大长度为 3
        const auto parts = input.split('.');
        for (const auto &seg : parts) {
            if (seg.length() > 3)
                return Invalid;
        }

        // 5) 如果是完整 4 段，且末尾不是点，交由 QHostAddress 验证
        if (parts.size() == 4 && !input.endsWith('.')) {
            QHostAddress addr;
            if (addr.setAddress(input)
                && addr.protocol() == QAbstractSocket::IPv4Protocol)
                return Acceptable;
            else
                return Invalid;
        }

        // 6) 否则视为正在输入中
        return Intermediate;
    }
};
