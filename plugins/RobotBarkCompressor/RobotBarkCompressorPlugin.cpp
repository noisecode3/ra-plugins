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

// -------------------------------------------------------------------------------

RobotBarkCompressorPlugin::RobotBarkCompressorPlugin()
    : Plugin(paramCount, 1, 0) // parameters, program, states
{
    // set default values
    loadProgram(0);

    // make windows

    for (int i=0; i<32; i++)
    {
        hanningWindow[i] = 0.5f*(1.0f-cosf((2.0f*M_PI*i)/(32.0f-1.0f)));
        gaussWindow[i]   = powf(M_E,-0.5f*powf((i-(32-1)*0.5f)/(0.4f*(32-1)*0.5f),2.0f));
    }
}

RobotBarkCompressorPlugin::~RobotBarkCompressorPlugin()
{
    kiss_fftr_free(cfg1);
    kiss_fftr_free(cfg2);
    kiss_fft_cleanup();
}

// -------------------------------------------------------------------------------
// Init

void RobotBarkCompressorPlugin::initParameter(uint32_t index, Parameter& parameter)
{
    switch (index)
    {
    case paramAttack:
        parameter.hints      = kParameterIsAutomable;
        parameter.name       = "Attack";
        parameter.symbol     = "A";
        parameter.unit       = "ms";
        parameter.ranges.def = 1.0f;
        parameter.ranges.min = 0.01f;
        parameter.ranges.max = 10.0f;
        break;

    case paramRelease:
        parameter.hints      = kParameterIsAutomable;
        parameter.name       = "Release";
        parameter.symbol     = "R";
        parameter.unit       = "ms";
        parameter.ranges.def = 36.0f;
        parameter.ranges.min = 0.01f;
        parameter.ranges.max = 120.0f;
        break;

    case paramThreshold:
        parameter.hints      = kParameterIsAutomable;
        parameter.name       = "Threshold";
        parameter.symbol     = "T";
        parameter.unit       = "dB";
        parameter.ranges.def = 0.0f;
        parameter.ranges.min = -80.0f;
        parameter.ranges.max = 0.0f;
        break;

    case paramRatio:
        parameter.hints      = kParameterIsAutomable;
        parameter.name       = "Ratio";
        parameter.symbol     = "r";
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

// -------------------------------------------------------------------------------
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

// -------------------------------------------------------------------------------
// Process

void RobotBarkCompressorPlugin::activate()
{
    cAT = exp(-1 * 1000 / fAttack / getSampleRate());
    cRT = exp(-1 * 1000 / fRelease / getSampleRate());

}
void RobotBarkCompressorPlugin::deactivate()
{
}

double RobotBarkCompressorPlugin::goertzel(int size, float const *data, int sample_fq, int detect_fq)
{
    double omega = M_PI_2 * detect_fq / sample_fq;
    double sine = sin(omega);
    double cosine = cos(omega);
    double coeff = cosine * 2;
    double q0 = 0;
    double q1 = 0;
    double q2 = 0;

    for (int i = 0; i < size; i++) {
        q0 = coeff * q1 - q2 + data[i];
        q2 = q1;
        q1 = q0;
    }

    double real = (q1 - q2 * cosine) / (size / 2.0);
    double imag = (q2 * sine) / (size / 2.0);

    return sqrt(real * real + imag * imag);
}

void RobotBarkCompressorPlugin::run(const float** inputs, float** outputs, uint32_t frames, const MidiEvent* midiEvents, uint32_t midiEventCount)
{
    const float* in1 = inputs[0];
    const float* in2 = inputs[1];

    float* out1 = outputs[0];
    float* out2 = outputs[1];

    double sr    = getSampleRate();
    int    sri   = (int)sr;
    float  slope = 1 - 1 / fRatio; // TODO Funny ideas
    float sampleRateScaler;
    uint32_t windows = frames / 64;
    uint32_t samples_left = frames % 64;
    kiss_fft_scalar windowed1[64], inverted1[64], bark1_fft[64];
    kiss_fft_scalar windowed2[64], inverted2[64], bark2_fft[64];

    float bark1[frames];
    float bark2[frames];

    /*  FFT has its own buffer bark and window bark1_fft
     *  and it's own loop
     *
     *  sample rates
     *
     *  22050  = 54.56695791
     *  32000  = 57.52123620
     *  44100  = 59.06280878
     *  48000  = 59.36179537
     *  96000  = 60.82050573
     *  192000 = 61.39865212
     *
     *  TO DO: calculate for possible sample rates in default, if it helps
     *  they could be constents and we know the most used sample rates...
     *  maybe remove 2 of them.. maybe use sampleRateChanged()
     */


    switch (sri)
    {
        case 22050:
            sampleRateScaler = (54.56695791*24);
            break;

        case 32000:
            sampleRateScaler = (57.52123620*24);
            break;

        case 44100:
            sampleRateScaler = (59.06280878*24);
            break;

        case 48000:
            sampleRateScaler = (59.36179537*24)*(2*PI_F);
            break;

        case 96000:
            sampleRateScaler = (60.82050573*24);
            break;

        case 192000:
            sampleRateScaler = (61.39865212*24);
            break;

        default:
            sampleRateScaler = 0.0;
            break;
    }

    /*************************************************************************************************************\
     *  kiss_fftr_cfg KISS_FFT_API kiss_fftr_alloc(int nfft,int inverse_fft,void * mem, size_t * lenmem)         *
     *      nfft must be even                                                                                    *
     *      If you don't care to allocate space, use mem = lenmem = NULL                                         *
     *                                                                                                           *
     *  void KISS_FFT_API kiss_fftr(kiss_fftr_cfg cfg,const kiss_fft_scalar *timedata,kiss_fft_cpx *freqdata)    *
     *      input timedata has nfft scalar points                                                                *
     *      output freqdata has nfft/2+1 complex points                                                          *
     *                                                                                                           *
     *  void KISS_FFT_API kiss_fftri(kiss_fftr_cfg cfg,const kiss_fft_cpx *freqdata,kiss_fft_scalar *timedata)   *
     *      input freqdata has  nfft/2+1 complex points                                                          *
     *      output timedata has nfft scalar points                                                               *
    \*************************************************************************************************************/

    for (uint32_t i = 0; i < frames; ++i)
    {
        // (13.0*atan(Hz/1315.8) + 3.5*atan(pow((Hz/7500.0),2)))/sampleRateScaler
    }

    /*
     * TODO Comment compression loop enough to see whats going on
     *
     */

    for (uint32_t i = 0; i < frames; ++i)
    {
        // -----------------------------------------------------------------------
        // Left

        float sideInput1 = in1[i];//bark1[i];

        float c1 = sideInput1 >= state1 ? cAT : cRT;
        float env1 = sideInput1 + c1 * (state1 - sideInput1);
        float env_db1 = 10*log10(env1);
        state1 = env1;

        float gain1 = slope * (fThreshold - env_db1);
        gain1 = fmin(0.f, gain1);
        gain1 = pow(10, (gain1/20));

        out1[i] = in1[i] * gain1 * (pow(10, (fMakeUpGain/20)));

        // -----------------------------------------------------------------------
        // Right

        float sideInput2 = in2[i];//bark2[i];

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

// -------------------------------------------------------------------------------

Plugin* createPlugin()
{
    return new RobotBarkCompressorPlugin();
}

// -------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
