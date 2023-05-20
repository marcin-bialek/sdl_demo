#pragma once
#include <queue>
#include <mutex>
#include <optional>


template <typename T>
class SafeQueue {
public:
    void push(const T& value) {
        std::lock_guard lock{mutex};
        queue.push(value);
    }

    void push(T&& value) {
        std::lock_guard lock{mutex};
        queue.push(std::move(value));
    }

    std::optional<T> pop() {
        std::lock_guard lock{mutex};

        if (queue.empty()) {
            return std::nullopt;
        }

        T value = std::move(queue.front());
        queue.pop();
        return std::move(value);
    }

private:
    std::queue<T> queue;
    std::mutex mutex;
};

