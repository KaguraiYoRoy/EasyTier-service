#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T>
class BlockingQueue {
public:
    // ����������Ԫ��
    void push(const T& item) {
        std::lock_guard<std::mutex> lock(mtx_);
        queue_.push(item);
        cv_.notify_one(); // ֪ͨһ���ȴ����߳�
    }

    // ��ȡ����Ԫ�أ�����ֱ�����зǿգ�
    T front() {
        std::unique_lock<std::mutex> lock(mtx_);
        cv_.wait(lock, [this] { return !queue_.empty(); }); // ����ֱ�����зǿ�
        return queue_.front();
    }

    T take() {
        std::unique_lock<std::mutex> lock(mtx_);
        cv_.wait(lock, [this] { return !queue_.empty(); });
        T item = std::move(queue_.front()); // �ƶ�������⿽��
        queue_.pop();
        return item;
    }

    // �Ƴ�����Ԫ�أ����ⲿ���ã���������
    void pop() {
        std::lock_guard<std::mutex> lock(mtx_);
        if (!queue_.empty()) {
            queue_.pop();
        }
    }

    // �������Ƿ�Ϊ��
    bool empty() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return queue_.empty();
    }

private:
    mutable std::mutex mtx_;         // ������
    std::condition_variable cv_;     // ��������
    std::queue<T> queue_;            // �ڲ�����
};