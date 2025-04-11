#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T>
class BlockingQueue {
public:
    // 向队列中添加元素
    void push(const T& item) {
        std::lock_guard<std::mutex> lock(mtx_);
        queue_.push(item);
        cv_.notify_one(); // 通知一个等待的线程
    }

    // 获取队首元素（阻塞直到队列非空）
    T front() {
        std::unique_lock<std::mutex> lock(mtx_);
        cv_.wait(lock, [this] { return !queue_.empty(); }); // 阻塞直到队列非空
        return queue_.front();
    }

    T take() {
        std::unique_lock<std::mutex> lock(mtx_);
        cv_.wait(lock, [this] { return !queue_.empty(); });
        T item = std::move(queue_.front()); // 移动语义避免拷贝
        queue_.pop();
        return item;
    }

    // 移除队首元素（需外部调用，非阻塞）
    void pop() {
        std::lock_guard<std::mutex> lock(mtx_);
        if (!queue_.empty()) {
            queue_.pop();
        }
    }

    // 检查队列是否为空
    bool empty() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return queue_.empty();
    }

private:
    mutable std::mutex mtx_;         // 互斥锁
    std::condition_variable cv_;     // 条件变量
    std::queue<T> queue_;            // 内部队列
};