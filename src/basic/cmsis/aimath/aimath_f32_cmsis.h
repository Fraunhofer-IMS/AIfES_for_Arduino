/**
 * \file basic/cmsis/aimath/aimath_f32_cmsis.h
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

#ifndef AIMATH_F32_CMSIS
#define AIMATH_F32_CMSIS

#include "basic/base/aimath/aimath_f32.h"


/** @brief Performs a matrix multiplication of f32 tensors a and b and adds a 1D tensor c to each row, using the ARM CMSIS DSP
  *
  * @details 	The addition of the horizontal vector c is performed via broadcast, i.e. elementwise in each column
  *				Mathematically this broadcast is equal to multiplying c with an vertical vector of ones (with the same number of elements as c)
  *
  * @details
  *				and adding the result to a * b
  * @f[
  *  result = a \cdot b + \left( \begin{array}{r}
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
					1 & 0  \\
					0 & 1  \\
					0 & 0
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
  *  result = a \cdot b + \left( \begin{array}{r}
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
  * @param *a f32 tensor a
  * @param *b f32 tensor b
  * @param *c Tensor c, 1 row and as many columns as the result
  * @param *result Result f32 tensor
  */
void aimath_f32_cmsis_linear(const aitensor_t *a, const aitensor_t *b, const aitensor_t *c, aitensor_t *result);

/** @brief Performs a matrix multiplication of f32 tensors a and b, using the ARM CMSIS DSP
  *
  * @details
  * @f[
  *  result = a \cdot b
  * @f]
  *
  * @param *a f32 tensor a
  * @param *b f32 tensor b
  * @param *result Result f32 tensor of the multiplication
  */
void aimath_f32_cmsis_mat_mul(const aitensor_t *a, const aitensor_t *b, aitensor_t *result);

#endif // AIMATH_F32_CMSIS

#endif // AIFES_WITH_CMSIS
#endif //__arm__
