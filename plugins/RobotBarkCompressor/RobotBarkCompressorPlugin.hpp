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

#pragma  once

#include "DistrhoPlugin.hpp"
#include "kiss-fft/kiss_fftr.h"

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------

class RobotBarkCompressorPlugin : public Plugin
{
public:
    enum Parameters
    {
        paramAttack = 0,
        paramRelease,
        paramThreshold,
        paramRatio,
        paramMakeUpGain,
        paramCount
    };

    RobotBarkCompressorPlugin();
    ~RobotBarkCompressorPlugin();


protected:
    // -------------------------------------------------------------------
    // Information

    const char* getLabel() const noexcept override
    {
        return "Robot Bark Compressor";
    }

    const char* getDescription() const override
    {
        return "Compressor";
    }

    const char* getMaker() const noexcept override
    {
        return "Robot Audio";
    }

    const char* getHomePage() const override
    {
        return "https://github.com/noisecode3/ra-plugins#bark-compressor";
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
        return d_cconst('r', 'B', 'b', 'C');
    }

    // -------------------------------------------------------------------
    // Init

    void initParameter(uint32_t index, Parameter& parameter) override;
    void initProgramName(uint32_t index, String& programName);

    // -------------------------------------------------------------------
    // Internal data

    float getParameterValue(uint32_t index) const override;
    void  setParameterValue(uint32_t index, float value) override;
    void  loadProgram(uint32_t index);

    // -------------------------------------------------------------------
    // Process

    void activate() override;
    void deactivate() override;
    double goertzel(int size, float const *data, int sample_fq, int detect_fq);
    void run(const float** inputs, float** outputs, uint32_t frames, const MidiEvent* midiEvents, uint32_t midiEventCount);
    // -------------------------------------------------------------------


private:
    // -------------------------------------------------------------------
    // Parameters

    float fAttack     = 1.0f;
    float fRelease    = 36.0f;
    float fThreshold  = 0.0f;
    float fRatio      = 1.0f;
    float fMakeUpGain = 0.0f;

    // -------------------------------------------------------------------
    // Dsp

    float cAT;
    float cRT;
    float state1, state2;


    float hanningWindow[32];
    float gaussWindow[32];


    // -------------------------------------------------------------------

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RobotBarkCompressorPlugin)
};
// -----------------------------------------------------------------------

END_NAMESPACE_DISTRHO
