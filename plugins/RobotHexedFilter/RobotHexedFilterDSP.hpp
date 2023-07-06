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
#pragma once
#include <cmath>

#define PI_F 3.1415927410125732421875f
#define E_F  2.7182818284590452353602f

class RobotHexedFilterDSP
{
public:
    RobotHexedFilterDSP(double sr, float cutoff =1.0f, float resonance=0.0f, float mode=4.0f);
    float process(float x);
    float responseDb(float scaledFreq) const;
    void setCutOff(float value);
    void setResonance(float value);
    void setMode(float value);
    void flush(double sr);
protected:
// -------------------------------------------------------------------
// Dsp 
    
    float rReso;
    float cutoffNorm;
    float g; //
    float lpc;
    float br;

    float sr;
    float srateInv;
    float s1,s2,s3,s4;
    float d, c;
    float R24;
    float rcor24,rcor24Inv;
    float bright;
    float mm_balancer = 0.7578f;
    
    // 24 db multimode
    int   mmch=4;
    float mmt_y1=0.0f, mmt_y2=0.0f, mmt_y3=0.0f, mmt_y4=1.0f;

    float dc_tmp;
    float dc_r;
    

    float logsc(float param, const float min, const float max, const float rolloff = 19.0f);
    float tptpc(float& state, float inp, float cutoff);
    float NR24(float sample, float g, float lpc);
    float modeLower(float value);
    float modeRise(float value);
};
