/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * For a full copy of the license see the LICENSE file.
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
        return "noisecode3";
    }

    const char* getHomePage() const override
    {
        return "robotaudio.site.non";
    }

    const char* getLicense() const noexcept override
    {
        return "LGPL";
    }

    uint32_t getVersion() const noexcept override
    {
        return d_version(1, 0, 0);
    }

    int64_t getUniqueId() const noexcept override
    {
        return d_cconst('R', '3', 'M', 'f');
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
    
    //MoogLadder

    //Parameters
    float fFreq = 22000.0f;
    float fRes  = 0.0f;
 
    uint8_t msToSamples= 48; // could be needed later


    float fSampleRate, fAcr, fTune, fFreqOld, fResOld;


    float fDelay[2][6];
    float fTanhstg[2][3];
    
    float moog_tanh(float x);
    void  moog_ladder_tune();
    float moog_ladder_process(float in, bool chan);
    //

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RobotMoogFilterPlugin)
};

// -----------------------------------------------------------------------

END_NAMESPACE_DISTRHO

#endif // ROBOT_MOOG_FILTER_PLUGIN_HPP_INCLUDED
 
