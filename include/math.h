/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-common-lib
 * Created on: 2 апр. 2020 г.
 *
 * lsp-common-lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-common-lib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-common-lib. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef LSP_PLUG_IN_STDLIB_MATH_H_
#define LSP_PLUG_IN_STDLIB_MATH_H_

#include <lsp-plug.in/common/version.h>
#include <lsp-plug.in/common/types.h>

#include <math.h>

// Define missing constants
#ifndef M_E
    #define M_E     2.7182818284590452354   /* e */
#endif

#ifndef M_LOG2E
    #define M_LOG2E     1.4426950408889634074   /* log_2 e */
#endif

#ifndef M_LOG10E
    #define M_LOG10E    0.43429448190325182765  /* log_10 e */
#endif

#ifndef M_LOG10_2
    #define M_LOG10_2   0.301029995664          /* log10(2) */
#endif

#ifndef M_LN2
    #define M_LN2       0.69314718055994530942  /* log_e 2 */
#endif

#ifndef M_LN10
    #define M_LN10      2.30258509299404568402  /* log_e 10 */
#endif

#ifndef M_PI
    #define M_PI        3.14159265358979323846  /* pi */
#endif

#ifndef M_PI_2
    #define M_PI_2      1.57079632679489661923  /* pi/2 */
#endif

#ifndef M_PI_4
    #define M_PI_4      0.78539816339744830962  /* pi/4 */
#endif

#ifndef M_1_PI
    #define M_1_PI      0.31830988618379067154  /* 1/pi */
#endif

#ifndef M_2_PI
    #define M_2_PI      0.63661977236758134308  /* 2/pi */
#endif

#ifndef M_2_SQRTPI
    #define M_2_SQRTPI  1.12837916709551257390  /* 2/sqrt(pi) */
#endif

#ifndef M_SQRT2
    #define M_SQRT2 1.41421356237309504880  /* sqrt(2) */
#endif

#ifndef M_SQRT1_2
    #define M_SQRT1_2   0.70710678118654752440  /* 1/sqrt(2) */
#endif

// Additional extensions
#ifndef M_GOLD_RATIO
    #define M_GOLD_RATIO        1.61803398875
#endif /* M_GOLD_RATIO */

#ifndef M_RGOLD_RATIO
    #define M_RGOLD_RATIO       0.61803398875
#endif /* M_RGOLD_RATIO */

#endif /* LSP_PLUG_IN_STDLIB_MATH_H_ */
