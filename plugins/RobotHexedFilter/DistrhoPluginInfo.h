/*
 *  Robot Audio Plugins
 *  Copyright (C) 2021  Martin Bångens
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

#ifndef DISTRHO_PLUGIN_INFO_H_INCLUDED
#define DISTRHO_PLUGIN_INFO_H_INCLUDED

#define DISTRHO_PLUGIN_BRAND   "Robot Audio"
#define DISTRHO_PLUGIN_NAME    "Robot Hexed Filter"
#define DISTRHO_PLUGIN_URI     "https://github.com/noisecode3/ra-plugins#hexed-filter"
#define DISTRHO_PLUGIN_CLAP_ID "robot.audio.Hexed.Filter"

#define DISTRHO_PLUGIN_NUM_INPUTS    2
#define DISTRHO_PLUGIN_NUM_OUTPUTS   2
#define DISTRHO_PLUGIN_HAS_UI        1
#define DISTRHO_PLUGIN_IS_RT_SAFE    1
#define DISTRHO_PLUGIN_WANT_PROGRAMS 1
#define DISTRHO_PLUGIN_IS_SYNTH 0
#define DISTRHO_PLUGIN_WANT_DIRECT_ACCESS 0
#define DISTRHO_PLUGIN_WANT_LATENCY 0
#define DISTRHO_PLUGIN_WANT_MIDI_INPUT 0
#define DISTRHO_PLUGIN_WANT_MIDI_OUTPUT 0
#define DISTRHO_PLUGIN_WANT_PARAMETER_VALUE_CHANGE_REQUEST 0
#define DISTRHO_PLUGIN_WANT_STATE 0
#define DISTRHO_PLUGIN_WANT_FULL_STATE 0
#define DISTRHO_PLUGIN_WANT_TIMEPOS 0
#define DISTRHO_UI_USE_CUSTOM 1
#define DISTRHO_UI_CUSTOM_INCLUDE_PATH "../../lib/DPF-Widgets/opengl/DearImGui.hpp"
#define DISTRHO_UI_CUSTOM_WIDGET_TYPE DGL_NAMESPACE::ImGuiTopLevelWidget
#define DISTRHO_UI_DEFAULT_WIDTH 600
#define DISTRHO_UI_DEFAULT_HEIGHT 400
#define DISTRHO_UI_USE_NANOVG 0
#define DISTRHO_UI_USER_RESIZABLE 1

#define DISTRHO_PLUGIN_CLAP_FEATURES   "audio-effect", "filter", "stereo"
#define DISTRHO_PLUGIN_LV2_CATEGORY    "lv2:LowpassPlugin"
#define DISTRHO_PLUGIN_VST3_CATEGORIES "Fx|Filter"

#endif // DISTRHO_PLUGIN_INFO_H_INCLUDED
