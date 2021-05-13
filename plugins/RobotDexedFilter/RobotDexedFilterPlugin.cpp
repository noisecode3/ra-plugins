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

#include "RobotDexedFilterPlugin.hpp"

#define PI_F 3.1415927410125732421875f
#define E_F  2.7182818284590452353602f
const float dc = 1e-18;

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------

RobotDexedFilterPlugin::RobotDexedFilterPlugin()
    : Plugin(paramCount, 1, 0) // parameters, program, states
{
    // set default values
    loadProgram(0);
}

// -----------------------------------------------------------------------
// Init

void RobotDexedFilterPlugin::initParameter(uint32_t index, Parameter& parameter)
{
    switch (index)
    {
    case paramCutOff:
        parameter.hints      = kParameterIsAutomable;
        parameter.name       = "CutOff";
        parameter.symbol     = "freq";
        parameter.unit       = "Hz";
        parameter.ranges.def = 1.0f;
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        break;

    case paramRes:
        parameter.hints      = kParameterIsAutomable;
        parameter.name       = "Resonance";
        parameter.symbol     = "res";
        parameter.unit       = "L";
        parameter.ranges.def = 0.0f;
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        break;

    case paramGain:
        parameter.hints      = kParameterIsAutomable;
        parameter.name       = "Gain";
        parameter.symbol     = "volt";
        parameter.unit       = "v";
        parameter.ranges.def = 0.0f;
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        break;

    case paramWet:
        parameter.hints      = kParameterIsAutomable;
        parameter.name       = "Wet";
        parameter.symbol     = "percent";
        parameter.unit       = "%";
        parameter.ranges.def = 0.0f;
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 100.0f;
        break;

    }
}

void RobotDexedFilterPlugin::initProgramName(uint32_t index, String& programName)
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

float RobotDexedFilterPlugin::getParameterValue(uint32_t index) const
{
    switch (index)
    {
    case paramCutOff:
        return fCutOff;

    case paramRes:
        return fRes;

    case paramGain:
        return fGain;

    case paramWet:
        return fWet;

    default:
        return 0.0f;
    }
}

void RobotDexedFilterPlugin::setParameterValue(uint32_t index, float value)
{
    if (getSampleRate() <= 0.0)
        return;

    switch (index)
    {
    case paramCutOff:
        fCutOff       = value;
        fChangeCutOff = fCutOff-fCutOffOld;
        fCutOffFall   = true;
        break;

    case paramRes:
        fRes          = value;
        fChangeRes    = fRes-fResOld;
        fResFall      = true;
        break;

    case paramGain:
        fGain         = value;
        fChangeGain   = fRes-fResOld;
        fGainFall     = true;
        break;

    case paramWet:
        fWet          = value;
        fChangeWet    = fWet-fWetOld;
        fWetFall      = true;
        break;
    }
}

void RobotDexedFilterPlugin::loadProgram(uint32_t index)
{
    switch (index)
    {
    case 0:
        // Default
        fCutOff = 1.0f;
        fRes    = 0.0f;
        fGain   = 0.0f;
        fWet    = 0.0f;
        activate();
        break;
    }
}

// -----------------------------------------------------------------------
// Process

void RobotDexedFilterPlugin::activate()
{

    fSampleRate = (float)getSampleRate();
    fSampleRateInv = 1/fSampleRate;

    fWetVol      = 1.0f - exp(-0.01f*fWet);
    fWetVol      = fWetVol + 0.367879*(0.01f*fWet);
    fCutOffOld   = fCutOff;
    fResOld      = fRes;
    fGainOld     = fGain;
    fWetOld      = fWet;

    /*
    mm=0;
    s1=s2=s3=s4=c=d=0;
    R24=0;

    mmch = (int)(mm * 3);
    mmt = mm*3-mmch;

    float rcrate =sqrt((44000/sampleRate));
    rcor24 = (970.0 / 44000)*rcrate;
    rcor24Inv = 1 / rcor24;

    bright = tan((sampleRate*0.5f-10) * juce::float_Pi * sampleRateInv);

    R = 1;
    rcor = (480.0 / 44000)*rcrate;
    rcorInv = 1 / rcor;
    bandPassSw = false;

    pCutoff = -1;
    pReso = -1;
*/
    dc_r = 1.0-(126.0/fSampleRate);
    dc_tmp[0] = 0;
    dc_tmp[1] = 0; 
}

void RobotDexedFilterPlugin::deactivate()
{

}

float RobotDexedFilterPlugin::parameterSurge(float x, float n)
{
    return x*n;
}

float RobotDexedFilterPlugin::logsc(float param, const float min, const float max, const float rolloff = 19.0f)
{
    return ((expf(param * logf(rolloff+1)) - 1.0f) / (rolloff)) * (max-min) + min;
}

float RobotDexedFilterPlugin::tptpc(float& state, float inp, float cutoff)
{
    double v = (inp - state) * cutoff / (1 + cutoff);
    double res = v + state;
    state = res + v;
    return res;
}

float RobotDexedFilterPlugin::tptlpupw(float& state , float inp, float cutoff, float srInv)
{
    cutoff = (cutoff * srInv)* PI_F;
    double v = (inp - state) * cutoff / (1 + cutoff);
    double res = v + state;
    state = res + v;
    return res;
}

float RobotDexedFilterPlugin::NR24(float sample,float g,float lpc)
{
    float ml = 1 / (1+g);
    float S = (lpc*(lpc*(lpc*s1 + s2) + s3) +s4)*ml;
    float G = lpc*lpc*lpc*lpc;
    float y = (sample - R24 * S) / (1 + R24*G);
    return y + 1e-8;
}

float RobotDexedFilterPlugin::dexed_filter_process(float x, bool chan)
{


    if (fSamplesFallWet > 1)
    {
        float steps  = 1.0f/fFrames;
        float wetAdd = parameterSurge((fFrames-fSamplesFallWet+1)*steps, fChangeWet);
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


    // basic DC filter, this removes a super tiny bit of low

    float dc_prev = x;
    x = x - dc_tmp[chan] + dc_r * dc_tmp[chan];
    dc_tmp[chan] = dc_prev;

    return x;
/*
    if ( uiGain != 1 ) {
        for(int i=0; i < sampleSize; i++ )
            work[i] *= uiGain;
    }

    // don't apply the LPF if the cutoff is to maximum
    if ( uiCutoff == 1 )
        return;

    if ( uiCutoff != pCutoff || uiReso != pReso ) {
        rReso = (0.991-logsc(1-uiReso,0,0.991));
        R24 = 3.5 * rReso;

        float cutoffNorm = logsc(uiCutoff,60,19000);
        rCutoff = (float)tan(cutoffNorm * sampleRateInv * juce::float_Pi);

        pCutoff = uiCutoff;
        pReso = uiReso;

        R = 1 - rReso;
    }

    // THIS IS MY FAVORITE 4POLE OBXd filter

    // maybe smooth this value
    float g = rCutoff;
    float lpc = g / (1 + g);

    for(int i=0; i < sampleSize; i++ ) {
        float s = work[i];
        s = s - 0.45*tptlpupw(c,s,15,sampleRateInv);
        s = tptpc(d,s,bright);

        float y0 = NR24(s,g,lpc);

        //first low pass in cascade
        double v = (y0 - s1) * lpc;
        double res = v + s1;
        s1 = res + v;

        //damping
        s1 =atan(s1*rcor24)*rcor24Inv;
        float y1= res;
        float y2 = tptpc(s2,y1,g);
        float y3 = tptpc(s3,y2,g);
        float y4 = tptpc(s4,y3,g);
        float mc;

        switch(mmch) {
            case 0:
                mc = ((1 - mmt) * y4 + (mmt) * y3);
                break;
            case 1:
                mc = ((1 - mmt) * y3 + (mmt) * y2);
                break;
            case 2:
                mc = ((1 - mmt) * y2 + (mmt) * y1);
                break;
            case 3:
                mc = y1;
                break;
        }

        //half volume comp
        work[i] = mc * (1 + R24 * 0.45);
    }*/
}


void RobotDexedFilterPlugin::run(const float** inputs, float** outputs, uint32_t frames)
{
    const float* in1  = inputs[0];
    const float* in2  = inputs[1];
    float*       out1 = outputs[0];
    float*       out2 = outputs[1];

    fFrames = frames;
    if (fCutOffFall) fSamplesFallCutOff = frames;
    if (fResFall)    fSamplesFallRes    = frames;
    if (fGainFall)   fSamplesFallGain   = frames;
    if (fWetFall)    fSamplesFallWet    = frames;

    for (uint32_t i=0; i < frames; ++i)
    {
        float fout1, fout2;

        fout1   = (in1[i]*(1.0f-fWetVol)) + (dexed_filter_process(in1[i], 0)*fWetVol);
        fout2   = (in2[i]*(1.0f-fWetVol)) + (dexed_filter_process(in2[i], 1)*fWetVol);

        if (fout1 >  1.0f) fout1 =  1.0f;
        if (fout1 < -1.0f) fout1 = -1.0f;
        if (fout2 >  1.0f) fout2 =  1.0f;
        if (fout2 < -1.0f) fout2 = -1.0f;

        out1[i] = fout1;
        out2[i] = fout2;
    }
}

// -----------------------------------------------------------------------

Plugin* createPlugin()
{
    return new RobotDexedFilterPlugin();
}

// -----------------------------------------------------------------------

END_NAMESPACE_DISTRHO
