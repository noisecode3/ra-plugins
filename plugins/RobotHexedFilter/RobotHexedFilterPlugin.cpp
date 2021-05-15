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

#include "RobotHexedFilterPlugin.hpp"

//#define PI_F 3.1415927410125732421875f
//#define E_F  2.7182818284590452353602f

const float PI_F =3.1415927410125732421875f;
const float E_F  = 2.7182818284590452353602f;
const float dc = 1e-18;

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------

RobotHexedFilterPlugin::RobotHexedFilterPlugin()
    : Plugin(paramCount, 1, 0) // parameters, program, states
{
    // set default values
    loadProgram(0);
}

// -----------------------------------------------------------------------
// Init

void RobotHexedFilterPlugin::initParameter(uint32_t index, Parameter& parameter)
{
    switch (index)
    {
    case paramCutOff:
        parameter.hints      = kParameterIsAutomable;
        parameter.name       = "CutOff";
        parameter.symbol     = "freq";
        parameter.unit       = "%";
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

    case paramDuck:
        parameter.hints      = kParameterIsAutomable;
        parameter.name       = "FilterDuck";
        parameter.symbol     = "volt";
        parameter.unit       = "V";
        parameter.ranges.def = 1.0f;
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

    case paramRes:
        return fRes;

    case paramDuck:
        return fDuck;

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
        fCutOff       = value;
        fChangeCutOff = fCutOff-fCutOffOld;
        fCutOffFall   = true;
        break;

    case paramRes:
        fRes          = value;
        fChangeRes    = fRes-fResOld;
        fResFall      = true;
        break;

    case paramDuck:
        fDuck         = value;
        fChangeDuck   = fDuck-fDuckOld;
        fDuckFall     = true;
        break;

    case paramWet:
        fWet          = value;
        fChangeWet    = fWet-fWetOld;
        fWetFall      = true;
        break;
    }
}

void RobotHexedFilterPlugin::loadProgram(uint32_t index)
{
    switch (index)
    {
    case 0:
        // Default
        fCutOff = 1.0f;
        fRes    = 0.0f;
        fDuck   = 1.0f;
        fWet    = 0.0f;
        activate();
        break;
    }
}

// -----------------------------------------------------------------------
// Process

void RobotHexedFilterPlugin::activate()
{

    fSampleRate = (float)getSampleRate();
    fSampleRateInv = 1/fSampleRate;

    fCutOffOld   = fCutOff;
    fResOld      = fRes;
    fDuckOld     = fDuck;
    fWetOld      = fWet;

    fCutOffFall  = false; // cus new framebuffer
    fResFall     = false;
    fDuckFall    = false;
    fWetFall     = false;

    fWetVol      = 1.0f - exp(-0.01f*fWet); //TODO get rid of fWetVol from activate
    fWetVol      = fWetVol + 0.367879*(0.01f*fWet);

    s1[0]=s2[0]=s3[0]=s4[0]=c[0]=d[0]=0;
    s1[0]=s2[0]=s3[0]=s4[0]=c[0]=d[0]=0;

    R24=0;

    int mm = 0;

    mmch = (int)(mm * 3); //TODO this are nuts without a nutcracker
    mmt  = mm * 3 - mmch;

    float rcrate = sqrt((44000/fSampleRate));
    rcor24 = (970.0 / 44000)*rcrate;
    rcor24Inv = 1 / rcor24;

    bright = tan((fSampleRate*0.5f-10) * PI_F * fSampleRateInv); //TODO maybe use diffrent tan

    dc_r = 1.0-(126.0/fSampleRate);
    dc_tmp[0] = 0;
    dc_tmp[1] = 0; 
}

void RobotHexedFilterPlugin::deactivate()
{

}

float RobotHexedFilterPlugin::parameterSurge(float x, float n)
{
    return x*n;
}

float RobotHexedFilterPlugin::logsc(float param, const float min, const float max, const float rolloff = 19.0f)
{
    return ((expf(param * logf(rolloff+1)) - 1.0f) / (rolloff)) * (max-min) + min;
}

float RobotHexedFilterPlugin::tptpc(float& state, float inp, float cutoff)
{
    double v   = (inp - state) * cutoff / (1 + cutoff);
    double res = v + state;
    state      = res + v;
    return res;
}

float RobotHexedFilterPlugin::tptlpupw(float& state, float inp, float cutoff, float srInv)
{
    cutoff     = (cutoff * srInv)* PI_F;
    double v   = (inp - state) * cutoff / (1 + cutoff);
    double res = v + state;
    state      = res + v;
    return res;
}

float RobotHexedFilterPlugin::NR24(float sample, float g, float lpc, bool chan)
{
    float ml = 1 / (1+g);

    float S = ((lpc+(0.0533789*sin(sample*E_F)))*
              ((lpc-(0.0330123*sin(sample*E_F)))*
              ((lpc-(0.0032384*sin(sample*E_F)))*
                            s1[chan] + s2[chan])+
                                       s3[chan])+
                                       s4[chan])* ml;
    // This is art

    float G  = lpc*lpc*lpc*lpc;
    float y  = (sample - R24 * S) / (1 + R24*G);
    return y + 1e-8;
}

float RobotHexedFilterPlugin::hexed_filter_process(float x, bool chan)
{

    float rCutoff;
    float rReso;
    float rGain;
    float uiCutoff;
    float uiReso;

    if (fSamplesFallCutOff > 1)
    {
        float steps     = 1.0f/fFrames;
        float cutoffAdd = parameterSurge((fFrames-fSamplesFallCutOff+1)*steps, fChangeCutOff);
        uiCutoff = fCutOffOld+cutoffAdd; 
        fSamplesFallCutOff--;
    }
    else { uiCutoff = fCutOffOld = fCutOff; fCutOffFall = false; } //TODO dont do else here

    if (fSamplesFallRes > 1)
    {
        float steps  = 1.0f/fFrames;
        float resAdd = parameterSurge((fFrames-fSamplesFallRes+1)*steps, fChangeRes);
        uiReso       = fResOld+resAdd;
        fSamplesFallRes--;
    }
    else { uiReso = fResOld = fRes; fResFall = false; }

    if (fSamplesFallDuck > 1)
    {
        float steps   = 1.0f/fFrames;
        float duckAdd = parameterSurge((fFrames-fSamplesFallDuck+1)*steps, fChangeDuck);
        rGain         = fDuckOld+duckAdd;
        fSamplesFallDuck--;
    }
    else { rGain = fDuckOld = fDuck; fDuckFall = false; }


    // basic DC filter, this removes a super tiny bit of low

    float dc_prev = x;
                x = x - dc_tmp[chan] + dc_r * dc_tmp[chan];
     dc_tmp[chan] = dc_prev;

    // TODO dont calculate rReso and rCutoff for every sample 
    
    rReso = (0.991-logsc(1-uiReso, 0, 0.991));
    R24   =  3.5 * rReso;

    float cutoffNorm = logsc(uiCutoff,60,19000);
    rCutoff = (float)tan(cutoffNorm * fSampleRateInv * PI_F);

    float g   = rCutoff;
    float lpc = g / (1 + g);

    float s = x;
    s       = s - 0.45*tptlpupw(c[chan], s, 15, fSampleRateInv);
    s       = tptpc(d[chan], s, bright);

    float y0 = NR24(s, g, lpc, chan);

    //first low pass in cascade
    double v   = (y0 - s1[chan]) * lpc;
    double res =   v + s1[chan];
    s1[chan]   = res + v;

    //damping
    s1[chan] = atan(s1[chan]*rcor24)*rcor24Inv;
    float y1 = res;
    float y2 = tptpc(s2[chan],y1,g);
    float y3 = tptpc(s3[chan],y2,g);
    float y4 = tptpc(s4[chan],y3,g);
    float mc = 0.0f;

    //TODO handel mmch and mmt

    switch(mmch)
    {
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
    return (mc * (1 + R24 * 0.45))*rGain;
}

void RobotHexedFilterPlugin::run(const float** inputs, float** outputs, uint32_t frames)
{
    const float* in1  = inputs[0];
    const float* in2  = inputs[1];
    float*       out1 = outputs[0];
    float*       out2 = outputs[1];

    fFrames = frames;
    if (fCutOffFall) fSamplesFallCutOff = fFrames;
    if (fResFall)    fSamplesFallRes    = fFrames;
    if (fDuckFall)   fSamplesFallDuck   = fFrames;
    if (fWetFall)    fSamplesFallWet    = fFrames;

    for (uint32_t i=0; i < frames; ++i)
    {
        float fout1, fout2;

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

        fout1   = (in1[i]*(1.0f-fWetVol)) + (hexed_filter_process(in1[i], 0)*fWetVol);
        fout2   = (in2[i]*(1.0f-fWetVol)) + (hexed_filter_process(in2[i], 1)*fWetVol);

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
    return new RobotHexedFilterPlugin();
}

// -----------------------------------------------------------------------

END_NAMESPACE_DISTRHO
