#pragma once

#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include "Color.hpp"
#include "Canvas.hpp"
#include <vector>
#include <algorithm>
#include <numeric>

namespace wxHelpers::Charts {

class Sparkline : public wxPanel {
public:
    Sparkline(wxWindow* parent, const std::vector<double>& values = {}, 
              const wxColour& lineColor = Color::Palette::Blue500,
              wxSize size = wxSize(200, 60))
        : wxPanel(parent, wxID_ANY, wxDefaultPosition, size, wxFULL_REPAINT_ON_RESIZE),
          m_values(values), m_lineColor(lineColor) {
        
        SetBackgroundStyle(wxBG_STYLE_PAINT);

        Bind(wxEVT_PAINT, [this](wxPaintEvent&) {
            wxAutoBufferedPaintDC dc(this);
            dc.Clear();
            wxSize sz = GetClientSize();

            // Background
            Canvas::Draw::RoundedRect(dc, wxRect(0, 0, sz.x, sz.y), 8, Color::Palette::Slate900);

            if (m_values.size() < 2) return;

            auto [minIt, maxIt] = std::minmax_element(m_values.begin(), m_values.end());
            double minVal = *minIt;
            double maxVal = *maxIt;
            if (minVal == maxVal) {
                maxVal += 1.0;
            }

            int pad = 10;
            int drawW = sz.x - pad * 2;
            int drawH = sz.y - pad * 2;

            std::vector<wxPoint> points;
            for (size_t i = 0; i < m_values.size(); ++i) {
                int x = pad + static_cast<int>(i * drawW / (m_values.size() - 1));
                double norm = (m_values[i] - minVal) / (maxVal - minVal);
                int y = pad + drawH - static_cast<int>(norm * drawH);
                points.emplace_back(x, y);
            }

            // Draw line
            dc.SetPen(wxPen(m_lineColor, 2));
            for (size_t i = 0; i < points.size() - 1; ++i) {
                dc.DrawLine(points[i], points[i + 1]);
            }

            // Draw endpoint marker dot
            dc.SetBrush(wxBrush(*wxWHITE));
            dc.SetPen(wxPen(m_lineColor, 2));
            dc.DrawCircle(points.back(), 4);
        });
    }

    void SetData(const std::vector<double>& values) {
        m_values = values;
        Refresh();
    }

    void PushValue(double val) {
        m_values.push_back(val);
        if (m_values.size() > 30) {
            m_values.erase(m_values.begin());
        }
        Refresh();
    }

private:
    std::vector<double> m_values;
    wxColour m_lineColor;
};

class BarChart : public wxPanel {
public:
    struct BarItem {
        wxString label;
        double value;
        wxColour color;
    };

    BarChart(wxWindow* parent, const std::vector<BarItem>& items = {}, wxSize size = wxSize(300, 160))
        : wxPanel(parent, wxID_ANY, wxDefaultPosition, size, wxFULL_REPAINT_ON_RESIZE),
          m_items(items) {
        
        SetBackgroundStyle(wxBG_STYLE_PAINT);

        Bind(wxEVT_PAINT, [this](wxPaintEvent&) {
            wxAutoBufferedPaintDC dc(this);
            dc.Clear();
            wxSize sz = GetClientSize();

            Canvas::Draw::RoundedRect(dc, wxRect(0, 0, sz.x, sz.y), 8, Color::Palette::Slate900);

            if (m_items.empty()) return;

            double maxVal = 0;
            for (const auto& item : m_items) {
                if (item.value > maxVal) maxVal = item.value;
            }
            if (maxVal <= 0) maxVal = 1.0;

            int pad = 16;
            int bottomPad = 30;
            int drawW = sz.x - pad * 2;
            int drawH = sz.y - pad - bottomPad;

            int count = static_cast<int>(m_items.size());
            int barWidth = std::max(12, (drawW / count) - 10);

            wxFont font = GetFont();
            font.SetPointSize(9);
            dc.SetFont(font);

            for (int i = 0; i < count; ++i) {
                const auto& item = m_items[i];
                int x = pad + i * (drawW / count) + 5;
                int barH = static_cast<int>((item.value / maxVal) * drawH);
                int y = pad + drawH - barH;

                // Draw Bar
                Canvas::Draw::RoundedRect(dc, wxRect(x, y, barWidth, barH), 4, item.color);

                // Draw Label
                dc.SetTextForeground(Color::Palette::Slate200);
                wxSize labelSz = dc.GetTextExtent(item.label);
                dc.DrawText(item.label, x + (barWidth - labelSz.x) / 2, sz.y - bottomPad + 8);
            }
        });
    }

    void SetItems(const std::vector<BarItem>& items) {
        m_items = items;
        Refresh();
    }

private:
    std::vector<BarItem> m_items;
};

inline Sparkline* CreateSparkline(wxWindow* parent, const std::vector<double>& values = {}, 
                                  const wxColour& color = Color::Palette::Blue500) {
    return new Sparkline(parent, values, color);
}

inline BarChart* CreateBarChart(wxWindow* parent, const std::vector<BarChart::BarItem>& items = {}) {
    return new BarChart(parent, items);
}

} // namespace wxHelpers::Charts
