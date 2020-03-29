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

#ifndef imguiplot_imguiplotconfig_h
#define imguiplot_imguiplotconfig_h

#include <string>
#include <vector>

#include <imgui.h>

/**
 * \brief Config for a single axis.
 */
struct AxisConfig {
    /// axis label
    std::string label = "";
    /// axis minimum value
    double min = 0.0;
    /// axis maximum value
    double max = 1.0;
    /// axis grid interval. Set to non-zero value to draw the grid
    double gridInterval = 0.0;
    /// axis grid hint
    double gridHint = 0.0;
    /// set to true to enable logscale on the x axis. min must be non-zero!
    bool enableLogScale = false;

    /// move a 0..1 range into logscale
    [[nodiscard]] double logConvert(double value) const noexcept;
    /// move a 0..1 back from logscale
    [[nodiscard]] double logConvertBack(double value) const noexcept;
    /// converts a pixel to a value for use in array access etc.
    [[nodiscard]] double pixelToValue(float pixel, float pixelRange) const noexcept;
    /// converts a value to a pixel
    [[nodiscard]] float valueToPixel(double value, float pixelRange) const noexcept;
    /// checks if a value is in axis range
    [[nodiscard]] bool isInAxisRange(double value) const noexcept;
    /// calculate axis
    [[nodiscard]] std::vector<double> calcGridValues() const noexcept;
};

/**
 * \brief Configures a plot and its axis
 */
struct PlotConfig {
    /// Label/Name of the plot. Used as ID
    std::string label = "Plot";
    /// Size of the plot. Set to 0.0F for it to try fitting the window
    ImVec2 size = ImVec2(0.0F, 0.0F);

    /// X axis config
    AxisConfig xAxisConfig = {};
    /// Y axis config
    AxisConfig yAxisConfig = {};
};

/**
 * \brief Config for a source
 */
struct PlotSourceConfig {
    /// if this plot is active. Only affects if a tooltip is drawn
    bool active = true;
    /// number of elements in source
    size_t count = 0;
    /// on source[0], this is the value on the x axis
    double xMin = 0.0;
    /// on source[count-1], this is the value on the x axis
    double xMax = 0.0;

    /// transform a value to an array index
    [[nodiscard]] size_t valueToArrayIndex(double value) const noexcept;

    /// transform an array index to a value
    [[nodiscard]] double arrayIndexToValue(size_t arrayIndex) const noexcept;

    /// color
    ImColor color = 0xFFFFFFFF;
};

#endif // imguiplot_imguiplot_h