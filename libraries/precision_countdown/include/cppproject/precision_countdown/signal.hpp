#pragma once

#include <cstddef>
#include <functional>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <utility>
#include <vector>

namespace cppproject::precision_countdown {

template <typename... Arguments> class Signal final {
private:
    using Slot = std::function<void(Arguments...)>;

    struct State {
        std::mutex mutex;
        std::unordered_map<std::size_t, Slot> slots;
        std::size_t next_identifier{1};
    };

public:
    class Connection final {
    public:
        Connection() = default;
        Connection(const Connection&) = delete;
        Connection& operator=(const Connection&) = delete;
        Connection(Connection&& other) noexcept
            : state_{std::move(other.state_)}, identifier_{std::exchange(other.identifier_, 0)} {}

        Connection& operator=(Connection&& other) noexcept {
            if (this != &other) {
                disconnect();
                state_ = std::move(other.state_);
                identifier_ = std::exchange(other.identifier_, 0);
            }
            return *this;
        }
        ~Connection() {
            disconnect();
        }

        void disconnect() noexcept {
            if (auto state = state_.lock()) {
                const std::scoped_lock lock{state->mutex};
                state->slots.erase(identifier_);
            }
            state_.reset();
            identifier_ = 0;
        }

        [[nodiscard]] bool connected() const noexcept {
            const auto state = state_.lock();
            if (!state) {
                return false;
            }
            const std::scoped_lock lock{state->mutex};
            return state->slots.contains(identifier_);
        }

    private:
        friend class Signal;

        Connection(std::weak_ptr<State> state, const std::size_t identifier)
            : state_{std::move(state)}, identifier_{identifier} {}

        std::weak_ptr<State> state_;
        std::size_t identifier_{0};
    };

    Signal() : state_{std::make_shared<State>()} {}
    Signal(const Signal&) = delete;
    Signal& operator=(const Signal&) = delete;
    Signal(Signal&&) = delete;
    Signal& operator=(Signal&&) = delete;

    [[nodiscard]] Connection connect(Slot slot) {
        const std::scoped_lock lock{state_->mutex};
        const auto identifier = state_->next_identifier++;
        state_->slots.emplace(identifier, std::move(slot));
        return Connection{state_, identifier};
    }

    void publish(Arguments... arguments) const {
        std::vector<Slot> snapshot;
        {
            // 复制订阅快照后释放锁，允许回调安全地连接或断开其他订阅。
            const std::scoped_lock lock{state_->mutex};
            snapshot.reserve(state_->slots.size());
            for (const auto& [identifier, slot] : state_->slots) {
                static_cast<void>(identifier);
                snapshot.push_back(slot);
            }
        }

        for (const auto& slot : snapshot) {
            try {
                slot(arguments...);
            } catch (...) {
                // 一个订阅者失败不应阻止其他订阅者，也不应终止定时器工作线程。
            }
        }
    }

    [[nodiscard]] std::size_t size() const noexcept {
        const std::scoped_lock lock{state_->mutex};
        return state_->slots.size();
    }

private:
    std::shared_ptr<State> state_;
};

} // namespace cppproject::precision_countdown
