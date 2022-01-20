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

RobotBarkCompressorPlugin::~RobotBarkCompressorPlugin()
{
    kiss_fft_free(cfg1);
    kiss_fft_free(cfg2);
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
        parameter.ranges.def = 1.0f;
        parameter.ranges.min = 0.01f;
        parameter.ranges.max = 10.0f;
        break;

    case paramRelease:
        parameter.hints      = kParameterIsAutomable;
        parameter.name       = "Release";
        parameter.symbol     = "re";
        parameter.unit       = "ms";
        parameter.ranges.def = 36.0f;
        parameter.ranges.min = 0.01f;
        parameter.ranges.max = 120.0f;
        break;

    case paramThreshold:
        parameter.hints      = kParameterIsAutomable;
        parameter.name       = "Threshold";
        parameter.symbol     = "th";
        parameter.unit       = "dB";
        parameter.ranges.def = 0.0f;
        parameter.ranges.min = -80.0f;
        parameter.ranges.max = 0.0f;
        break;

    case paramRatio:
        parameter.hints      = kParameterIsAutomable;
        parameter.name       = "Ratio";
        parameter.symbol     = "ra";
        parameter.unit       = ":1";
        parameter.ranges.def = 1.0f;
        parameter.ranges.min = 1.0f;
        parameter.ranges.max = 16.0f;
        break;

    case paramMakeUpGain:
        parameter.hints      = kParameterIsAutomable;
        parameter.name       = "MakeUpGain";
        parameter.symbol     = "mug";
        parameter.unit       = "dB";
        parameter.ranges.def = 0.0f;
        parameter.ranges.min = -8.0f;
        parameter.ranges.max = 24.0f;
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
		fAttack     = 1.0;
		fRelease    = 36.0;
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

    //Put new fruit in them bins
    for(int i = 0; i < 2; i++)
    {
        cx_in1[i].r  = 0;
        cx_in1[i].r  = 0;
        cx_out1[i].r = 0;
        cx_out2[i].r = 0;
    }
}
void RobotBarkCompressorPlugin::deactivate()
{
}

void RobotBarkCompressorPlugin::run(const float** inputs, float** outputs, uint32_t frames, const MidiEvent* midiEvents, uint32_t midiEventCount)
{
    const float* in1 = inputs[0];
    const float* in2 = inputs[1];

    float* out1 = outputs[0];
    float* out2 = outputs[1];

    double sr    = getSampleRate();
    int    sri   = (int)sr;
    float  slope = 1 - 1 / fRatio;

    // sample rates
    //
    // 22050  = 54.56695791
    // 32000  = 57.52123620
    // 44100  = 59.06280878
    // 48000  = 59.36179537
    // 96000  = 60.82050573
    // 192000 = 61.39865212
    //
    // TO DO: calculate for possible sample rates, no switch

    float sampleRateScaler;

    switch (sri)
    {
        case 22050:
            sampleRateScaler = 54.56695791;
            break;

        case 32000:
            sampleRateScaler = 57.52123620;
            break;

        case 44100:
            sampleRateScaler = 59.06280878;
            break;

        case 48000:
            sampleRateScaler = 59.36179537;
            break;

        case 96000:
            sampleRateScaler = 60.82050573;
            break;

        case 192000:
            sampleRateScaler = 61.39865212;
            break;

        default:
            sampleRateScaler = 0.0;
            break;
    }

//https://en.wikipedia.org/wiki/Goertzel_algorithm
// lol this is so wrong, I think there should be a separet loop for fft
// TODO: Detect frequinese for samples points in the signal

    for (uint32_t i = 0; i < frames; ++i) {

        //Left
        cx_in1[0].r = posts1;
        cx_in1[1].r = in1[i];

        if ((i+1)==frames)
            cx_in1[2].r = (posts1+in1[i])*0.5;

        else cx_in1[2].r = in1[i+1];

        posts1= in1[i];

        kiss_fft(cfg1, cx_in1, cx_out1);


        float hz1 = cx_out1[1].r*(getSampleRate()/(4*PI_F)); // this is close to what a spectrum analyser would show but this is for individual samples
        hz1 = fabs(hz1); //TODO: this is not perfect at end and the beginning of the buffer

        printf("hz1:%f\n", hz1);


        float sideInput1 = (13.0*atan(hz1/1315.8)
                + 3.5*atan(pow((hz1/7500.0),2)))/(sampleRateScaler*24); // Convert to the bark scale

        float c1 = sideInput1 >= state1 ? cAT : cRT;           // When  sideInput is bigger then state it compresses and have an attack
        float env1 = sideInput1 + c1 * (state1 - sideInput1);  // cus sideInput was bigger and in absolute value sideInput makes it negative.
        float env_db1 = 10*log10(env1);                        // the delta value (previous env) makes it smaller and smaller or bigger and bigger (if negative, it decompresses logically)
        state1 = env1;                                         // until the number of values defined by attack or release multiplied by change of "last state sample" and sideInput ends

        float gain1 = slope * (fThreshold - env_db1);          // if it is under the threshold nothing happens, but if above gain gets applied in absolute value
        gain1 = fmin(0.f, gain1);
        gain1 = pow(10, (gain1/20));

        out1[i] = in1[i] * gain1 * (pow(10, (fMakeUpGain/20))); //TODO I think I need to comment this math better


        //Right
        cx_in2[0].r = posts2;
        cx_in2[1].r = in2[i];

        if ((i+1)==frames)
            cx_in2[2].r = (posts2+in2[i])*0.5;

        else cx_in2[2].r = in2[i+1];

        posts2 = in1[i];

        kiss_fft(cfg2, cx_in2, cx_out2);


        float hz2 = cx_out2[1].r*(getSampleRate()/(4*PI_F));
        hz2 = fabs(hz2);

        printf("hz2:%f\n", hz2);


        float sideInput2 = (13.0*atan(hz2/1315.8)
                + 3.5*atan(pow((hz2/7500.0),2)))/(sampleRateScaler*24);

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

// --------------------------------------------------   ---------------------

END_NAMESPACE_DISTRHO
