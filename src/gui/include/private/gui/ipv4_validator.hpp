#pragma once

#include <QValidator>
#include <QHostAddress>
#include <QAbstractSocket>

class IPv4Validator : public QValidator {
public:
    explicit IPv4Validator(QObject* parent = nullptr)
        : QValidator(parent) {}

    State validate(QString &input, int & /*pos*/) const override {
        // 允许空串开始输入
        if (input.isEmpty())
            return Intermediate;

        // 只允许数字和点
        for (auto c : input) {
            if (!(c.isDigit() || c == '.'))
                return Invalid;
        }

        // “.” 不超过 3 个
        if (input.count('.') > 3)
            return Invalid;

        // 每段长度不超 3
        auto parts = input.split('.');
        for (const auto &seg : parts) {
            if (seg.length() > 3)
                return Invalid;
        }

        // 如果已输入 4 段且末尾没“.”，用 QHostAddress 做最终校验
        if (parts.size() == 4 && !input.endsWith('.')) {
            QHostAddress addr;
            if (addr.setAddress(input)
             && addr.protocol() == QAbstractSocket::IPv4Protocol)
                return Acceptable;
            else
                return Invalid;
        }

        // 其他情况都当做中间态，继续输入
        return Intermediate;
    }
};
