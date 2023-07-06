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

#ifndef ROBOT_MOOG_FILTER_PLUGIN_HPP_INCLUDED
#define ROBOT_MOOG_FILTER_PLUGIN_HPP_INCLUDED

#include "DistrhoPlugin.hpp"

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------

class RobotMoogFilterPlugin : public Plugin
{
public:
    enum Parameters
    {
        paramFreq = 0,
        paramRes,
        paramWet,
        paramCount
    };

    RobotMoogFilterPlugin();

protected:
    // -------------------------------------------------------------------
    // Information

    const char* getLabel() const noexcept override
    {
        return "Robot Moog Filter";
    }

    const char* getDescription() const override
    {
        return "Moog filter";
    }

    const char* getMaker() const noexcept override
    {
        return "Robot Audio";
    }

    const char* getHomePage() const override
    {
        return "https://github.com/noisecode3/ra-plugins#moog-filter";
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
        return d_cconst('r', 'B', 'm', 'F');
    }

    // -------------------------------------------------------------------
    // Init

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

    float fFreq = 100.0f;
    float fRes  = 0.0f;
    float fWet  = 0.0f;

    uint32_t fSamplesFallFreq = 0;
    bool     fFreqFall = false;
    float    fChangeFreq = 0.0f;

    uint32_t fSamplesFallRes = 0;
    bool     fResFall = false;
    float    fChangeRes = 0.0f;

    uint32_t fSamplesFallWet = 0;
    bool     fWetFall = false;
    float    fChangeWet = 0.0f;

    uint32_t fFrames;

    // -------------------------------------------------------------------
    // Dsp 

    float fSampleRate, fAcr, fTune, fWetVol, fFreqOld, fResOld, fWetOld;

    float fDelay[2][6];
    float fTanhstg[2][3];

    float logsc(float param, const float min, const float max, const float rolloff);
    float moog_tanh(float x);
    void  moog_ladder_tune(float freq);
    float moog_ladder_process(float in, bool chan);

    // -------------------------------------------------------------------

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RobotMoogFilterPlugin)
};

// -----------------------------------------------------------------------

END_NAMESPACE_DISTRHO

#endif // ROBOT_MOOG_FILTER_PLUGIN_HPP_INCLUDED
 
