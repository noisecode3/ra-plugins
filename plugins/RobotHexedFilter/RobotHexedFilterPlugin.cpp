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

const float PI_F = 3.1415927410125732421875f;
const float E_F  = 2.7182818284590452353602f;
const float dc   = 1e-18;

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
        parameter.hints      = kParameterIsAutomable | kParameterIsLogarithmic;
        parameter.name       = "CutOff";
        parameter.symbol     = "freq";
        parameter.unit       = "%";
        parameter.ranges.def = 100.0f;
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 100.0f;
        break;

    case paramRes:
        parameter.hints      = kParameterIsAutomable | kParameterIsLogarithmic;
        parameter.name       = "Resonance";
        parameter.symbol     = "res";
        parameter.unit       = "%";
        parameter.ranges.def = 0.0f;
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 100.0f;
        break;

    case paramMode:
        parameter.hints      = kParameterIsAutomable | kParameterIsInteger;
        parameter.name       = "Mode";
        parameter.symbol     = "switch";
        parameter.unit       = "p";
        parameter.ranges.def = 4;
        parameter.ranges.min = 1;
        parameter.ranges.max = 4;
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
        fCutOff       = value;
        fChangeCutOff = fCutOff-fCutOffOld;
        fCutOffFall   = true;
        break;

    case paramRes:
        fRes          = value;
        fChangeRes    = fRes-fResOld;
        fResFall      = true;
        break;

    case paramMode:
        fMode         = value;
        iModeOld      = iMode;
        iMode         = static_cast<int>(fMode);
        if (iMode != iModeOld) fModeFall = true;
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
        fCutOff = 100.0f;
        fRes    = 0.0f;
        fMode   = 4;
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
    fWetOld      = fWet;

    fCutOffFall  = false;
    fResFall     = false;
    fWetFall     = false;

    fWetVol      = 1.0 - exp(-0.01*fWet);
    fWetVol      = fWetVol + 0.367879*(0.01*fWet);

    s1[0]=s2[0]=s3[0]=s4[0]=c[0]=d[0]=0;
    s1[0]=s2[0]=s3[0]=s4[0]=c[0]=d[0]=0;

    R24=0;

    mmt_y1=mmt_y2=mmt_y3=mmt_y4=0; 

    float rcrate = sqrt((44000/fSampleRate));
    rcor24 = (970.0/44000)*rcrate;
    rcor24Inv = 1/rcor24;

    bright =  (sin((fSampleRate*0.5-5) * PI_F * fSampleRateInv))/
              (cos((fSampleRate*0.5-5) * PI_F * fSampleRateInv));

    dc_r = 1.0-(126.0/fSampleRate);
    dc_tmp[0] = 0;
    dc_tmp[1] = 0; 
}

void RobotHexedFilterPlugin::deactivate()
{

}

float RobotHexedFilterPlugin::parameterSurge(float x, float n) //TODO This too simple my not be needed
{
    return x*n;
}

float RobotHexedFilterPlugin::mm_switchSurge(float x)
{
     //TODO Need to test 
     return 0.8999-0.0328*pow(x,E_F);
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
    float S  = (lpc*(lpc*(lpc*s1[chan]+s2[chan])+s3[chan])+s4[chan])*ml;
    float G  = lpc*lpc*lpc*lpc;
    float y  = (sample - R24*S) / (1 + R24*G);
    return y + 1e-8;
}

float RobotHexedFilterPlugin::hexed_filter_process(float x, bool chan)
{
    // Remember about sampels fall, the last sample in the framebuffer is the same as the starting one on the next
    // only if there was NO NEW change. Calling run() with only 1 frame should just read fCutOff. This is linear here
    // but later ln is used

    float rCutoff;
    float rReso;

    //uiCutoff is used with value from 1.0 to 0.0 after this
    if (fSamplesFallCutOff > 1)
    {
        float steps     = 1.0f/fFrames;
        float cutoffAdd = parameterSurge((fFrames-fSamplesFallCutOff+1)*steps, fChangeCutOff);
        uiCutoff        = fCutOffOld+cutoffAdd;
        uiCutoff        = uiCutoff*0.01;
        fSamplesFallCutOff--;
        //printf("fSamplesFallCutOff = %d\n", fSamplesFallCutOff);
        //printf("uiCutoff = %f\n", uiCutoff);

    }
    else { uiCutoff = fCutOffOld = fCutOff; uiCutoff = uiCutoff * 0.01; fCutOffFall = false; } //TODO dont do else here

    //uiReso is used with value from 1.0 to 0.0 after this
    if (fSamplesFallRes > 1)
    {
        float steps  = 1.0f/fFrames;
        float resAdd = parameterSurge((fFrames-fSamplesFallRes+1)*steps, fChangeRes);
        uiReso       = fResOld+resAdd;
        uiReso       = uiReso*0.01;
        fSamplesFallRes--;
    }
    else { uiReso = fResOld = fRes; uiReso = uiReso * 0.01; fResFall = false; }

    if (fSamplesFallMode > 1)
    {
        
        float steps   = 1.0/fFrames;
        mm_balancer   = mm_switchSurge(iModeOld+(iMode-iModeOld)*steps*(fFrames-fSamplesFallMode+1));
        mmch = 5;

        //if(fFrames != fFramesOld) printf("Used diffrent frames value = %d", fFrames);
        //fFramesOld = fFrames;
        if (fFrames == fSamplesFallMode) mmt_y1 = mmt_y2 = mmt_y3 = mmt_y4 = 0;

        switch (iModeOld) // Lowering
        {
            case 4:
                mmt_y4   = 1.0 - exp(-(steps*(fSamplesFallMode-1)));
                mmt_y4   = mmt_y4 + 0.367879*(steps*(fSamplesFallMode-1));
                break;
            case 3:
                mmt_y3   = 1.0 - exp(-(steps*(fSamplesFallMode-1)));
                mmt_y3   = mmt_y3 + 0.367879*(steps*(fSamplesFallMode-1));
                break;
            case 2:
                mmt_y2   = 1.0 - exp(-(steps*(fSamplesFallMode-1)));
                mmt_y2   = mmt_y2 + 0.367879*(steps*(fSamplesFallMode-1));
                break;
            case 1:
                mmt_y1   = 1.0 - exp(-(steps*(fSamplesFallMode-1)));
                mmt_y1   = mmt_y1 + 0.367879*(steps*(fSamplesFallMode-1));
                break;
        }

        switch (iMode) // Rise
        {
            case 4:
                mmt_y4   = exp(-(steps*(fSamplesFallMode)));
                mmt_y4   = mmt_y4 - 0.367879*(steps*(fSamplesFallMode));
                break;
            case 3:
                mmt_y3   = exp(-(steps*(fSamplesFallMode)));
                mmt_y3   = mmt_y3 - 0.367879*(steps*(fSamplesFallMode));
                break;
            case 2:
                mmt_y2   = exp(-(steps*(fSamplesFallMode)));
                mmt_y2   = mmt_y2 - 0.367879*(steps*(fSamplesFallMode));
                break;
            case 1:
                mmt_y1   = exp(-(steps*(fSamplesFallMode)));
                mmt_y1   = mmt_y1 - 0.367879*(steps*(fSamplesFallMode));
                break;
        }
        //printf("mmt_y1 = %f mmt_y2 = %f mmt_y3 = %f mmt_y4 = %f\n", mmt_y1, mmt_y2, mmt_y3, mmt_y4);
        //if (fSamplesFallMode == 2)
        //{
            //printf("last sample, mm_balancer:%f\n", mm_balancer);
        //}
        fSamplesFallMode--;
    }
    else { mmch = iMode; fModeFall = false; }

    // basic DC filter, this removes a super tiny bit of low

    float dc_prev = x;
                x = x - dc_tmp[chan] + dc_r * dc_tmp[chan];
     dc_tmp[chan] = dc_prev;

    // TODO dont calculate rReso and rCutoff for every sample 

    rReso = (0.991-logsc(1-uiReso, 0, 0.991));
    float cutoffNorm = logsc(uiCutoff,60,19000);

    rCutoff = (float)tan(cutoffNorm * fSampleRateInv * PI_F);
    R24   =  3.8 * rReso;

    float g   = rCutoff;
    float lpc = g / (1 + g);

    float br = bright - ((bright-1)*(1.0-((cutoffNorm-60)*0.000000016)));

    float s = x;
    s       = s - 0.45*tptlpupw(c[chan], s, 15, fSampleRateInv);
    s       = tptpc(d[chan], s, br);

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
    float mc = 0.0;

    switch(mmch)
    {
        case 4:
            mc = y4;
            break;
        case 3:
            mc = y3;
            break;
        case 2:
            mc = y2;
            break;
        case 1:
            mc = y1;
            break;
        case 5:
            mc = mmt_y4*y4 + mmt_y3*y3 + mmt_y2*y2 + mmt_y1*y1;
            break;
    }
    return (mc * ( 1 + R24 * 0.45 )) * (1-(mm_balancer*rReso*0.222));
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
    if (fWetFall)    fSamplesFallWet    = fFrames;
    if (fModeFall)   fSamplesFallMode   = fFrames;

    for (uint32_t i=0; i < frames; ++i)
    {
        float fout1, fout2;

        if (fSamplesFallWet > 1)
        {
            float steps  = 1.0/fFrames;
            float wetAdd = parameterSurge((fFrames-fSamplesFallWet+1)*steps, fChangeWet);
            fWetVol      = 1.0 - exp(-0.01*(fWetOld+wetAdd));
            fWetVol      = fWetVol + 0.367879*(0.01*(fWetOld+wetAdd));
            //printf("fWetVol = %f\n", fWetVol);
            fSamplesFallWet--;
        }
        else
        {
            fWetOld      = fWet;
            fWetVol      = 1.0 - exp(-0.01*fWet);
            fWetVol      = fWetVol + 0.367879*(0.01*fWet);
            fWetFall     = false;
        }

        fout1   = (in1[i]*(1.0-fWetVol)) + (hexed_filter_process(in1[i], 0)*fWetVol);
        fout2   = (in2[i]*(1.0-fWetVol)) + (hexed_filter_process(in2[i], 1)*fWetVol);

        //if (fout1 >  1.0f) fout1 =  1.0f;
        //if (fout1 < -1.0f) fout1 = -1.0f;
        //if (fout2 >  1.0f) fout2 =  1.0f;
        //if (fout2 < -1.0f) fout2 = -1.0f;
        /*
        switch (std::fpclassify(fout1))
        {
        case FP_INFINITE:  printf ("infinite");  break;
        case FP_NAN:       printf ("NaN");       break;
        case FP_ZERO:      printf ("zero");      break;
        case FP_SUBNORMAL: printf ("subnormal"); break;
        case FP_NORMAL:    break;
        }

        switch (std::fpclassify(fout2))
        {
        case FP_INFINITE:  printf ("infinite");  break;
        case FP_NAN:       printf ("NaN");       break;
        case FP_ZERO:      printf ("zero");      break;
        case FP_SUBNORMAL: printf ("subnormal"); break;
        case FP_NORMAL:    break;
        }
        */

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
