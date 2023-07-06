/*
 *  Robot Audio Plugins
 *
 *  Copyright (C) 2023      Martin Bångens
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

#ifndef ROBOT_HEXED_FILTER_PLUGIN_HPP_INCLUDED
#define ROBOT_HEXED_FILTER_PLUGIN_HPP_INCLUDED

#include "DistrhoPlugin.hpp"
#include "RobotHexedFilterDSP.hpp"
#include "wet.hpp"
#include "smooth.hpp"
#include "samplePlayer.hpp"

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------

class RobotHexedFilterPlugin : public Plugin
{
    enum Parameters
    {
        paramCutOff = 0,
        paramResonance,
        paramMode,
        paramWet,
        paramCount
    };


public:
    RobotHexedFilterPlugin();

protected:
    // -------------------------------------------------------------------
    // Information

    const char* getLabel() const noexcept override
    {
        return "RobotHexedFilter";
    }

    const char* getDescription() const override
    {
        return "Modified Dexed filter";
    }

    const char* getMaker() const noexcept override
    {
        return "Robot Audio";
    }

    const char* getHomePage() const override
    {
        return "https://github.com/noisecode3/ra-plugins#hexed-filter";
    }

    const char* getLicense() const noexcept override
    {
        return "GPL";
    }

    uint32_t getVersion() const noexcept override
    {
        return d_version(1, 0, 0);
    }

    int64_t getUniqueId() const noexcept override
    {
        return d_cconst('r', 'B', 'h', 'F');
    }

    // -------------------------------------------------------------------
    // Init

    void initAudioPort(bool input, uint32_t index, AudioPort& port) override;
    void initParameter(uint32_t index, Parameter& parameter) override;
    void initProgramName(uint32_t index, String& programName) override;

    // -------------------------------------------------------------------
    // Internal data

    float getParameterValue(uint32_t index) const override;
    void  setParameterValue(uint32_t index, float value) override;
    void  loadProgram(uint32_t index) override;

    // -------------------------------------------------------------------
    // Process

    void activate() override;
    void deactivate() override;
    void run(const float** inputs, float** outputs, uint32_t frames) override;

    // -------------------------------------------------------------------

private:
    // -------------------------------------------------------------------
    // Parameters

    LPFSmooth CutOffLPF    = LPFSmooth(21.32f,getSampleRate());
    LISmooth  CutOffLI     = LISmooth(21.34f,getSampleRate());
    LPFSmooth ResonanceLPF = LPFSmooth(21.32f,getSampleRate());
    LISmooth  ResonanceLI  = LISmooth(21.34f,getSampleRate());
    LISmooth  ModeLI       = LISmooth(21.34f,getSampleRate());
    LISmooth  WetLI        = LISmooth(21.34f,getSampleRate());

    float fCutOff   = 100.0;
    float cutoff    = fCutOff*0.01;
    float fResonance = 0.0;
    float resonance = 0.0;
    float fMode     = 4;
    float fWet      = 0.0; 
    float wet      = 0.0; 

    RobotBufferPlayer sCutOff = RobotBufferPlayer(getSampleRate(), 45, 1.0f);
    RobotBufferPlayer sResonance = RobotBufferPlayer(getSampleRate(), 45, 0.0f);
    RobotBufferPlayer sMode = RobotBufferPlayer(getSampleRate(), 24, 4.0f);
    RobotBufferPlayer sWet = RobotBufferPlayer(getSampleRate(), 24, 0.0f);
    // -------------------------------------------------------------------
    // Dsp 
    RobotHexedFilterDSP left;
    RobotHexedFilterDSP right;
    RobotWet wetLeft;
    RobotWet wetRight;
    float hexed_filter_process(float x, bool chan);
    // -------------------------------------------------------------------

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RobotHexedFilterPlugin)
};

// -----------------------------------------------------------------------

END_NAMESPACE_DISTRHO

#endif // ROBOT_HEXED_FILTER_PLUGIN_HPP_INCLUDED
