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

#include "RobotHexedFilterPlugin.hpp"

START_NAMESPACE_DISTRHO

// --------------------------------------------------------------------------------------------

RobotHexedFilterPlugin::RobotHexedFilterPlugin()
    : Plugin(paramCount, 1, 0), // parameters, program, states
      left(getSampleRate()),
      right(getSampleRate())
{
    // set default values
    loadProgram(0);
}

// --------------------------------------------------------------------------------------------
// Init

void RobotHexedFilterPlugin::initAudioPort(bool input, uint32_t index, AudioPort& port)
{
    port.groupId = kPortGroupStereo;

    Plugin::initAudioPort(input, index, port);
}

void RobotHexedFilterPlugin::initParameter(uint32_t index, Parameter& parameter)
{
    switch (index)
    {
    case paramCutOff:
        parameter.hints      = kParameterIsAutomatable | kParameterIsLogarithmic;
        parameter.name       = "CutOff";
        parameter.shortName  = "CutOff";
        parameter.symbol     = "freq";
        parameter.unit       = "%";
        parameter.ranges.def = 100.0f;
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 100.0f;
        break;

    case paramResonance:
        parameter.hints      = kParameterIsAutomatable | kParameterIsLogarithmic;
        parameter.name       = "Resonance";
        parameter.shortName  = "Resonance";
        parameter.symbol     = "res";
        parameter.unit       = "%";
        parameter.ranges.def = 0.0f;
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 100.0f;
        break;

    case paramMode:
        parameter.hints      = kParameterIsAutomatable | kParameterIsInteger;
        parameter.name       = "PoleMode";
        parameter.shortName  = "PoleMode";
        parameter.symbol     = "switch";
        parameter.unit       = "";
        parameter.ranges.def = 4;
        parameter.ranges.min = 1;
        parameter.ranges.max = 4;
        break;

    case paramWet:
        parameter.hints      = kParameterIsAutomatable | kParameterIsLogarithmic;
        parameter.name       = "Wet";
        parameter.shortName  = "Wet";
        parameter.symbol     = "percent";
        parameter.unit       = "%";
        parameter.ranges.def = 0.0f;
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 100.0f;
        break;

    }
}

void RobotHexedFilterPlugin::initProgramName(uint32_t index, String& programName)
{
    switch (index)
    {
        case 0:
            programName = "Default";
            break;
    }
}

// -----------------------------------------------------------------------
// Internal data

float RobotHexedFilterPlugin::getParameterValue(uint32_t index) const
{
    switch (index)
    {
    case paramCutOff:
        return fCutOff;

    case paramResonance:
        return fResonance;

    case paramMode:
        return fMode;

    case paramWet:
        return fWet;

    default:
        return 0.0f;
    }
}

void RobotHexedFilterPlugin::setParameterValue(uint32_t index, float value)
{
    if (getSampleRate() <= 0.0)
        return;

    switch (index)
    {
    case paramCutOff:
        fCutOff = value;
        cutoff = fCutOff*0.01;
        break;

    case paramResonance:
        fResonance = value;
        resonance = fResonance*0.01;
        break;

    case paramMode:
        fMode = value;
        break;

    case paramWet:
        fWet = value;
        wet = fWet*0.01;
        break;
    }
}

void RobotHexedFilterPlugin::loadProgram(uint32_t index)
{
    switch (index)
    {
    case 0:
        // Default
        fCutOff    = 100.0f;
        cutoff     = 1.0f;
        fResonance = 0.0f;
        resonance  = 0.0f;
        fMode      = 4;
        fWet       = 0.0f;
        wet        = 0.0f;
        activate();
        break;
    }
}

// -----------------------------------------------------------------------
// Process

void RobotHexedFilterPlugin::activate()
{
    left.flush(getSampleRate());
    left.setCutOff(cutoff);
    left.setResonance(resonance);
    wetLeft.setWet(wet);
    left.setMode(fMode);

    right.flush(getSampleRate());
    right.setCutOff(cutoff);
    right.setResonance(resonance);
    wetRight.setWet(wet);
    right.setMode(fMode);
}

void RobotHexedFilterPlugin::deactivate()
{
    //TODO maybe there could be someting done here to minimized the work on activate()
}


void RobotHexedFilterPlugin::run(const float** inputs, float** outputs, uint32_t frames)
{
    for (uint32_t i=0; i < frames; ++i)
    {
        float c = sCutOff.processChangeTrigger(cutoff , cutoff);
        if(0.0f!=c)
        {
            float fc = CutOffLPF.process(CutOffLI.process(c));
            left.setCutOff(fc);
            right.setCutOff(fc);

        }
        float r = sResonance.processChangeTrigger(resonance, resonance);
        if(0.0f!=r)
        {
            float fr = ResonanceLPF.process(ResonanceLI.process(r));
            left.setResonance(fr);
            right.setResonance(fr);
        }
        float m = sMode.processChangeTrigger(fMode, fMode);
        if(0.0f!=m)
        {
            float fm = ModeLI.process(fMode);
            left.setMode(fm);
            right.setMode(fm);
        }
        float w = sWet.processChangeTrigger(wet, wet);
        if(0.0f!=w)
        {
            float fw = WetLI.process(wet);
            wetLeft.setWet(fw);
            wetRight.setWet(fw);

        }
        outputs[0][i] = wetLeft.process(inputs[0][i], left.process(inputs[0][i]));
        outputs[1][i] = wetRight.process(inputs[1][i], right.process(inputs[1][i]));
    }
}

// -----------------------------------------------------------------------

Plugin* createPlugin()
{
    return new RobotHexedFilterPlugin();
}

// -----------------------------------------------------------------------

END_NAMESPACE_DISTRHO
