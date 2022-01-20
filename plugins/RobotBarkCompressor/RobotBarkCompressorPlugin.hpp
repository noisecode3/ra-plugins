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

#ifndef ROBOT_BARK_COMPRESSOR_PLUGIN_HPP_INCLUDED
#define ROBOT_BARK_COMPRESSOR_PLUGIN_HPP_INCLUDED

#include "DistrhoPlugin.hpp"
#include "kissfft/kiss_fft.h"

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
    void  loadProgram(uint32_t index) ;

    // -------------------------------------------------------------------
    // Process

    void activate() override;
    void deactivate() override;
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
    float posts1, posts2;
    float hz1, hz2;

    kiss_fft_cfg cfg1 = kiss_fft_alloc(2 ,0 ,NULL ,NULL);
    kiss_fft_cfg cfg2 = kiss_fft_alloc(2 ,0 ,NULL ,NULL);

    kiss_fft_cpx cx_in1[2], cx_out1[2], cx_in2[2], cx_out2[2];

    // Juggle with k=2 and N=2
    // frequency-domain data is stored from dc up to 2pi.
    // so cx_out[0] is the dc bin of the FFT and cx_out[N/2] is the Nyquist bin.
    // In this case cx_out[1]

    // -------------------------------------------------------------------

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RobotBarkCompressorPlugin)
};

// -----------------------------------------------------------------------

END_NAMESPACE_DISTRHO

#endif // ROBOT_MOOG_FILTER_PLUGIN_HPP_INCLUDED
 
