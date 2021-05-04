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

#include "RobotMoogFilterPlugin.hpp"

#define PI_F 3.1415927410125732421875f

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------

RobotMoogFilterPlugin::RobotMoogFilterPlugin()
    : Plugin(paramCount, 1, 0) // 1 program, 0 states
{
    // set default values
    loadProgram(0);
}

// -----------------------------------------------------------------------
// Init

void RobotMoogFilterPlugin::initParameter(uint32_t index, Parameter& parameter)
{
    switch (index)
    {
    case paramFreq:
        parameter.hints      = kParameterIsAutomable;
        parameter.name       = "CutOffFreq";
        parameter.symbol     = "freq";
        parameter.unit       = "Hz";
        parameter.ranges.def = 20000.0f;
        parameter.ranges.min = 20.0f;
        parameter.ranges.max = 20000.0f;
        break;

    case paramRes:
        parameter.hints      = kParameterIsAutomable;
        parameter.name       = "Resonance";
        parameter.symbol     = "res";
        parameter.unit       = "L";
        parameter.ranges.def = 0.0f;
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 0.95f;
        break;

    }
}

void RobotMoogFilterPlugin::initProgramName(uint32_t index, String& programName)
{
    if (index != 0)
        return;

    programName = "Default";
}

// -----------------------------------------------------------------------
// Internal data

float RobotMoogFilterPlugin::getParameterValue(uint32_t index) const
{
    switch (index)
    {
    case paramFreq:
        return fFreq;
    case paramRes:
        return fRes;
    default:
        return 0.0f;
    }
}

void RobotMoogFilterPlugin::setParameterValue(uint32_t index, float value)
{
    if (getSampleRate() <= 0.0)
        return;

    switch (index)
    {
    case paramFreq:
        {
            fFreq  = value;

            float f, fc, fc2, fc3, fcr;

            fc        = (fFreq / fSampleRate);
            f         = 0.5f * fc;
            fc2       = fc * fc;
            fc3       = fc2 * fc2;

            fcr   = 1.8730f * fc3 + 0.4955f * fc2 - 0.6490f * fc + 0.9988f;
            fAcr  = -3.9364f * fc2 + 1.8409f * fc + 0.9968f;
            fTune = (1.0f - expf(-((2 * PI_F) * f * fcr))) / 0.000025; // 0.000025 = THERMAL
        }
        break;

    case paramRes:
        {
            fRes   = value;

            if(fRes < 0)
            {
                fRes = 0;
            }

            float f, fc, fc2, fc3, fcr;

            fc        = (fFreq / fSampleRate);
            f         = 0.5f * fc;
            fc2       = fc * fc;
            fc3       = fc2 * fc2;

            fcr   = 1.8730f * fc3 + 0.4955f * fc2 - 0.6490f * fc + 0.9988f;
            fAcr  = -3.9364f * fc2 + 1.8409f * fc + 0.9968f;
            fTune = (1.0f - expf(-((2 * PI_F) * f * fcr))) / 0.000025; // 0.000025 = THERMAL
        }
        break;
    }
}

void RobotMoogFilterPlugin::loadProgram(uint32_t index)
{
    if (index != 0)
        return;

    // Default values
    fFreq = 20000.0f;
    fRes = 0.0f;

    // reset filter values
    activate();
}

// -----------------------------------------------------------------------
// Process

void RobotMoogFilterPlugin::activate()
{
    fSampleRate = (float)getSampleRate();

    for(int i = 0; i < 6; i++)
    {
        fDelay[0][i]        = 0.0;
        fTanhstg[0][i % 3]  = 0.0;
        fDelay[1][i]        = 0.0;
        fTanhstg[1][i % 3]  = 0.0;
    }
    
    float f, fc, fc2, fc3, fcr;
    
    fc        = (fFreq / fSampleRate);
    f         = 0.5f * fc;
    fc2       = fc * fc;
    fc3       = fc2 * fc2;

    fcr   = 1.8730f * fc3 + 0.4955f * fc2 - 0.6490f * fc + 0.9988f;
    fAcr  = -3.9364f * fc2 + 1.8409f * fc + 0.9968f;
    fTune = (1.0f - expf(-((2 * PI_F) * f * fcr))) / 0.000025; // 0.000025 = THERMAL

}

void RobotMoogFilterPlugin::deactivate()
{
    // not sure if something needs to be done here
}

float RobotMoogFilterPlugin::moog_tanh(float x)
{
    int sign = 1;
    if(x < 0)
    {
        sign = -1;
        x    = -x;
        return x * sign;
    }
    else if(x >= 4.0f)
    {
        return sign;
    }
    else if(x < 0.5f)
    {
        return x * sign;
    }
    return sign * tanhf(x);
}

float RobotMoogFilterPlugin::moog_process(float in, bool chan)
{
    float  res4;
    float  stg[4];
    float  THERMAL = 0.000025;

    res4 = 4.0f * fRes * fAcr;

    for(int j = 0; j < 2; j++)
    {
        in -= res4 * fDelay[chan][5];
        fDelay[chan][0] = stg[0]
            = fDelay[chan][0] + fTune * (moog_tanh(in * THERMAL) - fTanhstg[chan][0]);
        for(int k = 1; k < 4; k++)
        {
            in     = stg[k - 1];
            stg[k] = fDelay[chan][k]
                     + fTune
                           * ((fTanhstg[chan][k - 1] = moog_tanh(in * THERMAL))
                              - (k != 3 ? fTanhstg[chan][k]
                                        : moog_tanh(fDelay[chan][k] * THERMAL)));
            fDelay[chan][k] = stg[k];
        }
        fDelay[chan][5] = (stg[3] + fDelay[chan][4]) * 0.5f;
        fDelay[chan][4] =  stg[3];
    }
    return fDelay[chan][5];
}

void RobotMoogFilterPlugin::run(const float** inputs, float** outputs, uint32_t frames)
{
    const float* in1  = inputs[0];
    const float* in2  = inputs[1];
    float*       out1 = outputs[0];
    float*       out2 = outputs[1];

    for (uint32_t i=0; i < frames; ++i)
    {
        out1[i] = moog_process(in1[i], 0);
        out2[i] = moog_process(in2[i], 1);
    }
}

// -----------------------------------------------------------------------

Plugin* createPlugin()
{
    return new RobotMoogFilterPlugin();
}

// -----------------------------------------------------------------------

END_NAMESPACE_DISTRHO
