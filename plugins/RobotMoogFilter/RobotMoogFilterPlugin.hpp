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
        return "RobotMoogFilterExperimental";
    }

    const char* getDescription() const override
    {
        return "Moog filter for reptiles";
    }

    const char* getMaker() const noexcept override
    {
        return "noisecode3";
    }

    const char* getHomePage() const override
    {
        return "Robot.home.page.missing";
    }

    const char* getLicense() const noexcept override
    {
        return "LGPL";
    }

    uint32_t getVersion() const noexcept override
    {
        return d_version(0, 0, 1);
    }

    int64_t getUniqueId() const noexcept override
    {
        return d_cconst('R', 'b', 'F', 'l');
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
    float fFreq = 20000.0f;
    float fRes  = 0.4f;
    float fSampleRate, fAcr, fTune;

    float fDelay[2][6], fTanhstg[2][3]; // stereo tmp
    
    float moog_tanh(float x);
    float moog_process(float in, bool chan);
    //

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RobotMoogFilterPlugin)
};

// -----------------------------------------------------------------------

END_NAMESPACE_DISTRHO

#endif // ROBOT_MOOG_FILTER_PLUGIN_HPP_INCLUDED
 
