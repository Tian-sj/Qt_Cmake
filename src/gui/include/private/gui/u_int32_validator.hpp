#pragma once

#include <QValidator>
#include <limits>

class UInt32Validator : public QValidator {
public:
    explicit UInt32Validator(QObject* parent = nullptr)
        : QValidator(parent) {}

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