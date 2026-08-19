#pragma once

#include <wx/wx.h>
#include <wx/timer.h>
#include <functional>
#include <chrono>
#include <cmath>
#include <memory>
#include <algorithm>

namespace wxHelpers::Animation {

// Easing function type
using EasingFunc = std::function<double(double t)>;

namespace Easing {
    inline double Linear(double t) { return t; }
    
    inline double EaseInQuad(double t) { return t * t; }
    inline double EaseOutQuad(double t) { return t * (2.0 - t); }
    inline double EaseInOutQuad(double t) { return t < 0.5 ? 2.0 * t * t : -1.0 + (4.0 - 2.0 * t) * t; }

    inline double EaseInCubic(double t) { return t * t * t; }
    inline double EaseOutCubic(double t) { double f = t - 1.0; return f * f * f + 1.0; }

    inline double EaseOutBounce(double t) {
        const double n1 = 7.5625;
        const double d1 = 2.75;
        if (t < 1.0 / d1) {
            return n1 * t * t;
        } else if (t < 2.0 / d1) {
            t -= 1.5 / d1;
            return n1 * t * t + 0.75;
        } else if (t < 2.5 / d1) {
            t -= 2.25 / d1;
            return n1 * t * t + 0.9375;
        } else {
            t -= 2.625 / d1;
            return n1 * t * t + 0.984375;
        }
    }
}

class TweenAnimation : public wxTimer {
public:
    TweenAnimation(double startVal, double endVal, int durationMs,
                   EasingFunc easing,
                   std::function<void(double)> onUpdate,
                   std::function<void()> onComplete = nullptr)
        : m_startVal(startVal),
          m_endVal(endVal),
          m_durationMs(std::max(durationMs, 16)),
          m_easing(easing ? std::move(easing) : Easing::EaseInOutQuad),
          m_onUpdate(std::move(onUpdate)),
          m_onComplete(std::move(onComplete)),
          m_startTime(std::chrono::steady_clock::now()) {
        Start(16); // ~60 FPS
    }

    void Notify() override {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_startTime).count();
        double progress = std::clamp(static_cast<double>(elapsed) / m_durationMs, 0.0, 1.0);
        double easedProgress = m_easing(progress);
        double currentVal = m_startVal + (m_endVal - m_startVal) * easedProgress;

        if (m_onUpdate) {
            m_onUpdate(currentVal);
        }

        if (progress >= 1.0) {
            Stop();
            if (m_onComplete) {
                m_onComplete();
            }
            delete this;
        }
    }

private:
    double m_startVal;
    double m_endVal;
    int m_durationMs;
    EasingFunc m_easing;
    std::function<void(double)> m_onUpdate;
    std::function<void()> m_onComplete;
    std::chrono::steady_clock::time_point m_startTime;
};

// Start a smooth tween animation
inline void Animate(double from, double to, int durationMs,
                    EasingFunc easing,
                    std::function<void(double)> onUpdate,
                    std::function<void()> onComplete = nullptr) {
    new TweenAnimation(from, to, durationMs, std::move(easing), std::move(onUpdate), std::move(onComplete));
}

inline void Animate(double from, double to, int durationMs,
                    std::function<void(double)> onUpdate,
                    std::function<void()> onComplete = nullptr) {
    Animate(from, to, durationMs, Easing::EaseInOutQuad, std::move(onUpdate), std::move(onComplete));
}

} // namespace wxHelpers::Animation
