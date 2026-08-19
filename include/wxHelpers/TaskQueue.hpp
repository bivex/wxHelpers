#pragma once

#include <wx/wx.h>
#include "Events.hpp"
#include <functional>
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <atomic>
#include <memory>

namespace wxHelpers::Tasks {

class TaskQueue {
public:
    using Task = std::function<void()>;

    TaskQueue() : m_running(true) {
        m_worker = std::thread([this]() {
            while (m_running) {
                Task task;
                {
                    std::unique_lock<std::mutex> lock(m_mutex);
                    m_cv.wait(lock, [this]() {
                        return !m_running || !m_tasks.empty();
                    });

                    if (!m_running && m_tasks.empty()) {
                        return;
                    }

                    task = std::move(m_tasks.front());
                    m_tasks.pop();
                }

                if (task) {
                    task();
                }
            }
        });
    }

    ~TaskQueue() {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_running = false;
        }
        m_cv.notify_all();
        if (m_worker.joinable()) {
            m_worker.join();
        }
    }

    template <typename TaskFn, typename CallbackFn>
    void Enqueue(TaskFn&& task, CallbackFn&& callback) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_tasks.push([task = std::forward<TaskFn>(task), callback = std::forward<CallbackFn>(callback)]() mutable {
            using ResultType = decltype(task());
            if constexpr (std::is_void_v<ResultType>) {
                task();
                Events::RunOnUIThread([callback = std::move(callback)]() mutable {
                    callback();
                });
            } else {
                auto result = task();
                Events::RunOnUIThread([callback = std::move(callback), result = std::move(result)]() mutable {
                    callback(std::move(result));
                });
            }
        });
        m_cv.notify_one();
    }

    size_t PendingTasks() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_tasks.size();
    }

private:
    std::atomic<bool> m_running;
    std::queue<Task> m_tasks;
    mutable std::mutex m_mutex;
    std::condition_variable m_cv;
    std::thread m_worker;
};

inline TaskQueue& GlobalQueue() {
    static TaskQueue s_queue;
    return s_queue;
}

} // namespace wxHelpers::Tasks
