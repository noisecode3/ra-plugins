/**
 * One-pole LPF for smooth parameter changes
 *
 * https://www.musicdsp.org/en/latest/Filters/257-1-pole-lpf-for-smooth-parameter-changes.html
 */

#ifndef C_PARAM_SMOOTH_H
#define C_PARAM_SMOOTH_H

#include <math.h>
#include <cstdint>
#define TWO_PI 6.283185307179586476925286766559f

class LPFSmooth
{
public:
    LPFSmooth(float smoothingTimeMs, float samplingRate)
        : t(smoothingTimeMs)
    {
        setSampleRate(samplingRate);
    }
    ~LPFSmooth() { }
    void setSampleRate(float samplingRate)
    {
        if (samplingRate != fs)
        {
            fs = samplingRate;
            a = exp(-TWO_PI / (t * 0.001f * samplingRate));
            b = 1.0f - a;
            z = 0.0f;
        }
    }
    inline void flush()
    {
        z = 0.0f;
    }
    inline float process(float in)
    {
        return z = (in * b) + (z * a);
    }
private:
    float a, b, t, z;
    double fs = 0.0;
};

/* Linear interpolation
 *
 *
 */
class LISmooth
{
public:
    LISmooth(float smoothingTimeMs, float samplingRate)
        : t(smoothingTimeMs)
    {
        setSampleRate(samplingRate);
    }
    ~LISmooth() { }
    void setSampleRate(float samplingRate)
    {
        if (samplingRate != fs)
        {
            fs = samplingRate;
            tail = (uint32_t)(t * 0.001 * samplingRate);
        }
    }
    inline void flush()
    {
        tick = 0;
    }
    inline float step()
    {
        return ((end-start)/tail) * (tail-tick);
    }
    float process(float in)
    {
        if(in==end)
        {
            if(tick)
            {
                float out = start + step();
                tick-=1;
                return out;
            }
            return in;
        }
        if(tick==0)
            start = end;
        else start = start + step();
        end = in;
        tick = tail-1;
        return start + step();
    }
private:
    float start = 0.0f;
    float end = 0.0f;
    double fs = 0.0;
    double t = 0.0;
    uint32_t tick = 0;
    uint32_t tail = 0;
};


#endif  // #ifndef C_PARAM_SMOOTH_H
