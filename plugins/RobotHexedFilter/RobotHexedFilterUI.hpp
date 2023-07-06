/*
 *  Robot Audio Plugins
 *  
 *  Copyright (C) 2022 Martin Bångens
 *
 *  Programing style originally from https://github.com/DISTRHO/DPF-Plugins
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef ROBOT_HEXED_FILTER_UI_HPP_INCLUDED
#define ROBOT_HEXED_FILTER_UI_HPP_INCLUDED

#include "DistrhoUI.hpp"
#include "src/DistrhoDefines.h"
#include "RobotHexedFilterArt.hpp"
#include "ResizeHandle.hpp"
#include "DearImGui.hpp"
#include "implot.h"
#include "implot_internal.h"
#include "lunasvg.h"

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------

class RobotHexedFilterUI : public UI
{
public:
    RobotHexedFilterUI();
    ~RobotHexedFilterUI();

protected:
    // -------------------------------------------------------------------
    // DSP Callbacks

    void parameterChanged(uint32_t index, float value) override;
    void programLoaded(uint32_t index) override;

    // -------------------------------------------------------------------
    // Widget Callbacks

    void onImGuiDisplay() override;

private:

    float fCutOff = 100.0f;
    float fRes    = 0.0f;
    float fMode   = 4.0f;
    float fWet    = 0.0f;

    ResizeHandle fResizeHandle;
    GLuint bgImageTexture;
    bool tabEditor=1;
    bool tabAbout=0;
    bool tabPresets=0;
    ImPlotContext* const context;
    const ImPlotFlags plotFlags = 0
            | ImPlotFlags_NoTitle
            | ImPlotFlags_NoMouseText
            | ImPlotFlags_NoMenus
            | ImPlotFlags_NoBoxSelect
            | ImPlotFlags_NoFrame
            | ImPlotFlags_Crosshairs;

    const ImPlotAxisFlags axisFlags = 0
            | ImPlotAxisFlags_NoMenus
            | ImPlotAxisFlags_NoSideSwitch
            | ImPlotAxisFlags_NoHighlight
            | ImPlotAxisFlags_Opposite
            | ImPlotAxisFlags_Lock;

    const ImPlotLegendFlags legendFlags = 0
            | ImPlotLegendFlags_NoMenus
            | ImPlotLegendFlags_Horizontal
            | ImPlotLegendFlags_NoButtons;

    int fftSize = 1024;
    std::vector<float> dataPsdOut;
    float lastWidth = 0.0f;
    float lastHeight = 0.0f;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RobotHexedFilterUI)
};

// -----------------------------------------------------------------------

END_NAMESPACE_DISTRHO

#endif // ROBOT_HEXED_FILTER_UI_HPP_INCLUDED
