#pragma once

#include <QValidator>
#include <limits>

class UInt16Validator : public QValidator {
public:
    explicit UInt16Validator(QObject* parent = nullptr)
        : QValidator(parent) {}

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