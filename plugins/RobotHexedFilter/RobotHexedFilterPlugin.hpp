/*
 *  Robot Audio Plugins
 *  
 *  Copyright (C) 2021      Martin Bångens
 *  Copyright (c) 2013-2014 Pascal Gauthier
 *  Copyright (c) 2013-2014 Filatov Vadim
 *
 *  Filter taken from the Obxd project :
 *    https://github.com/asb2m10/dexed
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

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------

class RobotHexedFilterPlugin : public Plugin
{
public:
    enum Parameters
    {
        paramCutOff = 0,
        paramRes,
        paramMode,
        paramWet,
        paramCount
    };

    RobotHexedFilterPlugin();

protected:
    // -------------------------------------------------------------------
    // Information

    const char* getLabel() const noexcept override
    {
        return "Robot Hexed Filter";
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

    float fCutOff = 100.0;
    float fRes    = 0.0;
    float fDuck   = 1.0;
    float fMode   = 4.0;
    float fWet    = 0.0;

    float parameterSurge(float x, float n);

    uint32_t fSamplesFallCutOff = 0;
    bool     fCutOffFall        = false;
    float    fChangeCutOff      = 0.0f;

    uint32_t fSamplesFallRes    = 0;
    bool     fResFall           = false;
    float    fChangeRes         = 0.0f;

    uint32_t fSamplesFallDuck   = 0;
    bool     fDuckFall          = false;
    float    fChangeDuck        = 0.0f;

    uint32_t fSamplesFallWet    = 0;
    bool     fWetFall           = false;
    float    fChangeWet         = 0.0f;

    uint32_t fFrames = 0;

    // -------------------------------------------------------------------
    // Dsp 

    float fCutOffOld, fResOld, fWetOld, fDuckOld, fWetVol;

    float s1[2],s2[2],s3[2],s4[2];
    float fSampleRate;
    float fSampleRateInv;
    float d[2], c[2];
    float R24;
    float rcor24,rcor24Inv;
    float bright, ringc;

    // 24 db multimode
    float mmt; //TODO
    int   mmch = 4;

    float uiCutoff, uiReso;

    float dc_tmp[2];
    float dc_r;

    float hexed_tanh(float x);
    float logsc(float param, const float min, const float max, const float rolloff);
    float tptpc(float& state, float inp, float cutoff);
    float tptlpupw(float& state , float inp, float cutoff, float srInv);
    float NR24(float sample, float g, float lpc, bool chan );
    float hexed_filter_process(float x, bool chan);

    // -------------------------------------------------------------------

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RobotHexedFilterPlugin)
};

// -----------------------------------------------------------------------

END_NAMESPACE_DISTRHO

#endif // ROBOT_HEXED_FILTER_PLUGIN_HPP_INCLUDED
 
