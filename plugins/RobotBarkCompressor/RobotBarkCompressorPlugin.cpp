/*
 *  Robot Audio Plugins
 *  
 *  Copyright (C) 2021 Martin Bångens
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

#include "RobotBarkCompressorPlugin.hpp"

const float PI_F = 3.1415927410125732421875f;
const float E_F  = 2.7182818284590452353602f;

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------

RobotBarkCompressorPlugin::RobotBarkCompressorPlugin()
    : Plugin(paramCount, 1, 0) // parameters, program, states
{
    // set default values
    loadProgram(0);
}

// -----------------------------------------------------------------------
// Init

void RobotBarkCompressorPlugin::initParameter(uint32_t index, Parameter& parameter)
{
    switch (index)
    {
    case paramAttack:
        parameter.hints      = kParameterIsAutomable;
        parameter.name       = "Attack";
        parameter.symbol     = "at";
        parameter.unit       = "ms";
        parameter.ranges.def = 10.0f;
        parameter.ranges.min = 0.001f;
        parameter.ranges.max = 500.0f;
        break;

    case paramRelease:
        parameter.hints      = kParameterIsAutomable;
        parameter.name       = "Release";
        parameter.symbol     = "re";
        parameter.unit       = "ms";
        parameter.ranges.def = 100.0f;
        parameter.ranges.min = 0.001f;
        parameter.ranges.max = 600.0f;
        break;

    case paramThreshold:
        parameter.hints      = kParameterIsAutomable;
        parameter.name       = "Threshold";
        parameter.symbol     = "th";
        parameter.unit       = "db";
        parameter.ranges.def = 0.0f;
        parameter.ranges.min = -40.0f;
        parameter.ranges.max = 0.0f;
        break;

    case paramRatio:
        parameter.hints      = kParameterIsAutomable;
        parameter.name       = "Ratio";
        parameter.symbol     = "ra";
        parameter.unit       = "";
        parameter.ranges.def = 1.0f;
        parameter.ranges.min = 1.0f;
        parameter.ranges.max = 12.0f;
        break;

    case paramMakeUpGain:
        parameter.hints      = kParameterIsAutomable;
        parameter.name       = "MakeUpGain";
        parameter.symbol     = "mug";
        parameter.unit       = "db";
        parameter.ranges.def = 0.0f;
        parameter.ranges.min = -4.0f;
        parameter.ranges.max = 12.0f;
        break;

    }
}

void RobotBarkCompressorPlugin::initProgramName(uint32_t index, String& programName)
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

float RobotBarkCompressorPlugin::getParameterValue(uint32_t index) const
{
    switch (index)
    {
    case paramAttack:
        return fAttack;

	case paramRelease:
		return fRelease;

	case paramThreshold:
		return fThreshold;

	case paramRatio:
		return fRatio;

	case paramMakeUpGain:
		return fMakeUpGain;

    default:
        return 0.0f;
    }
}

void RobotBarkCompressorPlugin::setParameterValue(uint32_t index, float value)
{
    if (getSampleRate() <= 0.0)
        return;

    switch (index)
    {
    case paramAttack:
        fAttack        = value;
        cAT            = exp(-1 * 1000 / fAttack / getSampleRate());
        break;

    case paramRelease:
        fRelease       = value;
        cRT            = exp(-1 * 1000 / fRelease / getSampleRate());
        break;

    case paramThreshold:
        fThreshold      = value;
        break;

    case paramRatio:
        fRatio          = value;
        break;

    case paramMakeUpGain:
        fMakeUpGain     = value;
        break;

    }
}

void RobotBarkCompressorPlugin::loadProgram(uint32_t index)
{
    switch (index)
    {
    case 0:
        // Default
		fAttack     = 10.0;
		fRelease    = 100.0;
		fThreshold  = 0.0;
		fRatio      = 1.0;
		fMakeUpGain = 0.0;
        activate();
        break;
    }
}

// -----------------------------------------------------------------------
// Process

void RobotBarkCompressorPlugin::activate()
{

	cAT = exp(-1 * 1000 / fAttack / getSampleRate());
	cRT = exp(-1 * 1000 / fRelease / getSampleRate());

	state1 = state2 = 0;
}

void RobotBarkCompressorPlugin::deactivate()
{

}

void RobotBarkCompressorPlugin::run(const float** inputs, float** outputs, uint32_t frames, const MidiEvent* events, uint32_t eventCount)
{
	const float* in1  = inputs[0];
    const float* in2  = inputs[1];
    float*       out1 = outputs[0];
    float*       out2 = outputs[1];

	
	float slope = 1 - 1 / fRatio;

	for (uint32_t i = 0; i < frames; ++i) {

		//left
		
		float sideInput1 = fabs(in1[i]);
		float c1 = sideInput1 >= state1 ? cAT : cRT;           // When  sideInput is bigger then state it compresses and have an attack
		float env1 = sideInput1 + c1 * (state1 - sideInput1);  // becues sideInput was bigger and in absolute value sideInput makes it negative.
		float env_db1 = 10*log10(env1);                        // the delta value (previous env) makes it smaller and smaller or bigger and bigger (if negative, it decompresses logically)
		state1 = env1;                                         // until the number of values defind by attack or realease(in time domain) multiplied by delta sideInput ends 

		float gain1 = slope * (fThreshold - env_db1);          // if it is under the threshold nothing happens, but if above gain gets applied in absolute value
		gain1 = fmin(0.f, gain1);
		gain1 = pow(10, (gain1/20));


		out1[i] = in1[i] * gain1 * (pow(10, (fMakeUpGain/20)));

		//right

		float sideInput2 = fabs(in2[i]);
		float c2 = sideInput2 >= state2 ? cAT : cRT;
		float env2 = sideInput2 + c2 * (state2 - sideInput2);
		float env_db2 = 10*log10(env2);
		state2 = env2;

		float gain2 = slope * (fThreshold - env_db2);
		gain2 = fmin(0.f, gain2);
		gain2 = pow(10, (gain2/20));


		out2[i] = in2[i] * gain2 * (pow(10, (fMakeUpGain/20)));

	}

}

// -----------------------------------------------------------------------

Plugin* createPlugin()
{
    return new RobotBarkCompressorPlugin();
}

// -----------------------------------------------------------------------

END_NAMESPACE_DISTRHO
