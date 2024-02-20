/**
 * \file basic/avr_pgm/aimath/aimath_f32_avr_pgm.h
 * \internal
 * \date 16.11.2021
 * \endinternal
 * \version 2.2.0
 * \copyright  Copyright (C) 2020-2023  Fraunhofer Institute for Microelectronic Circuits and Systems.
    All rights reserved.<br><br>
    AIfES is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.<br><br>
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.<br><br>
    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * \brief Math functions for \link aimath_f32.h F32 \endlink data type using AVR pgmspace.h library for constant data storage
 *
 * These functions modify the default implementation of the F32 math functions to work with parameters,
 * stored in the program memory of AVR controllers.
 *
 * The library [avr/pgmspace.h](https://www.nongnu.org/avr-libc/user-manual/pgmspace_8h.html) is required.
 */

#include "aifes_config.h"

#if __AVR__
#ifdef AIFES_WITH_AVR_PGM

#ifndef AIMATH_F32_AVR_PGM_H
#define AIMATH_F32_AVR_PGM_H

#include "basic/default/aimath/aimath_f32_default.h"

/** @brief Performs a matrix multiplication of \link aimath_f32.h F32 \endlink matrices a and b and adds a vector c to each row
 *
 * The data of b and c must be defined constant in PROGMEM.
 *
 * Same functionality as aimath_f32_default_linear().
 *
 * The addition of the horizontal vector c is performed via broadcast, i.e. element wise in each column
 * Mathematically this broadcast is equal to multiplying c with an vertical vector (with the same number of elements as c)
 * and adding the result to a * b
 *
 * @f[
 *  result = a \cdot b + \left( \begin{array}{c}
 							1  \\
							1 \\
							\vdots \\
							1  \\
							\end{array}\right)  \cdot c
 * @f]
 *
 * Example:
 * \code{.c}
 * uint16_t a_shape[2] = {3, 3};
 * float a_data[3*3] = {1.0f, 2.0f, 3.0f,
 *                      4.0f, 5.0f, 6.0f,
 *                      7.0f, 8.0f, 9.0f};
 * aitensor_t a = AITENSOR_2D_F32(a_shape, a_data);
 *
 * uint16_t b_shape[2] = {3, 2};
 * const float b_data[3*2] PROGMEM = {1.0f, 0.0f,
 *                                    0.0f, 1.0f,
 *                                    0.0f, 0.0f};
 * aitensor_t b = AITENSOR_2D_F32(b_shape, b_data);
 *
 * uint16_t c_shape[2] = {1, 2};
 * const float c_data[1*2] PROGMEM = {2.0f, 5.0f};
 * aitensor_t c = AITENSOR_2D_F32(c_shape, c_data);
 *
 * uint16_t result_shape[2] = {3, 2};
 * float result_data[3*2];
 * aitensor_t result = AITENSOR_2D_F32(result_shape, result_data);
 *
 * aimath_f32_avr_pgm_linear(&a, &b, &c, &result);
 * \endcode
 *
 * @param *a        F32 matrix a (2D tensor of shape [N x K])
 * @param *b        F32 matrix b (2D tensor of shape [K x M]) (data const in PROGMEM)
 * @param *c        F32 vector c (2D tensor of shape [1 x M]) (data const in PROGMEM)
 * @param *result   Resulting F32 matrix (2D tensor of shape [N x M])
 */
void aimath_f32_avr_pgm_linear(const aitensor_t *a, const aitensor_t *b, const aitensor_t *c, aitensor_t *result);

#endif // AIMATH_F32_AVR_PGM_H

#endif // AIFES_WITH_AVR_PGM
#endif // __AVR__
