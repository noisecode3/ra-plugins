/*
 * Copyright (c) 2007 - 2021 Joseph Gaeddert
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

//
// firfilt : finite impulse response (FIR) filter
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// defined:
//  FIRFILT()       name-mangling macro
//  T               coefficients type
//  WINDOW()        window macro
//  DOTPROD()       dotprod macro
//  PRINTVAL()      print macro

// NOTE: using the window is about 27% slower, but fixes a valgrind issue
#define LIQUID_FIRFILT_USE_WINDOW   (1)

// firfilt object structure
struct FIRFILT(_s) {
    TC * h;             // filter coefficients array [size; h_len x 1]
    unsigned int h_len; // filter length

#if LIQUID_FIRFILT_USE_WINDOW
    // use window object for internal buffer
    WINDOW() w;
#else
    // use array as internal buffer (faster)
    TI * w;                 // internal buffer object
    unsigned int w_len;     // window length
    unsigned int w_mask;    // window index mask
    unsigned int w_index;   // window read index
#endif
    DOTPROD() dp;           // dot product object
    TC scale;               // output scaling factor
};

// create firfilt object
//  _h      :   coefficients (filter taps) [size: _n x 1]
//  _n      :   filter length
FIRFILT() FIRFILT(_create)(TC * _h,
                           unsigned int _n)
{
    // validate input
    if (_n == 0)
        return liquid_error_config("firfilt_%s_create(), filter length must be greater than zero", EXTENSION_FULL);

    // create filter object and initialize
    FIRFILT() q = (FIRFILT()) malloc(sizeof(struct FIRFILT(_s)));
    q->h_len = _n;
    q->h = (TC *) malloc((q->h_len)*sizeof(TC));

#if LIQUID_FIRFILT_USE_WINDOW
    // create window (internal buffer)
    q->w = WINDOW(_create)(q->h_len);
#else
    // initialize array for buffering
    q->w_len   = 1<<liquid_msb_index(q->h_len); // effectively 2^{floor(log2(len))+1}
    q->w_mask  = q->w_len - 1;
    q->w       = (TI *) malloc((q->w_len + q->h_len + 1)*sizeof(TI));
    q->w_index = 0;
#endif

    // move coefficients
    memmove(q->h, _h, (q->h_len)*sizeof(TC));

    // create dot product object with coefficients in reverse order
    q->dp = DOTPROD(_create_rev)(q->h, q->h_len);

    // set default scaling
    q->scale = 1;

    // reset filter state (clear buffer)
    FIRFILT(_reset)(q);

    return q;
}

// create filter using Kaiser-Bessel windowed sinc method
//  _n      : filter length, _n > 0
//  _fc     : cutoff frequency, 0 < _fc < 0.5
//  _As     : stop-band attenuation [dB], _As > 0
//  _mu     : fractional sample offset, -0.5 < _mu < 0.5
FIRFILT() FIRFILT(_create_kaiser)(unsigned int _n,
                                  float        _fc,
                                  float        _As,
                                  float        _mu)
{
    // validate input
    if (_n == 0)
        return liquid_error_config("firfilt_%s_create_kaiser(), filter length must be greater than zero", EXTENSION_FULL);

    // compute temporary array for holding coefficients
    float hf[_n];
    liquid_firdes_kaiser(_n, _fc, _As, _mu, hf);

    // copy coefficients to type-specific array
    TC h[_n];
    unsigned int i;
    for (i=0; i<_n; i++)
        h[i] = (TC) hf[i];

    // 
    return FIRFILT(_create)(h, _n);
}

// create from square-root Nyquist prototype
//  _type   : filter type (e.g. LIQUID_FIRFILT_RRC)
//  _k      : nominal samples/symbol, _k > 1
//  _m      : filter delay [symbols], _m > 0
//  _beta   : rolloff factor, 0 < beta <= 1
//  _mu     : fractional sample offset,-0.5 < _mu < 0.5
FIRFILT() FIRFILT(_create_rnyquist)(int          _type,
                                    unsigned int _k,
                                    unsigned int _m,
                                    float        _beta,
                                    float        _mu)
{
    // validate input
    if (_k < 2)
        return liquid_error_config("firfilt_%s_create_rnyquist(), filter samples/symbol must be greater than 1", EXTENSION_FULL);
    if (_m == 0)
        return liquid_error_config("firfilt_%s_create_rnyquist(), filter delay must be greater than 0", EXTENSION_FULL);
    if (_beta < 0.0f || _beta > 1.0f)
        return liquid_error_config("firfilt_%s_create_rnyquist(), filter excess bandwidth factor must be in [0,1]", EXTENSION_FULL);

    // generate square-root Nyquist filter
    unsigned int h_len = 2*_k*_m + 1;
    float hf[h_len];
    liquid_firdes_prototype(_type,_k,_m,_beta,_mu,hf);

    // copy coefficients to type-specific array (e.g. float complex)
    unsigned int i;
    TC hc[h_len];
    for (i=0; i<h_len; i++)
        hc[i] = hf[i];

    // return filter object and return
    return FIRFILT(_create)(hc, h_len);
}

// Create object from Parks-McClellan algorithm prototype
//  _h_len  : filter length, _h_len > 0
//  _fc     : cutoff frequency, 0 < _fc < 0.5
//  _As     : stop-band attenuation [dB], _As > 0
FIRFILT() FIRFILT(_create_firdespm)(unsigned int _h_len,
                                    float        _fc,
                                    float        _As)
{
    // validate input
    if (_h_len < 1)
        return liquid_error_config("firfilt_%s_create_firdespm(), filter samples/symbol must be greater than 1", EXTENSION_FULL);
    if (_fc < 0.0f || _fc > 0.5f)
        return liquid_error_config("firfilt_%s_create_firdespm(), filter cutoff frequency must be in (0,0.5]", EXTENSION_FULL);

    // generate square-root Nyquist filter
    float hf[_h_len];
    firdespm_lowpass(_h_len,_fc,_As,0,hf);

    // copy coefficients to type-specific array (e.g. float complex)
    // and scale by filter bandwidth to be consistent with other lowpass prototypes
    unsigned int i;
    TC hc[_h_len];
    for (i=0; i<_h_len; i++)
        hc[i] = hf[i] * 0.5f / _fc;

    // return filter object and return
    return FIRFILT(_create)(hc, _h_len);
}

// create rectangular filter prototype
FIRFILT() FIRFILT(_create_rect)(unsigned int _n)
{
    // validate input
    if (_n == 0 || _n > 1024)
        return liquid_error_config("firfilt_%s_create_rect(), filter length must be in [1,1024]", EXTENSION_FULL);

    // create float array coefficients
    float hf[_n];
    unsigned int i;
    for (i=0; i<_n; i++)
        hf[i] = 1.0f;

    // copy coefficients to type-specific array
    TC h[_n];
    for (i=0; i<_n; i++)
        h[i] = (TC) hf[i];

    // return filter object and return
    return FIRFILT(_create)(h, _n);
}

// create DC blocking filter
FIRFILT() FIRFILT(_create_dc_blocker)(unsigned int _m,
                                      float        _As)
{
    // validate input
    if (_m < 1 || _m > 1000)
        return liquid_error_config("firfilt_%s_create_dc_blocker(), filter semi-length (%u) must be in [1,1000]",EXTENSION_FULL, _m);
    if (_As <= 0.0f)
        return liquid_error_config("firfilt_%s_create_dc_blocker(), prototype stop-band suppression (%12.4e) must be greater than zero",EXTENSION_FULL, _As);

    // create float array coefficients and design filter
    unsigned int h_len = 2*_m+1;
    float        hf[h_len];
    liquid_firdes_notch(_m, 0, _As, hf);

    // copy coefficients to type-specific array
    TC h[h_len];
    unsigned int i;
    for (i=0; i<h_len; i++)
        h[i] = (TC) hf[i];

    // return filter object and return
    return FIRFILT(_create)(h, h_len);
}

// create notch filter
FIRFILT() FIRFILT(_create_notch)(unsigned int _m,
                                 float        _As,
                                 float        _f0)
{
    // validate input
    if (_m < 1 || _m > 1000)
        return liquid_error_config("firfilt_%s_create_notch(), filter semi-length (%u) must be in [1,1000]",EXTENSION_FULL, _m);
    if (_As <= 0.0f)
        return liquid_error_config("firfilt_%s_create_notch(), prototype stop-band suppression (%12.4e) must be greater than zero",EXTENSION_FULL, _As);
    if (_f0 < -0.5f || _f0 > 0.5f)
        return liquid_error_config("firfilt_%s_create_notch(), notch frequency (%e) must be in [-0.5,0.5]",EXTENSION_FULL, _f0);

    // create float array coefficients and design filter
    unsigned int i;
    unsigned int h_len = 2*_m+1;    // filter length
    float        hf[h_len];         // prototype filter with float coefficients
    TC           h [h_len];         // output filter with type-specific coefficients
#if TC_COMPLEX
    // design notch filter as DC blocker, then mix to appropriate frequency
    liquid_firdes_notch(_m, 0, _As, hf);
    for (i=0; i<h_len; i++) {
        float phi = 2.0f * M_PI * _f0 * ((float)i - (float)_m);
        h[i] = cexpf(_Complex_I*phi) * (TC) hf[i];
    }
#else
    // design notch filter for real-valued coefficients directly
    liquid_firdes_notch(_m, _f0, _As, hf);
    for (i=0; i<h_len; i++)
        h[i] = hf[i];
#endif

    // return filter object and return
    return FIRFILT(_create)(h, h_len);
}

// re-create firfilt object
//  _q      :   original firfilt object
//  _h      :   new coefficients [size: _n x 1]
//  _n      :   new filter length
FIRFILT() FIRFILT(_recreate)(FIRFILT() _q,
                             TC * _h,
                             unsigned int _n)
{
    unsigned int i;

    // reallocate memory array if filter length has changed
    if (_n != _q->h_len) {
        // reallocate memory
        _q->h_len = _n;
        _q->h = (TC*) realloc(_q->h, (_q->h_len)*sizeof(TC));

#if LIQUID_FIRFILT_USE_WINDOW
        // recreate window object, preserving internal state
        _q->w = WINDOW(_recreate)(_q->w, _q->h_len);
#else
        // free old array
        free(_q->w);

        // initialize array for buffering
        _q->w_len   = 1<<liquid_msb_index(_q->h_len);   // effectively 2^{floor(log2(len))+1}
        _q->w_mask  = _q->w_len - 1;
        // FIXME: valgrind is complaining about an uninitialized variable in the following malloc line
        _q->w       = (TI *) malloc((_q->w_len + _q->h_len + 1)*sizeof(TI));
        _q->w_index = 0;
#endif
    }

    // load filter in reverse order
    for (i=_n; i>0; i--)
        _q->h[i-1] = _h[_n-i];

    // re-create internal dot product object
    _q->dp = DOTPROD(_recreate)(_q->dp, _q->h, _q->h_len);

    return _q;
}

// destroy firfilt object
void FIRFILT(_destroy)(FIRFILT() _q)
{
#if LIQUID_FIRFILT_USE_WINDOW
    WINDOW(_destroy)(_q->w);
#else
    free(_q->w);
#endif
    DOTPROD(_destroy)(_q->dp);
    free(_q->h);
    free(_q);
}

// reset internal state of filter object
void FIRFILT(_reset)(FIRFILT() _q)
{
#if LIQUID_FIRFILT_USE_WINDOW
    WINDOW(_reset)(_q->w);
#else
    unsigned int i;
    for (i=0; i<_q->w_len; i++)
        _q->w[i] = 0.0;
    _q->w_index = 0;
#endif
}

// print filter object internals (taps, buffer)
void FIRFILT(_print)(FIRFILT() _q)
{
    printf("firfilt_%s:\n", EXTENSION_FULL);
    unsigned int i;
    unsigned int n = _q->h_len;
    for (i=0; i<n; i++) {
        printf("  h(%3u) = ", i+1);
        PRINTVAL_TC(_q->h[n-i-1],%12.8f);
        printf("\n");
    }

    // print scaling
    printf("  scale = ");
    PRINTVAL_TC(_q->scale,%12.8f);
    printf("\n");

#if LIQUID_FIRFILT_USE_WINDOW
    WINDOW(_print)(_q->w);
#endif
}

// set output scaling for filter
void FIRFILT(_set_scale)(FIRFILT() _q,
                         TC        _scale)
{
    _q->scale = _scale;
}

// get output scaling for filter
void FIRFILT(_get_scale)(FIRFILT() _q,
                         TC *      _scale)
{
    *_scale = _q->scale;
}

// push sample into filter object's internal buffer
//  _q      :   filter object
//  _x      :   input sample
void FIRFILT(_push)(FIRFILT() _q,
                    TI        _x)
{
#if LIQUID_FIRFILT_USE_WINDOW
    WINDOW(_push)(_q->w, _x);
#else
    // increment index
    _q->w_index++;

    // wrap around pointer
    _q->w_index &= _q->w_mask;

    // if pointer wraps around, copy excess memory
    if (_q->w_index == 0)
        memmove(_q->w, _q->w + _q->w_len, (_q->h_len)*sizeof(TI));

    // append value to end of buffer
    _q->w[_q->w_index + _q->h_len - 1] = _x;
#endif
}

// Write block of samples into filter object's internal buffer
//  _q      : filter object
//  _x      : buffer of input samples, [size: _n x 1]
//  _n      : number of input samples
void FIRFILT(_write)(FIRFILT()    _q,
                     TI *         _x,
                     unsigned int _n)
{
#if LIQUID_FIRFILT_USE_WINDOW
    WINDOW(_write)(_q->w, _x, _n);
#else
    // TODO: be smarter about this
    unsigned int i;
    for (i=0; i<_n; i++)
        FIRFILT(_push)(_q, _x[i]);
#endif
}

// compute output sample (dot product between internal
// filter coefficients and internal buffer)
//  _q      :   filter object
//  _y      :   output sample pointer
void FIRFILT(_execute)(FIRFILT() _q,
                       TO *      _y)
{
    // read buffer (retrieve pointer to aligned memory array)
#if LIQUID_FIRFILT_USE_WINDOW
    TI *r;
    WINDOW(_read)(_q->w, &r);
#else
    TI *r = _q->w + _q->w_index;
#endif

    // execute dot product
    DOTPROD(_execute)(_q->dp, r, _y);

    // apply scaling factor
    *_y *= _q->scale;
}

// execute the filter on a block of input samples; the
// input and output buffers may be the same
//  _q      : filter object
//  _x      : pointer to input array [size: _n x 1]
//  _n      : number of input, output samples
//  _y      : pointer to output array [size: _n x 1]
void FIRFILT(_execute_block)(FIRFILT()    _q,
                             TI *         _x,
                             unsigned int _n,
                             TO *         _y)
{
    unsigned int i;
    for (i=0; i<_n; i++) {
        // push sample into filter
        FIRFILT(_push)(_q, _x[i]);

        // compute output sample
        FIRFILT(_execute)(_q, &_y[i]);
    }
}

// get filter length
unsigned int FIRFILT(_get_length)(FIRFILT() _q)
{
    return _q->h_len;
}

// Get pointer to coefficients array
const TC * FIRFILT(_get_coefficients)(FIRFILT() _q)
{
    return (const TC *) _q->h;
}

// Copy internal coefficients to external buffer
int FIRFILT(_copy_coefficients)(FIRFILT() _q,
                                TC *      _h)
{
    // internal coefficients are stored in normal order
    memmove(_h, _q->h, (_q->h_len)*sizeof(TC));
    return LIQUID_OK;
}

// compute complex frequency response
//  _q      :   filter object
//  _fc     :   frequency
//  _H      :   output frequency response
void FIRFILT(_freqresponse)(FIRFILT()       _q,
                            float           _fc,
                            float complex * _H)
{
    unsigned int i;
    float complex H = 0.0f;

    // compute dot product between coefficients and exp{ 2 pi fc {0..n-1} }
    for (i=0; i<_q->h_len; i++)
        H += _q->h[i] * cexpf(_Complex_I*2*M_PI*_fc*i);

    // apply scaling
    H *= _q->scale;

    // set return value
    *_H = H;
}


// compute group delay in samples
//  _q      :   filter object
//  _fc     :   frequency
float FIRFILT(_groupdelay)(FIRFILT() _q,
                           float     _fc)
{
    // copy coefficients
    float h[_q->h_len];
    unsigned int i;
    for (i=0; i<_q->h_len; i++)
        h[i] = crealf(_q->h[i]);

    return fir_group_delay(h, _q->h_len, _fc);
}
