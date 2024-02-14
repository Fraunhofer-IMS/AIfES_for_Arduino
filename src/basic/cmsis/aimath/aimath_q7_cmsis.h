/**
 * \file basic/cmsis/aimath/aimath_q7_cmsis.h
 * \internal
 * \date 15.03.2021
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
 * \brief
 * \details
 */

#include "aifes_config.h"

#if __arm__
#ifdef AIFES_WITH_CMSIS

#ifndef AIMATH_Q7_CMSIS
#define AIMATH_Q7_CMSIS

#include "basic/base/aimath/aimath_q7.h"
#include "basic/base/aimath/aimath_q31.h"


/** @brief Performs a matrix multiplication of \link aimath_q7.h Q7 \endlink matrices a and b (transposed) and adds a \link aimath_q31.h Q31 \endlink vector c to each row
 *
 * The addition of the horizontal vector c is performed via broadcast, i.e. element wise in each column
 * Mathematically this broadcast is equal to multiplying c with an vertical vector (with the same number of elements as c)
 * and adding the result to \f$ a \cdot b^T \f$.
 *
 * **The quantization parameters of the vector c have to be {zero_point = 0, shift = a.shift + b.shift}!**
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
 * 	@f[
 *  a =  \left( \begin{array}{rrr}
				1 & 2 & 3 \\
				4 & 5 & 6 \\
				7 & 8 & 9
				\end{array}\right)
 * @f]
 *
 * @f[
 *  b =  \left( \begin{array}{rr}
				1 & 0 & 0 \\
				0 & 1 & 0
                \end{array}\right)
 * @f]
 *
 * @f[
 *  c =  \left( \begin{array}{rr}
				2 & 5
				\end{array}\right)
 * @f]
 *
 * @f[
 *  result = a \cdot b^T + \left( \begin{array}{r}
								 1 \\
							 1 \\
							 1 \\
							 \end{array}\right)  \cdot c
 * @f]
 *
 * @f[
 *   = \left( \begin{array}{rr}
				1 & 2  \\
				4 & 5  \\
				7 & 8
				\end{array}\right)
		 + \left( \begin{array}{rr}
                2 & 5  \\
				2 & 5  \\
				2 & 5
				\end{array}\right)
 * @f]
 *
 * @details
 *
 * @f[
 *   = \left( \begin{array}{rr}
				3 & 7  \\
				6 & 10  \\
				9 & 13
				\end{array}\right)
 * @f]
 *
 * Example:
 * \code{.c}
 * uint16_t a_shape[2] = {3, 3};
 * aimath_q7_params_t a_params = {1, 0}; // {shift, zero point}
 * int8_t a_data[3*3] = { 2,  4,  6,
 *                        8, 10, 12,
 *                       14, 16, 18};
 * aitensor_t a = AITENSOR_2D_Q7(a_shape, &a_params, a_data);
 *
 * uint16_t b_shape[2] = {2, 3};
 * aimath_q7_params_t b_params = {2, 0}; // {shift, zero point}
 * int8_t b_data[2*3] = {4, 0, 0,
 *                       0, 4, 0};
 * aitensor_t b = AITENSOR_2D_Q7(b_shape, &b_params, b_data);
 *
 * uint16_t c_shape[2] = {1, 2};
 * aimath_q31_params_t c_params = {3, 0}; // {shift, zero point}
 * int32_t c_data[1*2] = {16, 40};
 * aitensor_t c = AITENSOR_2D_Q31(c_shape, &c_params, c_data);
 *
 * uint16_t result_shape[2] = {3, 2};
 * aimath_q7_params_t result_params = {1, 0}; // {shift, zero point}
 * int8_t result_data[3*2];
 * aitensor_t result = AITENSOR_2D_Q7(result_shape, &result_params, result_data);
 *
 * aimath_q7_cmsis_linear32_bt(&a, &b, &c, &result);
 *
 * print_aitensor(&result);
 * \endcode
 *
 * @param *a        Q7 matrix a (2D tensor of shape [N x K])
 * @param *b        Q7 matrix b (2D tensor of shape [M x K])
 * @param *c        Q31 vector c (2D tensor of shape [1 x M] or 1D tensor of shape [M])
 * @param *result   Resulting Q7 matrix (2D tensor of shape [N x M])
 */
void aimath_q7_cmsis_linear32_bt(const aitensor_t *a, const aitensor_t *b, const aitensor_t *c, aitensor_t *result);

#endif // AIMATH_Q7_CMSIS

#endif // AIFES_WITH_CMSIS
#endif //__arm__
