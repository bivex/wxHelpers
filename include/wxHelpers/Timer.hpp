#pragma once

#include <wx/wx.h>
#include <wx/timer.h>
#include <wx/app.h>
#include <functional>
#include <memory>
#include <chrono>
#include <tuple>

namespace wxHelpers::Timer {

class LambdaTimer : public wxTimer {
public:
    explicit LambdaTimer(std::function<void()> callback, bool oneShot = false)
        : m_callback(std::move(callback)), m_oneShot(oneShot) {}

    void Notify() override {
        if (m_callback) {
            m_callback();
        }
        if (m_oneShot) {
            Stop();
            if (wxTheApp) {
                wxTheApp->CallAfter([this]() {
                    delete this;
                });
            }
        }
    }

private:
    std::function<void()> m_callback;
    bool m_oneShot;
};

class IntervalHandle {
public:
    explicit IntervalHandle(wxTimer* timer) : m_timer(timer) {}
    ~IntervalHandle() {
        Stop();
        if (m_timer) {
            if (wxTheApp) {
                wxTheApp->CallAfter([t = m_timer]() {
                    delete t;
                });
            } else {
                delete m_timer;
            }
            m_timer = nullptr;
        }
    }

    void Stop() {
        if (m_timer && m_timer->IsRunning()) {
            m_timer->Stop();
        }
    }

    void Start(int milliseconds = -1) {
        if (m_timer) {
            m_timer->Start(milliseconds > 0 ? milliseconds : m_timer->GetInterval());
        }
    }

    bool IsRunning() const {
        return m_timer && m_timer->IsRunning();
    }

    int GetInterval() const {
        return m_timer ? m_timer->GetInterval() : 0;
    }

private:
    wxTimer* m_timer;
};

// Execute callback once after delay in milliseconds
inline void SetTimeout(int milliseconds, std::function<void()> callback) {
    auto* timer = new LambdaTimer(std::move(callback), true);
    timer->StartOnce(milliseconds);
}

// Execute callback once after delay with automatic weak reference lifetime safeguard
template <typename TWindow, typename Fn>
inline void SetTimeout(TWindow* context, int milliseconds, Fn&& callback) {
    wxWeakRef<TWindow> weakRef(context);
    SetTimeout(milliseconds, [weakRef, cb = std::forward<Fn>(callback)]() {
        if (weakRef) {
            cb(weakRef.get());
        }
    });
}

// Execute callback repeatedly every interval in milliseconds
inline std::shared_ptr<IntervalHandle> SetInterval(int milliseconds, std::function<void()> callback) {
    auto* timer = new LambdaTimer(std::move(callback), false);
    timer->Start(milliseconds);
    return std::make_shared<IntervalHandle>(timer);
}

// Execute callback repeatedly with automatic weak reference lifetime safeguard
template <typename TWindow, typename Fn>
inline std::shared_ptr<IntervalHandle> SetInterval(TWindow* context, int milliseconds, Fn&& callback) {
    wxWeakRef<TWindow> weakRef(context);
    return SetInterval(milliseconds, [weakRef, cb = std::forward<Fn>(callback)]() {
        if (weakRef) {
            cb(weakRef.get());
        }
    });
}

// Returns a debounced version of a callable that delays execution until after `delayMs` has elapsed
template <typename... Args, typename Fn>
inline auto Debounce(int delayMs, Fn&& fn) {
    auto timer = std::make_shared<wxTimer*>();
    auto func = std::function<void(Args...)>(std::forward<Fn>(fn));

    return [delayMs, func, timer](Args... args) mutable {
        if (*timer) {
            (*timer)->Stop();
            if (wxTheApp) {
                wxTheApp->CallAfter([t = *timer]() {
                    delete t;
                });
            }
            *timer = nullptr;
        }
        
        auto tupleArgs = std::make_tuple(args...);
        auto* newTimer = new LambdaTimer([func, tupleArgs]() {
            std::apply(func, tupleArgs);
        }, true);
        *timer = newTimer;
        newTimer->StartOnce(delayMs);
    };
}

} // namespace wxHelpers::Timer
