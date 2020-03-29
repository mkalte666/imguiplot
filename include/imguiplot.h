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

#ifndef imguiplot_imguiplot_h
#define imguiplot_imguiplot_h

#include "imgui.h"
#include "imguiplotconfig.h"
#include <functional>

using PlotCallback = std::function<double(size_t index)>;

/**
 * \brief Struct returned by Plot() that holds infos on if and where a plot was clicked on
 */
struct PlotClickInfo {
    /// if the plot was clicked.
    bool clicked = false;
    /// where, on the x axis, the plot was clicked
    double x = 0.0;
    /// the value of the plot at x
    double y = 0.0;
    /// the index where the click happend
    size_t index = 0;

    /// returns true if ckicked is true, for convenience
    explicit operator bool() const noexcept;
};

void BeginPlot(const PlotConfig& config) noexcept;
PlotClickInfo Plot(const PlotSourceConfig& sourceConfig, const PlotCallback& callback) noexcept;
void EndPlot() noexcept;

#endif // imguiplot_imguiplot_h