#pragma once
#include <cmath>
/*
 * Simpler Wet
 * I thought that setWet(float float) sounded better
 * with linear interpolation smoothing
 */
class RobotWet
{
public:
    RobotWet(float startWet=0.0f)
    {
        setWet(startWet);
    }
    inline void setWet(float value)
    {
        wet = 1.0 - exp(-value) + 0.367879*value;
    }
    inline void setLinearWet(float value)
    {
        wet = value;
    }
    inline float process(float mainIn, float effectIn)
    {
        return (mainIn*(1.0-wet)) + (effectIn*wet);
    }
private:
    float wet;
};
