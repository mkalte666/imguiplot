/*
 * Copyright (c) 2020 Malte Kießling
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "imguiplot.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include <sstream>
#include <stack>

template <class T>
std::string toStringPrecision(T value, long precision)
{
    std::stringstream ss;
    ss.precision(precision);
    ss << value;
    return ss.str();
}

struct InternalPlotConfig {
    ImVec2 labelSize = {};
    ImRect frameBb = {};
    ImRect innerBb = {};
    ImRect totalBb = {};
    bool skipped = false;
    ImGuiWindow* window = nullptr;
};

static std::stack<PlotConfig> gConfigStack;
static std::stack<InternalPlotConfig> gInternalConfigStack;

void BeginPlot(const PlotConfig& config) noexcept
{
    gConfigStack.push(config);
    InternalPlotConfig internalConfig;
    internalConfig.window = ImGui::GetCurrentWindow();
    internalConfig.skipped = internalConfig.window->SkipItems;

    if (internalConfig.skipped) {
        return;
    }

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    internalConfig.labelSize = ImGui::CalcTextSize(config.label.c_str(), NULL, true);
    internalConfig.frameBb = ImRect(internalConfig.window->DC.CursorPos, internalConfig.window->DC.CursorPos + config.size);
    internalConfig.innerBb = ImRect(internalConfig.frameBb.Min + style.FramePadding, internalConfig.frameBb.Max - style.FramePadding);
    internalConfig.totalBb = ImRect(internalConfig.frameBb.Min, internalConfig.frameBb.Max + ImVec2(internalConfig.labelSize.x > 0.0f ? style.ItemInnerSpacing.x + internalConfig.labelSize.x : 0.0f, 10.0F));
    ImGui::RenderFrame(internalConfig.frameBb.Min, internalConfig.frameBb.Max, ImGui::GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);

    auto xGridVals = config.xAxisConfig.calcGridValues();
    auto yGridVals = config.yAxisConfig.calcGridValues();

    for (auto&& xGridVal : xGridVals) {
        auto pixelX = config.xAxisConfig.valueToPixel(xGridVal, internalConfig.innerBb.GetWidth());
        ImVec2 p0 = internalConfig.innerBb.Min + ImVec2(pixelX, 0.0);
        ImVec2 p1 = internalConfig.innerBb.Min + ImVec2(pixelX, internalConfig.innerBb.GetHeight());
        internalConfig.window->DrawList->AddLine(p0, p1, 0x99999999);
        internalConfig.window->DrawList->AddText(p1, 0x99999999, toStringPrecision(xGridVal, 2).c_str());
    }

    for (auto&& yGridVal : yGridVals) {
        auto pixelY = config.yAxisConfig.valueToPixel(yGridVal, internalConfig.innerBb.GetHeight());
        ImVec2 p0 = internalConfig.innerBb.Min + ImVec2(0.0, internalConfig.innerBb.GetHeight()-pixelY);
        ImVec2 p1 = internalConfig.innerBb.Min + ImVec2(internalConfig.innerBb.GetWidth(), internalConfig.innerBb.GetHeight()-pixelY);
        internalConfig.window->DrawList->AddLine(p0, p1, 0x99999999);
        internalConfig.window->DrawList->AddText(p0, 0x99999999, toStringPrecision(yGridVal, 2).c_str());
    }

    gInternalConfigStack.push(internalConfig);
}

void Plot(const PlotSourceConfig& sourceConfig, const PlotCallback& callback) noexcept
{
    IM_ASSERT_USER_ERROR(!gConfigStack.empty(), "BeginPlot() needs to be called before Plot()");
    IM_ASSERT(gConfigStack.size() == gInternalConfigStack.size());
    auto config = gConfigStack.top();
    auto internalConfig = gInternalConfigStack.top();

    const ImGuiID id = internalConfig.window->GetID(config.label.c_str());
    const bool hovered = ImGui::ItemHoverable(internalConfig.frameBb, id);

    if (sourceConfig.count < 2u) {
        return;
    }


    float lastX = 0.0F;
    float lastY = 0.0F;
    for (int x = 0; x < static_cast<int>(internalConfig.innerBb.GetWidth()); x++) {
        auto newX = static_cast<float>(x);
        auto xValue = config.xAxisConfig.pixelToValue(newX, internalConfig.innerBb.GetWidth());
        size_t arrayIndex = sourceConfig.valueToArrayIndex(xValue);
        double yValue = callback(arrayIndex);
        auto newY = config.yAxisConfig.valueToPixel(yValue, internalConfig.innerBb.GetHeight());

        if (x != 0 && config.xAxisConfig.isInAxisRange(xValue)) {
            ImVec2 pos1 = internalConfig.innerBb.Min + ImVec2(static_cast<float>(newX), internalConfig.innerBb.GetHeight() - static_cast<float>(newY));
            ImVec2 pos0 = internalConfig.innerBb.Min + ImVec2(static_cast<float>(lastX), internalConfig.innerBb.GetHeight() - static_cast<float>(lastY));
            internalConfig.window->DrawList->AddLine(pos0, pos1, 0xFFFFFFFFu);
        }

        lastX = newX;
        lastY = newY;
    }

    if (hovered && internalConfig.innerBb.Contains(GImGui->IO.MousePos)) {
        auto x = GImGui->IO.MousePos.x - internalConfig.innerBb.Min.x;
        auto xVal = config.xAxisConfig.pixelToValue(x, internalConfig.innerBb.GetWidth());
        auto index = sourceConfig.valueToArrayIndex(xVal);
        double v = callback(index);

        ImVec2 pos0 = internalConfig.innerBb.Min;
        pos0.x = GImGui->IO.MousePos.x;
        ImVec2 pos1 = internalConfig.innerBb.Max;
        pos1.x = GImGui->IO.MousePos.x;
        internalConfig.window->DrawList->AddLine(pos0, pos1, 0xFFFFFFFFu);

        ImGui::SetTooltip("%f [%zu]->%f", xVal, index, v);
    }
}

void EndPlot() noexcept
{
    IM_ASSERT_USER_ERROR(!gConfigStack.empty(), "BeginPlot() needs to be called before EndPlot()");
    IM_ASSERT(gConfigStack.size() == gInternalConfigStack.size());
    auto config = gConfigStack.top();
    auto internalConfig = gInternalConfigStack.top();

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;

    ImGui::ItemSize(internalConfig.totalBb, style.FramePadding.y);
    if (!ImGui::ItemAdd(internalConfig.totalBb, 0, &internalConfig.frameBb)) {
        return;
    }

    gConfigStack.pop();
    gInternalConfigStack.pop();
}