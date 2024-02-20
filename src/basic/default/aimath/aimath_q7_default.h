/**
 * \file basic/default/aimath/aimath_q7_default.h
 * \internal
 * \date 28.10.2020
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
 * \brief Math functions for \link aimath_q7.h Q7 \endlink data type, default implementation
 *
 * These functions can be used when no hardware specific implementation is available.
 */

#ifndef AIMATH_Q7_DEFAULT
#define AIMATH_Q7_DEFAULT

#include <stdint.h>
#include <math.h>
#include <stdlib.h>

#include "basic/base/aimath/aimath_q7.h"
#include "basic/base/aimath/aimath_q31.h"


/** @brief Performs a matrix multiplication of \link aimath_q7.h Q7 \endlink matrices a and b and adds a \link aimath_q31.h Q31 \endlink vector c to each row
 *
 * The addition of the horizontal vector c is performed via broadcast, i.e. element wise in each column
 * Mathematically this broadcast is equal to multiplying c with an vertical vector (with the same number of elements as c)
 * and adding the result to \f$ a \cdot b \f$.
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
 * Example:
 * \code{.c}
 * uint16_t a_shape[2] = {3, 3};
 * aimath_q7_params_t a_params = {1, 0}; // {shift, zero point}
 * int8_t a_data[3*3] = { 2,  4,  6,
 *                        8, 10, 12,
 *                       14, 16, 18};
 * aitensor_t a = AITENSOR_2D_Q7(a_shape, &a_params, a_data);
 *
 * uint16_t b_shape[2] = {3, 2};
 * aimath_q7_params_t b_params = {2, 0}; // {shift, zero point}
 * int8_t b_data[3*2] = {4, 0,
 *                       0, 4,
 *                       0, 0};
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
 * aimath_q7_default_linear32(&a, &b, &c, &result);
 *
 * print_aitensor(&result);
 * \endcode
 *
 * @param *a        Q7 matrix a (2D tensor of shape [N x K])
 * @param *b        Q7 matrix b (2D tensor of shape [K x M])
 * @param *c        Q31 vector c (2D tensor of shape [1 x M] or 1D tensor of shape [M])
 * @param *result   Resulting Q7 matrix (2D tensor of shape [N x M])
 */
void aimath_q7_default_linear32(const aitensor_t *a, const aitensor_t *b, const aitensor_t *c, aitensor_t *result);

/** @brief Performs a matrix multiplication of \link aimath_q7.h Q7 \endlink matrices a and b (transposed) and adds a \link aimath_q31.h Q31 \endlink vector c to each row
 *
 * Same operation as aimath_q7_default_linear32() but with a transposed b matrix.
 *
 * The addition of the horizontal vector c is performed via broadcast, i.e. element wise in each column
 * Mathematically this broadcast is equal to multiplying c with an vertical vector (with the same number of elements as c)
 * and adding the result to \f$ a \cdot b^T \f$.
 *
 * ** The quantization parameters of the vector c have to be {zero_point = 0, shift = a.shift + b.shift}! **
 *
 * @f[
 *  result = a \cdot b^T + \left( \begin{array}{c}
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
 * aimath_q7_default_linear32_bt(&a, &b, &c, &result);
 *
 * print_aitensor(&result);
 * \endcode
 *
 * @param *a        Q7 matrix a (2D tensor of shape [N x K])
 * @param *b        Q7 matrix b (2D tensor of shape [M x K])
 * @param *c        Q31 vector c (2D tensor of shape [1 x M] or 1D tensor of shape [M])
 * @param *result   Resulting Q7 matrix (2D tensor of shape [N x M])
 */
void aimath_q7_default_linear32_bt(const aitensor_t *a, const aitensor_t *b, const aitensor_t *c, aitensor_t *result);

/** @brief Performs a matrix multiplication of \link aimath_q7.h Q7 \endlink matrices a and b
  *
  * @f[
  *  result = a \cdot b
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
  * uint16_t b_shape[2] = {3, 2};
  * aimath_q7_params_t b_params = {2, 0}; // {shift, zero point}
  * int8_t b_data[3*2] = {4, 0,
  *                       0, 4,
  *                       0, 0};
  * aitensor_t b = AITENSOR_2D_Q7(b_shape, &b_params, b_data);
  *
  * uint16_t result_shape[2] = {3, 2};
  * aimath_q7_params_t result_params = {1, 0}; // {shift, zero point}
  * int8_t result_data[3*2];
  * aitensor_t result = AITENSOR_2D_Q7(result_shape, &result_params, result_data);
  *
  * aimath_q7_default_mat_mul(&a, &b, &result);
  *
  * print_aitensor(&result);
  * \endcode
  *
  * @param *a       Q7 matrix a (2D tensor of shape [N x K])
  * @param *b       Q7 matrix b (2D tensor of shape [K x M])
  * @param *result  Resulting Q7 matrix of the multiplication (2D tensor of shape [N x M])
  */
void aimath_q7_default_mat_mul(const aitensor_t *a, const aitensor_t *b, aitensor_t *result);

/** @brief Performs an element wise multiplication of \link aimath_q7.h Q7 \endlink tensors a and b (Hadamard product)
  *
  * @f[
  *  result = a \circ b
  * @f]
  *
  * Example:
  * \code{.c}
  * uint16_t a_shape[2] = {2, 3};
  * aimath_q7_params_t a_params = {1, 0}; // {shift, zero point}
  * int8_t a_data[2*3] = {  2,  -4,   6,
  *                        -8,  10, -12};
  * aitensor_t a = AITENSOR_2D_Q7(a_shape, &a_params, a_data);
  *
  * uint16_t b_shape[2] = {2, 3};
  * aimath_q7_params_t b_params = {2, 0}; // {shift, zero point}
  * int8_t b_data[2*3] = {  4,  -8,  12,
  *                       -16,  20, -24};
  * aitensor_t b = AITENSOR_2D_Q7(b_shape, &b_params, b_data);
  *
  * uint16_t result_shape[2] = {2, 3};
  * aimath_q7_params_t result_params = {1, 0}; // {shift, zero point}
  * int8_t result_data[2*3];
  * aitensor_t result = AITENSOR_2D_Q7(result_shape, &result_params, result_data);
  *
  * aimath_q7_default_multiply(&a, &b, &result);
  *
  * print_aitensor(&result);
  * \endcode
  *
  * @param *a       Q7 tensor a (N-D tensor)
  * @param *b       Q7 tensor b (N-D tensor)
  * @param *result  Resulting Q7 tensor of the element wise multiplication (N-D tensor)
  */
void aimath_q7_default_multiply(const aitensor_t *a, const aitensor_t *b, aitensor_t *result);

/** @brief Performs a scalar multiplication (scaling) of \link aimath_q7.h Q7 \endlink tensor a and a scalar
  *
  * @f[
  *  result = scalar \cdot a
  * @f]
  *
  * Example:
  * \code{.c}
  * uint16_t a_shape[2] = {2, 3};
  * aimath_q7_params_t a_params = {1, 0}; // {shift, zero point}
  * int8_t a_data[2*3] = {  2,  -4,   6,
  *                        -8,  10, -12};
  * aitensor_t a = AITENSOR_2D_Q7(a_shape, &a_params, a_data);
  *
  * aiscalar_q7_t scalar = AISCALAR_Q7(0.1f, 7, 0); // (value, shift, zero_point)
  *
  * uint16_t result_shape[2] = {2, 3};
  * aimath_q7_params_t result_params = {7, 0}; // {shift, zero point}
  * int8_t result_data[2*3];
  * aitensor_t result = AITENSOR_2D_Q7(result_shape, &result_params, result_data);
  *
  * aimath_q7_default_scalar_mul(&scalar, &a, &result);
  *
  * print_aitensor(&result);
  * \endcode
  *
  * @param *scalar  Scalar (type aiscalar_q7_t)
  * @param *a       Q7 tensor a (N-D tensor)
  * @param *result  Resulting Q7 tensor of the scalar multiplication (N-D tensor)
  */
void aimath_q7_default_scalar_mul(const void *scalar, const aitensor_t *a, aitensor_t *result);

/** @brief Performs an element wise addition of \link aimath_q7.h Q7 \endlink tensors a and b with different shifts
  *
  * @f[
  *  result = a + b
  * @f]
  *
  * The tensors a, b and result can have different shifts. The function will rescale the tensors
  * internally to perform the addition. If a, b and result have the same shift, use aimath_q7_default_tensor_add_same_shift()
  * instead because it is more efficient.
  *
  * Example:
  * \code{.c}
  * uint16_t a_shape[2] = {2, 3};
  * aimath_q7_params_t a_params = {1, 0}; // {shift, zero point}
  * int8_t a_data[2*3] = {  2,  -4,   6,
  *                        -8,  10, -12};
  * aitensor_t a = AITENSOR_2D_Q7(a_shape, &a_params, a_data);
  *
  * uint16_t b_shape[2] = {2, 3};
  * aimath_q7_params_t b_params = {2, 0}; // {shift, zero point}
  * int8_t b_data[2*3] = {  4,  -8,   12,
  *                        -16,  20, -24};
  * aitensor_t b = AITENSOR_2D_Q7(b_shape, &b_params, b_data);
  *
  * uint16_t result_shape[2] = {2, 3};
  * aimath_q7_params_t result_params = {0, 0}; // {shift, zero point}
  * int8_t result_data[2*3];
  * aitensor_t result = AITENSOR_2D_Q7(result_shape, &result_params, result_data);
  *
  * aimath_q7_default_tensor_add_different_shift(&a, &b, &result);
  *
  * print_aitensor(&result);
  * \endcode
  *
  * @param *a       Q7 tensor a (N-D tensor)
  * @param *b       Q7 tensor b (N-D tensor)
  * @param *result  Resulting Q7 tensor of the element wise addition (N-D tensor)
  */
void aimath_q7_default_tensor_add_different_shift(const aitensor_t *a, const aitensor_t *b, aitensor_t *result);

/** @brief Performs an element wise addition of \link aimath_q7.h Q7 \endlink tensors a and b with same shifts
  *
  * @f[
  *  result = a + b
  * @f]
  *
  * The tensors a, b and result must have the same shift.
  * If a, b and result have the different shifts, use aimath_q7_default_tensor_add_different_shift() instead.
  *
  * Example:
  * \code{.c}
  * uint16_t a_shape[2] = {2, 3};
  * aimath_q7_params_t a_params = {1, 0}; // {shift, zero point}
  * int8_t a_data[2*3] = {  2,  -4,   6,
  *                        -8,  10, -12};
  * aitensor_t a = AITENSOR_2D_Q7(a_shape, &a_params, a_data);
  *
  * uint16_t b_shape[2] = {2, 3};
  * aimath_q7_params_t b_params = {1, 0}; // {shift, zero point}
  * int8_t b_data[2*3] = {  2,  -4,   6,
  *                        -8,  10, -12};
  * aitensor_t b = AITENSOR_2D_Q7(b_shape, &b_params, b_data);
  *
  * uint16_t result_shape[2] = {2, 3};
  * aimath_q7_params_t result_params = {1, 0}; // {shift, zero point}
  * int8_t result_data[2*3];
  * aitensor_t result = AITENSOR_2D_Q7(result_shape, &result_params, result_data);
  *
  * aimath_q7_default_tensor_add_same_shift(&a, &b, &result);
  *
  * print_aitensor(&result);
  * \endcode
  *
  * @param *a       Q7 tensor a (N-D tensor)
  * @param *b       Q7 tensor b (N-D tensor)
  * @param *result  Resulting Q7 tensor of the element wise addition (N-D tensor)
  */
void aimath_q7_default_tensor_add_same_shift(const aitensor_t *a, const aitensor_t *b, aitensor_t *result);

/** @brief Performs an element wise addition of \link aimath_q7.h Q7 \endlink tensors a and b with different shifts
  *
  * @f[
  *  result = a - b
  * @f]
  *
  * The tensors a, b and result can have different shifts. The function will rescale the tensors
  * internally to perform the subtraction. If a, b and result have the same shift, use aimath_q7_default_tensor_sub_same_shift()
  * instead because it is more efficient.
  *
  * Example:
  * \code{.c}
  * uint16_t a_shape[2] = {2, 3};
  * aimath_q7_params_t a_params = {1, 0}; // {shift, zero point}
  * int8_t a_data[2*3] = {  2,  -4,   6,
  *                        -8,  10, -12};
  * aitensor_t a = AITENSOR_2D_Q7(a_shape, &a_params, a_data);
  *
  * uint16_t b_shape[2] = {2, 3};
  * aimath_q7_params_t b_params = {2, 0}; // {shift, zero point}
  * int8_t b_data[2*3] = {  4,  8, 12,
  *                        16, 20, 24};
  * aitensor_t b = AITENSOR_2D_Q7(b_shape, &b_params, b_data);
  *
  * uint16_t result_shape[2] = {2, 3};
  * aimath_q7_params_t result_params = {0, 0}; // {shift, zero point}
  * int8_t result_data[2*3];
  * aitensor_t result = AITENSOR_2D_Q7(result_shape, &result_params, result_data);
  *
  * aimath_q7_default_tensor_sub_different_shift(&a, &b, &result);
  *
  * print_aitensor(&result);
  * \endcode
  *
  * @param *a       Q7 tensor a (N-D tensor)
  * @param *b       Q7 tensor b (N-D tensor)
  * @param *result  Resulting Q7 tensor of the element wise subtraction (N-D tensor)
  */
void aimath_q7_default_tensor_sub_different_shift(const aitensor_t *a, const aitensor_t *b, aitensor_t *result);

/** @brief Performs an element wise subtraction of \link aimath_q7.h Q7 \endlink tensors a and b with same shifts
  *
  * @f[
  *  result = a - b
  * @f]
  *
  * The tensors a, b and result must have the same shift.
  * If a, b and result have the different shifts, use aimath_q7_default_tensor_sub_different_shift() instead.
  *
  * Example:
  * \code{.c}
  * uint16_t a_shape[2] = {2, 3};
  * aimath_q7_params_t a_params = {1, 0}; // {shift, zero point}
  * int8_t a_data[2*3] = {  2,  -4,   6,
  *                        -8,  10, -12};
  * aitensor_t a = AITENSOR_2D_Q7(a_shape, &a_params, a_data);
  *
  * uint16_t b_shape[2] = {2, 3};
  * aimath_q7_params_t b_params = {1, 0}; // {shift, zero point}
  * int8_t b_data[2*3] = { 2,  4,  6,
  *                        8, 10, 12};
  * aitensor_t b = AITENSOR_2D_Q7(b_shape, &b_params, b_data);
  *
  * uint16_t result_shape[2] = {2, 3};
  * aimath_q7_params_t result_params = {1, 0}; // {shift, zero point}
  * int8_t result_data[2*3];
  * aitensor_t result = AITENSOR_2D_Q7(result_shape, &result_params, result_data);
  *
  * aimath_q7_default_tensor_sub_same_shift(&a, &b, &result);
  *
  * print_aitensor(&result);
  * \endcode
  *
  * @param *a       Q7 tensor a (N-D tensor)
  * @param *b       Q7 tensor b (N-D tensor)
  * @param *result  Resulting Q7 tensor of the element wise subtraction (N-D tensor)
  */
void aimath_q7_default_tensor_sub_same_shift(const aitensor_t *a, const aitensor_t *b, aitensor_t *result);

/** @brief Performs an element wise copy of \link aimath_q7.h Q7 \endlink tensors
  *
  * @f[
  *  to \leftarrow from
  * @f]
  *
  * Dimension, shape and quantization parameters of from and to tensors have to be the same.
  *
  * Example:
  * \code{.c}
  * uint16_t from_shape[2] = {2, 3};
  * aimath_q7_params_t from_params = {1, 0}; // {shift, zero point}
  * int8_t from_data[2*3] = {  2,  -4,   6,
  *                           -8,  10, -12};
  * aitensor_t from = AITENSOR_2D_Q7(from_shape, &from_params, from_data);
  *
  * uint16_t to_shape[2] = {2, 3};
  * aimath_q7_params_t to_params = {1, 0}; // {shift, zero point}
  * int8_t to_data[2*3];
  * aitensor_t to = AITENSOR_2D_Q7(to_shape, &to_params, to_data);
  *
  * aimath_q7_default_copy_tensor(&from, &to);
  *
  * print_aitensor(&to);
  * \endcode
  *
  * @param *from    Q7 tensor to copy from (N-D tensor)
  * @param *to      Q7 tensor to copy to (N-D tensor)
  */
void aimath_q7_default_copy_tensor(const aitensor_t *from, aitensor_t *to);

/** @brief Transposes a \link aimath_q7.h Q7 \endlink vector
  *
  * The given tensor must be a vector (2D tensor of shape [1 x N] or [N x 1]).
  *
  * @f[
  *  vector \leftarrow vector^T
  * @f]
  *
  * Example:
  * \code{.c}
  * uint16_t vector_shape[2] = {1, 3};
  * aimath_q7_params_t vector_params = {1, 0}; // {shift, zero point}
  * int8_t vector_data[2*3] = {  2,  -4,   6};
  * aitensor_t vector = AITENSOR_2D_Q7(vector_shape, &vector_params, vector_data);
  *
  * aimath_q7_default_transpose_vector(&vector);
  *
  * print_aitensor(&vector);
  * \endcode
  *
  * @param *vector Q7 vector (2D tensor of shape [1 x N] or [N x 1])
  */
void aimath_q7_default_transpose_vector(aitensor_t *vector);

/** @brief Transpose a \link aimath_q7.h Q7 \endlink tensor
  *
  * @f[
  *  x \leftarrow x^T
  * @f]
  *
  * Example:
  * \code{.c}
  * uint16_t x_shape[2] = {2, 3};
  * aimath_q7_params_t x_params = {1, 0}; // {shift, zero point}
  * int8_t x_data[2*3] = {  2,  -4,   6,
  *                        -8,  10, -12};
  * aitensor_t x = AITENSOR_2D_Q7(x_shape, &x_params, x_data);
  *
  * aimath_q7_default_transpose_matrix(&x);
  *
  * print_aitensor(&x);
  * \endcode
  *
  * @param *x Q7 tensor to be transposed (2D tensor)
  */
void aimath_q7_default_transpose_matrix(aitensor_t *x);

/** @brief Calculates the sigmoid of each element in a \link aimath_q7.h Q7 \endlink tensor
  *
  * @f[
  *   result_{i} = \sigma(x_{i}) = \frac{1}{1 + e^{-x_{i}}}
  * @f]
  *
  * The sigmoid is calculated with a piecewise linear approximation (PLAN) to avoid using exponential functions.
  *
  * @f[
  *   result_{i} = \sigma_{PLAN}(x_i) = \begin{cases}
                1 & \text{if } 5 \leq x_i\\
                0.03125 \cdot |x_i| + 0.84375 & \text{if } 2.375 \leq x_i < 5\\
                0.0125 \cdot |x_i| + 0.625 & \text{if } 1 \leq x_i < 2.375\\
                0.25 \cdot |x_i| + 0.5 & \text{if } 0 \leq x_i < 1\\
                1 - \sigma_{PLAN}(- x_i) & \text{if } x_i < 0\\
                \end{cases}
  * @f]
  *
  * **The quantization parameters of the result tensor are set to {shift = 8, zero_point = -2^7} by the function
  * because the output values are in the interval (0, 1).**
  *
  * Example:
  * \code{.c}
  * uint16_t x_shape[2] = {2, 3};
  * aimath_q7_params_t x_params = {1, 0}; // {shift, zero point}
  * int8_t x_data[2*3] = {  2,  -4,   6,
  *                        -8,  10, -12};
  * aitensor_t x = AITENSOR_2D_Q7(x_shape, &x_params, x_data);
  *
  * uint16_t result_shape[2] = {2, 3};
  * aimath_q7_params_t result_params;
  * int8_t result_data[2*3];
  * aitensor_t result = AITENSOR_2D_Q7(result_shape, &result_params, result_data);
  *
  * aimath_q7_default_sigmoid(&x, &result);
  *
  * print_aitensor(&result);
  * \endcode
  *
  * @see Sigmoid PLAN: https://www.researchgate.net/figure/Comparative-representation-of-the-sigmoid-function-and-PLAN-approximation_fig7_228618304
  *
  * @param *x       Q7 tensor to calculate the sigmoid from (N-D tensor)
  * @param *result  Resulting Q7 tensor (N-D tensor)
  */
void aimath_q7_default_sigmoid(const aitensor_t *x, aitensor_t *result);

/** @brief Calculates the rectifier (ReLU) value of each element in a \link aimath_q7.h Q7 \endlink tensor
  *
  * @f[
  *  result_{i} = max(0, x_{i})
  * @f]
  *
  * **The quantization parameters of the result tensor are set to {shift = x.shift, zero_point = x.zero_point} by the function
  * because the output values are in the interval [0, max(x)].**
  *
  * Example:
  * \code{.c}
  * uint16_t x_shape[2] = {2, 3};
  * aimath_q7_params_t x_params = {1, 0}; // {shift, zero point}
  * int8_t x_data[2*3] = {  2,  -4,   6,
  *                        -8,  10, -12};
  * aitensor_t x = AITENSOR_2D_Q7(x_shape, &x_params, x_data);
  *
  * uint16_t result_shape[2] = {2, 3};
  * aimath_q7_params_t result_params;
  * int8_t result_data[2*3];
  * aitensor_t result = AITENSOR_2D_Q7(result_shape, &result_params, result_data);
  *
  * aimath_q7_default_relu(&x, &result);
  *
  * print_aitensor(&result);
  * \endcode
  *
  * @param *x       Q7 tensor to calculate the ReLU from (N-D tensor)
  * @param *result  Resulting Q7 tensor (N-D tensor)
  */
void aimath_q7_default_relu(const aitensor_t *x, aitensor_t *result);

/** @brief Calculates the rectifier (ReLU) derivative of each element in a \link aimath_q7.h Q7 \endlink tensor
  *
  * @f[
  *  result_{ij} = \begin{cases}
                    0 & \text{if } x_i < 0\\
                    1 & \text{if } x_i \geq 0
                    \end{cases}
  * @f]
  *
  * **The quantization parameters of the result tensor are set to {shift = 0, zero_point = 0} by the function
  * because the output values are either 0 or 1.**
  *
  * Example:
  * \code{.c}
  * uint16_t x_shape[2] = {2, 3};
  * aimath_q7_params_t x_params = {1, 0}; // {shift, zero point}
  * int8_t x_data[2*3] = {  2,  -4,   6,
  *                        -8,  10, -12};
  * aitensor_t x = AITENSOR_2D_Q7(x_shape, &x_params, x_data);
  *
  * uint16_t result_shape[2] = {2, 3};
  * aimath_q7_params_t result_params; // {shift, zero point}
  * int8_t result_data[2*3];
  * aitensor_t result = AITENSOR_2D_Q7(result_shape, &result_params, result_data);
  *
  * aimath_q7_default_d_relu(&x, &result);
  *
  * print_aitensor(&result);
  * \endcode
  *
  * @param *x       Q7 tensor to calculate the ReLU derivative from (N-D tensor)
  * @param *result  Resulting Q7 tensor (N-D tensor)
  */
void aimath_q7_default_d_relu(const aitensor_t *x, aitensor_t *result);

/** @brief Calculates the leaky rectifier (leaky ReLU) value of each element in a \link aimath_q7.h Q7 \endlink tensor
 *
 * @f[
 *  result_{i} = \begin{cases}
        \alpha \cdot x_i & \text{if } x_i < 0 \\
          x_i & \text{if } x_i \geq 0
     \end{cases}
 * @f]
 *
 * **The quantization parameters of the result tensor are set to {shift = x.shift, zero_point = x.zero_point} by the function
 * because the output values are in the interval (alpha * min(x), max(x)].**
 *
 * Example:
 * \code{.c}
 * uint16_t x_shape[2] = {2, 3};
 * aimath_q7_params_t x_params = {6, 0}; // {shift, zero point}
 * int8_t x_data[2*3] = {  2,  -4,   6,
 *                        -8,  10, -12};
 * aitensor_t x = AITENSOR_2D_Q7(x_shape, &x_params, x_data);
 *
 * aiscalar_q7_t alpha = AISCALAR_Q7(0.01f, 10, 0);
 *
 * uint16_t result_shape[2] = {2, 3};
 * aimath_q7_params_t result_params; // {shift, zero point}
 * int8_t result_data[2*3];
 * aitensor_t result = AITENSOR_2D_Q7(result_shape, &result_params, result_data);
 *
 * aimath_q7_default_leaky_relu(&x, &alpha, &result);
 *
 * print_aitensor(&result);
 * \endcode
 *
 * @param *x        Q7 tensor to calculate the leaky ReLU from (N-D tensor)
 * @param *alpha    Scalar \f$ \alpha \f$ (type aiscalar_q7_t) for the leakage
 * @param *result   Resulting Q7 tensor (N-D tensor)
 */
void aimath_q7_default_leaky_relu(const aitensor_t *x, const void *alpha, aitensor_t *result);

/** @brief Calculates the exponential rectifier (ELU) value of each element in a \link aimath_q7.h Q7 \endlink tensor
 *
 * @f[
 *  result_{i} = \begin{cases}
        \alpha \cdot (e^{x_i} - 1) & \text{if } x_i < 0 \\
        x_i & \text{if } x_i \geq 0
    \end{cases}
 * @f]
 *
 * The ELU is calculated with a piecewise linear approximation to avoid using exponential functions.
 *
 * @f[
 *  result_{i} = \begin{cases}
                 x_i & \text{if } 0 \leq x_i\\
                 \alpha \cdot 0.625 \cdot x_i & \text{if } -1 \leq x < 0\\
                 \alpha \cdot (0.25 \cdot x_i - 0.375) & \text{if } -2 \leq x < -1\\
                 \alpha \cdot (0.09375 \cdot x_i - 0.6875) & \text{if } -3 \leq x < -2\\
                 \alpha \cdot (0.03125 \cdot x_i - 0.875) & \text{if } -4 \leq x < -3\\
                 - \alpha & \text{if } x < -4
                 \end{cases}
 * @f]
 *
 * **The quantization parameters of the result tensor are set to {shift = x.shift, zero_point = x.zero_point} by the function
 * because the output values are in the interval (max(-alpha, min(x)), max(x)].**
 *
 * Example:
 * \code{.c}
 * uint16_t x_shape[2] = {2, 3};
 * aimath_q7_params_t x_params = {1, 0}; // {shift, zero point}
 * int8_t x_data[2*3] = {  2,  -4,   6,
 *                        -8,  10, -12};
 * aitensor_t x = AITENSOR_2D_Q7(x_shape, &x_params, x_data);
 *
 * aiscalar_q7_t alpha = AISCALAR_Q7(1.0f, 0, 0);
 *
 * uint16_t result_shape[2] = {2, 3};
 * aimath_q7_params_t result_params; // {shift, zero point}
 * int8_t result_data[2*3];
 * aitensor_t result = AITENSOR_2D_Q7(result_shape, &result_params, result_data);
 *
 * aimath_q7_default_elu(&x, &alpha, &result);
 *
 * print_aitensor(&result);
 * \endcode
 *
 * @param *x        Q7 tensor to calculate the ELU from (N-D tensor)
 * @param *alpha    Scalar \f$ \alpha \f$ (type aiscalar_q7_t)
 * @param *result   Resulting Q7 tensor (N-D tensor)
 */
void aimath_q7_default_elu(const aitensor_t *x, const void *alpha, aitensor_t *result);

/** @brief Calculates the tanh of each element in a \link aimath_q7.h Q7 \endlink tensor
 *
 * @f[
 *   result_{i} = \tanh(x_{i}) = \frac{e^{x_i} - e^{-x_i}}{e^{x_i} + e^{-x_i}}
 * @f]
 *
 * The tanh is calculated with a piecewise linear approximation (PLA) to avoid using exponential functions.
 *
 * @f[
 *   result_{i} = \tanh_{PLA}(x_i) = 2 \cdot \sigma(2x_i) - 1 = \begin{cases}
               1 & \text{if } 5 \leq x_i\\
               0.0625 \cdot |x_i| + 0.6875 & \text{if } 2.375 \leq x_i < 5\\
               0.25 \cdot |x_i| + 0.25 & \text{if } 1 \leq x_i < 2.375\\
               0.5 \cdot |x_i| & \text{if } 0 \leq x_i < 1\\
               - \tanh_{PLA}(- x_i) & \text{if } x_i < 0\\
               \end{cases}
 * @f]
 *
 * **The quantization parameters of the result tensor are set to {shift = 7, zero_point = 0} by the function
 * because the output values are in the interval (-1, 1).**
 *
 * Example:
 * \code{.c}
 * uint16_t x_shape[2] = {2, 3};
 * aimath_q7_params_t x_params = {1, 0}; // {shift, zero point}
 * int8_t x_data[2*3] = {  2,  -4,   6,
 *                        -8,  10, -12};
 * aitensor_t x = AITENSOR_2D_Q7(x_shape, &x_params, x_data);
 *
 * uint16_t result_shape[2] = {2, 3};
 * aimath_q7_params_t result_params;
 * int8_t result_data[2*3];
 * aitensor_t result = AITENSOR_2D_Q7(result_shape, &result_params, result_data);
 *
 * aimath_q7_default_tanh(&x, &result);
 *
 * print_aitensor(&result);
 * \endcode
 *
 * @see Sigmoid PLAN: https://www.researchgate.net/figure/Comparative-representation-of-the-sigmoid-function-and-PLAN-approximation_fig7_228618304
 *
 * @param *x        Q7 tensor to calculate the tanh from (N-D tensor)
 * @param *result   Resulting Q7 tensor (N-D tensor)
 */
void aimath_q7_default_tanh(const aitensor_t *x, aitensor_t *result);

/** @brief Calculates the softsign value of each element in a \link aimath_q7.h Q7 \endlink tensor
 *
 * @f[
 *  result_{i} = \frac {x_i} {1 + |x_i|}
 * @f]
 *
 * **The quantization parameters of the result tensor are set to {shift = 7, zero_point = 0} by the function
 * because the output values are in the interval (-1, 1).**
 *
 * Example:
 * \code{.c}
 * uint16_t x_shape[2] = {2, 3};
 * aimath_q7_params_t x_params = {1, 0}; // {shift, zero point}
 * int8_t x_data[2*3] = {  2,  -4,   6,
 *                        -8,  10, -12};
 * aitensor_t x = AITENSOR_2D_Q7(x_shape, &x_params, x_data);
 *
 * uint16_t result_shape[2] = {2, 3};
 * aimath_q7_params_t result_params;
 * int8_t result_data[2*3];
 * aitensor_t result = AITENSOR_2D_Q7(result_shape, &result_params, result_data);
 *
 * aimath_q7_default_softsign(&x, &result);
 *
 * print_aitensor(&result);
 * \endcode
 *
 * @param *x       Q7 tensor to calculate the softsign from (N-D tensor)
 * @param *result  Resulting Q7 tensor (N-D tensor)
 */
void aimath_q7_default_softsign(const aitensor_t *x, aitensor_t *result);

/** @brief Calculates the softmax value of each batch element (row) of a \link aimath_q7.h Q7 \endlink tensor
 *
 * @f[
 *  result_{i} = \frac{e^{x_i}}{\sum_{j=1}^{K} e^{x_j}}
 * @f]
 *
 * **The quantization parameters of the result tensor are set to {shift = 8, zero_point = -128} by the function
 * because the output values are in the interval (0, 1).**
 *
 * Example:
 * \code{.c}
 * uint16_t x_shape[2] = {2, 3};
 * aimath_q7_params_t x_params = {1, 0}; // {shift, zero point}
 * int8_t x_data[2*3] = {  2,  -4,   6,
 *                        -8,  10, -12};
 * aitensor_t x = AITENSOR_2D_Q7(x_shape, &x_params, x_data);
 *
 * uint16_t result_shape[2] = {2, 3};
 * aimath_q7_params_t result_params;
 * int8_t result_data[2*3];
 * aitensor_t result = AITENSOR_2D_Q7(result_shape, &result_params, result_data);
 *
 * aimath_q7_default_softmax(&x, &result);
 *
 * print_aitensor(&result);
 * \endcode
 *
 * @param *x       Q7 tensor to calculate the softmax from (N-D tensor)
 * @param *result  Resulting Q7 tensor (N-D tensor)
 */
void aimath_q7_default_softmax(const aitensor_t *x, aitensor_t *result);

/** @brief Fills a \link aimath_q7.h Q7 \endlink tensor with zeros
  *
  * @f[
  *  tensor_{i} = 0
  * @f]
  *
  * The function sets all tensor elements to the zero_point given in the tensor parameters.
  *
  * Example:
  * \code{.c}
  * uint16_t tensor_shape[2] = {2, 3};
  * aimath_q7_params_t tensor_params = {1, 0}; // {shift, zero point}
  * int8_t tensor_data[2*3] = {  2,  -4,   6,
  *                             -8,  10, -12};
  * aitensor_t tensor = AITENSOR_2D_Q7(tensor_shape, &tensor_params, tensor_data);
  *
  * aimath_q7_default_zero_tensor(&tensor);
  *
  * print_aitensor(&tensor);
  * \endcode
  *
  * @param *tensor Q7 tensor to set to zero (N-D tensor)
  */
void aimath_q7_default_zero_tensor(aitensor_t *tensor);

/** @brief Fills a \link aimath_q7.h Q7 \endlink tensor with zeros
  *
  * @f[
  *  tensor_{i} = 0
  * @f]
  *
  * The function sets all tensor elements, the shift and the zero_point to 0.
  *
  * Example:
  * \code{.c}
  * uint16_t tensor_shape[2] = {2, 3};
  * aimath_q7_params_t tensor_params;
  * int8_t tensor_data[2*3];
  * aitensor_t tensor = AITENSOR_2D_Q7(tensor_shape, &tensor_params, tensor_data);
  *
  * aimath_q7_default_init_zeros(&tensor);
  *
  * print_aitensor(&tensor);
  * \endcode
  *
  * @param *tensor Q7 tensor to set to zero (N-D tensor)
  */
void aimath_q7_default_init_zeros(aitensor_t *tensor);


#endif // AIMATH_Q7_DEFAULT
