#pragma once
#include <cstdint>
#include <cmath>
/*
 * Simpler buffer player
 */
class RobotBufferPlayer
{
public:
    RobotBufferPlayer(double sampleRate, double startTime=0.0, float startParamter=0.0f)
    :    parameter(startParamter), sr(sampleRate)
    {
        setTime(startTime);
    }
    void setSampleRate(double sampleRate)
    {
        sr = sampleRate;
        setTime(time);
    }
    void setTime(float value)
    {
        time = value;
        samples = (uint32_t)sr*0.001*time;
        head = samples;
    }
    void setSamples(uint32_t value)
    {
        samples = value;
        head = samples;
        time =   (double)samples/(sr*0.001);
    }
    inline double getTime()
    {
        return time;
    }
    inline uint32_t getHead()
    {
        return head;
    }
    uint32_t getSampels()
    {
        return samples;
    }
    float processBoolTrigger(float &in, bool &trigger)
    {
        if(trigger)
        {
            head=0;
            trigger = false;
        }
        if(head!=samples)
        {
            head+=1;
            return in;
        }
        else return 0.0f;
    }
    float processChangeTrigger(float &in, float &parameterValue)
    {
        if(parameter!=parameterValue)
            head=0;
        if(head!=samples)
        {
            head+=1;
            return in;
        }
        else return 0.0f;
    }
private:
    double time, parameter;
    double sr;
    uint32_t samples, head;
};
