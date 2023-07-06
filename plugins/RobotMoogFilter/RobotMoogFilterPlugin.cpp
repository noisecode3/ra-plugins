/*
 *  Robot Audio Plugins
 *  Copyright (C) 2021  Martin Bångens
 *
 *  Programing style originally from https://github.com/DISTRHO/DPF-Plugins
 *  Dsp algorithms originally from https://github.com/electro-smith/DaisySP
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

#include "RobotMoogFilterPlugin.hpp"

#define PI_F 3.1415927410125732421875f
#define E_F  2.7182818284590452353602f
#define THERMAL 0.000026f

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------

RobotMoogFilterPlugin::RobotMoogFilterPlugin()
    : Plugin(paramCount, 1, 0) // parameters, program, states
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
        parameter.hints      = kParameterIsAutomatable|kParameterIsLogarithmic;
        parameter.name       = "CutOff";
        parameter.symbol     = "freq";
        parameter.unit       = "%";
        parameter.ranges.def = 100.0f;
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 100.0f;
        break;

    case paramRes:
        parameter.hints      = kParameterIsAutomatable;
        parameter.name       = "Resonance";
        parameter.symbol     = "res";
        parameter.unit       = "%";
        parameter.ranges.def = 0.0f;
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 100.0f;
        break;

    case paramWet:
        parameter.hints      = kParameterIsAutomatable|kParameterIsLogarithmic;
        parameter.name       = "Wet";
        parameter.symbol     = "percent";
        parameter.unit       = "%";
        parameter.ranges.def = 0.0f;
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 100.0f;
        break;

    }
}

void RobotMoogFilterPlugin::initProgramName(uint32_t index, String& programName)
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

float RobotMoogFilterPlugin::getParameterValue(uint32_t index) const
{
    switch (index)
    {
    case paramFreq:
        return fFreq;

    case paramRes:
        return fRes;

    case paramWet:
        return fWet;

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
        fFreq        = value;
        fChangeFreq  = fFreq-fFreqOld;
        fFreqFall    = true;
        break;

    case paramRes:
        fRes         = value;
        fChangeRes   = fRes-fResOld;
        fResFall     = true;
        break;

    case paramWet:
        fWet         = value;
        fChangeWet   = fWet-fWetOld;
        fWetFall     = true;
        break;
    }
}

void RobotMoogFilterPlugin::loadProgram(uint32_t index)
{
    switch (index)
    {
    case 0:
        // Default
        fFreq = 100.0f;
        fRes  = 0.0f;
        fWet  = 0.0f;
        activate();
        break;
    }
}

// -----------------------------------------------------------------------
// Process

float RobotMoogFilterPlugin::logsc(float param, const float min, const float max, const float rolloff = 19.0f)
{
    return ((expf(param * logf(rolloff+1)) - 1.0f) / (rolloff)) * (max-min) + min;
}

void RobotMoogFilterPlugin::activate()
{
    fSampleRate = (float)getSampleRate();

    for(int i = 0; i < 6; i++)
    {
        fDelay[0][i]     = 0.0;
        fDelay[1][i]     = 0.0;
    }

    for(int i = 0; i < 3; i++)
    {
        fTanhstg[0][i]   = 0.0;
        fTanhstg[1][i]   = 0.0;
    }

    moog_ladder_tune(logsc(0.01*fFreq, 20.0, 22000.0));
    fWetVol      = 1.0f - exp(-0.01f*fWet);
    fWetVol      = fWetVol + 0.367879*(0.01f*fWet);

    fFreqOld     = fFreq;
    fResOld      = fRes;
    fWetOld      = fWet;

    fFreqFall    = false;
    fResFall     = false;
    fWetFall     = false;
}

void RobotMoogFilterPlugin::deactivate()
{
    //TODO
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

void RobotMoogFilterPlugin::moog_ladder_tune(float freq)
{
    float f, fc, fc2, fc3, fcr;

    fc        = (freq / fSampleRate);
    f         = 0.5f * fc;
    fc2       = fc * fc;
    fc3       = fc2 * fc2;

    fcr   = 1.8730f * fc3 + 0.4955f * fc2 - 0.6490f * fc + 0.9988f;
    fAcr  = -3.9364f * fc2 + 1.8409f * fc + 0.9968f;
    fTune = (1.0f - expf(-((2 * PI_F) * f * fcr))) / THERMAL;
}

float RobotMoogFilterPlugin::moog_ladder_process(float in, bool chan)
{

// -----------------------------------------------------------------------
// Parameters

    float  res4;

    if (fSamplesFallFreq > 1)
    {
        float steps   = 1.0f/fFrames;
        float freqAdd = ((fFrames-fSamplesFallFreq+1)*steps)*(fChangeFreq);
        moog_ladder_tune(logsc(0.01*(fFreqOld+freqAdd), 20.0, 22000.0));
        fSamplesFallFreq--;
    }
    else { moog_ladder_tune(logsc(0.01*fFreq, 20.0, 22000.0)); fFreqOld = fFreq; fFreqFall = false; }

    if (fSamplesFallRes > 1)
    {
        float steps  = 1.0f/fFrames;
        float resAdd = ((fFrames-fSamplesFallRes+1)*steps)*(fChangeRes);
        res4         = 4.0f * logsc(0.01*(fResOld+resAdd), 0.0, 0.95) * fAcr;
        fSamplesFallRes--;
    }
    else { res4 = 4.0f * logsc(0.01*fRes, 0.0, 0.95) * fAcr; fResOld = fRes; fResFall = false; }

    if (fSamplesFallWet > 1)
    {
        float steps  = 1.0f/fFrames;
        float wetAdd = ((fFrames-fSamplesFallWet+1)*steps)*(fChangeWet);
        fWetVol      = 1.0f - exp(-0.01f*(fWetOld+wetAdd));
        fWetVol      = fWetVol + 0.367879*(0.01f*(fWetOld+wetAdd));
        fSamplesFallWet--;
    }
    else
    {
        fWetOld      = fWet;
        fWetVol      = 1.0f - exp(-0.01f*fWet);
        fWetVol      = fWetVol + 0.367879*(0.01f*fWet);
        fWetFall     = false;
    }

// -----------------------------------------------------------------------
// Filter

    float  stg[4];

    for(int j = 0; j < 2; j++)
    {
        in -= res4 * fDelay[chan][5];
        fDelay[chan][0] = stg[0]
            = fDelay[chan][0] + fTune * (moog_tanh(in * THERMAL) - fTanhstg[chan][0]);
        for(int k = 1; k < 4; k++)
        {
            in     = stg[k - 1];
 
            stg[k] = fDelay[chan][k] + fTune *
                ((fTanhstg[chan][k - 1] = moog_tanh(in * THERMAL)) -
                 (k != 3 ? fTanhstg[chan][k] : moog_tanh(fDelay[chan][k] * THERMAL)));

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

    fFrames = frames;
    if (fFreqFall) fSamplesFallFreq = frames;
    if (fResFall)  fSamplesFallRes  = frames;
    if (fWetFall)  fSamplesFallWet  = frames;

    for (uint32_t i=0; i < frames; ++i)
    {
        float fout1, fout2;

        fout1   = ((in1[i]*(1.0f-fWetVol)) + (moog_ladder_process(in1[i], 0)*fWetVol));
        fout2   = ((in2[i]*(1.0f-fWetVol)) + (moog_ladder_process(in2[i], 1)*fWetVol));

        out1[i] = fout1;
        out2[i] = fout2;
    }
}

// -----------------------------------------------------------------------

Plugin* createPlugin()
{
    return new RobotMoogFilterPlugin();
}

// -----------------------------------------------------------------------

END_NAMESPACE_DISTRHO
