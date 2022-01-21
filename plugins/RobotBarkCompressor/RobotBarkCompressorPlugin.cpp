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
    kiss_fftr_free(cfg1);
    kiss_fftr_free(cfg2);
    kiss_fft_cleanup();
    printf("Clean and free\n");

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
    for(int i = 0; i < 1; i++)
    {

        printf("JAckson was here!\n");
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

// kiss_fftr_cfg KISS_FFT_API kiss_fftr_alloc(int nfft,int inverse_fft,void * mem, size_t * lenmem);
/*
 nfft must be even

 If you don't care to allocate space, use mem = lenmem = NULL
*/


//void KISS_FFT_API kiss_fftr(kiss_fftr_cfg cfg,const kiss_fft_scalar *timedata,kiss_fft_cpx *freqdata);
/*
 input timedata has nfft scalar points
 output freqdata has nfft/2+1 complex points // MY note: 2/2+1=2 in this case
*/

//void KISS_FFT_API kiss_fftri(kiss_fftr_cfg cfg,const kiss_fft_cpx *freqdata,kiss_fft_scalar *timedata);
/*
 input freqdata has  nfft/2+1 complex points
 output timedata has nfft scalar points
*/


    for (uint32_t i = 0; i < frames; ++i) {

        //Left
        timedata1[0] = in1[i];

        if ((i+2) > frames) timedata1[1] = in1[frames-1];
            else timedata1[1] = in1[i+1]; // shouldent need to look forvard one sample on the frame buffer this is bad

        kiss_fftr(cfg1, timedata1, cx_out1);

        hz1 = (cx_out1[1].r);



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
        timedata2[0] = in2[i];

        if ((i+2) > frames) timedata2[1] = in2[frames-1];
            else timedata2[1] = in2[i+1]; // shouldent need to look forvard one sample on the frame buffer this is bad

        kiss_fftr(cfg2, timedata2, cx_out2);

        hz2 = (cx_out2[1].r);

        printf("hz1:%f\t\t\t hz2:%f\n", hz1, hz2);


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
