/*
 *  Robot Audio Plugins
 *  
 *  Copyright (C) 2023      Martin Bångens
 *  Copyright (c) 2013-2014 Pascal Gauthier
 *  Copyright (c) 2013-2014 Filatov Vadim
 *
 *  Filter taken from the Obxd project :
 *    https://github.com/asb2m10/dexed
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
#include "RobotHexedFilterDSP.hpp"
RobotHexedFilterDSP::RobotHexedFilterDSP(double sampleRate, float cutoff, float resonance, float mode)
    : sr(sampleRate)  
{
    setCutOff(cutoff);
    setResonance(resonance);
    setMode(mode);
}
void RobotHexedFilterDSP::setCutOff(float value)
{
    cutoffNorm   = logsc(value,60,19000);
    g            = (float)tan(cutoffNorm * srateInv * PI_F);
    br           = bright - ((bright-1)*(1.0-((cutoffNorm-60)*0.000000016)));
    lpc          = g / (1 + g);
}

void RobotHexedFilterDSP::setResonance(float value)
{
    rReso       = (0.991-logsc(1-value, 0, 0.991));
    R24         =  3.7 * rReso;

}
inline float RobotHexedFilterDSP::modeLower(float value)
{
    return 1.0f-value;
}
inline float RobotHexedFilterDSP::modeRise(float value)
{
    return value;
}
void RobotHexedFilterDSP::setMode(float value) // range 1-4 but should go towards whole number
{
    
    if(value<1.0f) value = 1.0f;
    if(value>4.0f) value = 4.0f;
    int offset = (int)value;
    float remain = value-offset;
    // set all to 0 if not exact
    value==1.0f ? mmt_y1 = 1.0f : mmt_y1 = 0.0f;
    value==2.0f ? mmt_y2 = 1.0f : mmt_y2 = 0.0f;
    value==3.0f ? mmt_y3 = 1.0f : mmt_y3 = 0.0f;
    value==4.0f ? mmt_y4 = 1.0f : mmt_y4 = 0.0f;
    // if it was exact return
    if(remain==0)
        return;
    // Parallel mix
    switch (offset)
    {
        case 1:
            mmt_y1 = modeLower(remain);
            mmt_y2 = modeRise(remain);
            break;
        case 2:
            mmt_y2 = modeLower(remain);
            mmt_y3 = modeRise(remain);
            break;
        case 3:
            mmt_y3 = modeLower(remain);
            mmt_y4 = modeRise(remain);
            break;
    }
}

float RobotHexedFilterDSP::responseDb(float scaledFreq) const
{
	// This was an example for another filter 
    //float w = scaledFreq*2*M_PI;
	//std::complex<Sample> invZ = {std::cos(w), -std::sin(w)}, invZ2 = invZ*invZ;
	//float energy = std::norm(b0 + invZ*b1 + invZ2*b2)/std::norm(1 + invZ*a1 + invZ2*a2);
	//return 10*std::log10(energy);
    return scaledFreq;
}

// -----------------------------------------------------------------------
// Process

void RobotHexedFilterDSP::flush(double srate)
{
    sr = (float)srate;
    srateInv = 1/srate;

    s1=s2=s3=s4=c=d=0;

    R24=0;

    mmt_y1=mmt_y2=mmt_y3=mmt_y4=0; 

    float rcrate = sqrt((44000/srate));
    rcor24 = (970.0/44000)*rcrate;
    rcor24Inv = 1/rcor24;

    bright =  (sin((44000/srate)*(43900/44000) * PI_F * srateInv))/
              (cos((44000/srate)*(43900/44000) * PI_F * srateInv));

    dc_r = 1.0-(126.0/srate);
    dc_tmp = 0;
}


float RobotHexedFilterDSP::logsc(float param, const float min, const float max, const float rolloff)
{
    return ((expf(param * logf(rolloff+1)) - 1.0f) / (rolloff)) * (max-min) + min;
}

float RobotHexedFilterDSP::tptpc(float& state, float inp, float cutoff)
{
    double v   = (inp - state) * cutoff / (1 + cutoff);
    double res = v + state;
    state      = res + v;
    return res;
}

float RobotHexedFilterDSP::NR24(float sample, float g, float lpc)
{
    float ml = 1 / (1+g);
    float S  = (lpc*(lpc*(lpc*s1+s2)+s3)+s4)*ml;
    float G  = lpc*lpc*lpc*lpc;
    float y  = (sample - R24*S) / (1 + R24*G);
    return y + 1e-8;
}

float RobotHexedFilterDSP::process(float x)
{
    // Simple DC filter
    float dc_prev = x;
                x = x - dc_tmp + dc_r * dc_tmp;
           dc_tmp = dc_prev;
    // Remove a bit under 15
                x = x - 0.45*tptpc(c, x, (15 * srateInv)* PI_F);
    // Add bright value..
                x = tptpc(d, x, br);

    // All states in a recursive composite pre order
    // controlled by resonance
    // add dc back
         float y0 = NR24(x, g, lpc);

    // First low pass in cascade
    double res = tptpc(s1,y0,g);
    // Damping
    s1       = atan(s1*rcor24)*rcor24Inv;
    float y1 = res;
    float y2 = tptpc(s2,y1,g);
    float y3 = tptpc(s3,y2,g);
    float y4 = tptpc(s4,y3,g);
    // Multi-mode mixer
    float mc = mmt_y1*y1 + mmt_y2*y2 + mmt_y3*y3 + mmt_y4*y4;
    return (mc * ( 1 + R24 * 0.45 )) * (1-(mm_balancer*rReso*0.96422));
}


// -----------------------------------------------------------------------
