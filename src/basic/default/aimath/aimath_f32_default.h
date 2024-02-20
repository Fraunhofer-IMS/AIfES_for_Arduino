/**
 * \file basic/default/aimath/aimath_f32_default.h
 * \internal
 * \date 25.10.2020
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
 * \brief Math functions for \link aimath_f32.h F32 \endlink data type, default implementation
 *
 * These functions can be used when no hardware specific implementation is available.
 */

#ifndef AIMATH_F32_DEFAULT
#define AIMATH_F32_DEFAULT

#include <stdint.h>
#include <math.h>
#include <stdlib.h>

#include "basic/base/aimath/aimath_f32.h"

/** @brief Performs a matrix multiplication of \link aimath_f32.h F32 \endlink matrices a and b and adds a vector c to each row
 *
 * The addition of the horizontal vector c is performed via broadcast, i.e. element wise in each column
 * Mathematically this broadcast is equal to multiplying c with an vertical vector (with the same number of elements as c)
 * and adding the result to \f$ a \cdot b \f$.
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
 *  result = a \cdot b + \left( \begin{array}{r} 1 \\
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
 *   = \left( \begin{array}{rr} 3 & 7  \\
				6 & 10  \\
				9 & 13
				\end{array}\right)
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
 * float b_data[3*2] = {1.0f, 0.0f,
 *                      0.0f, 1.0f,
 *                      0.0f, 0.0f};
 * aitensor_t b = AITENSOR_2D_F32(b_shape, b_data);
 *
 * uint16_t c_shape[2] = {1, 2};
 * float c_data[1*2] = {2.0f, 5.0f};
 * aitensor_t c = AITENSOR_2D_F32(c_shape, c_data);
 *
 * uint16_t result_shape[2] = {3, 2};
 * float result_data[3*2];
 * aitensor_t result = AITENSOR_2D_F32(result_shape, result_data);
 *
 * aimath_f32_default_linear(&a, &b, &c, &result);
 *
 * print_aitensor(&result);
 * \endcode
 *
 * @param *a        F32 matrix a (2D tensor of shape [N x K])
 * @param *b        F32 matrix b (2D tensor of shape [K x M])
 * @param *c        F32 vector c (2D tensor of shape [1 x M] or 1D tensor of shape [M])
 * @param *result   Resulting F32 matrix (2D tensor of shape [N x M])
 */
void aimath_f32_default_linear(const aitensor_t *a, const aitensor_t *b, const aitensor_t *c, aitensor_t *result);

/** @brief Performs a matrix multiplication of \link aimath_f32.h F32 \endlink matrices a (transposed) and b and adds a vector c to each row
 *
 * Same operation as aimath_f32_default_linear() but with a transposed a matrix.
 *
 * The addition of the horizontal vector c is performed via broadcast, i.e. element wise in each column
 * Mathematically this broadcast is equal to multiplying c with an vertical vector (with the same number of elements as c)
 * and adding the result to \f$ a^T \cdot b \f$.
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
 * \code{.c}
 * uint16_t a_shape[2] = {3, 3};
 * float a_data[3*3] = {1.0f, 2.0f, 3.0f,
 *                      4.0f, 5.0f, 6.0f,
 *                      7.0f, 8.0f, 9.0f};
 * aitensor_t a = AITENSOR_2D_F32(a_shape, a_data);
 *
 * uint16_t b_shape[2] = {3, 2};
 * float b_data[3*2] = {1.0f, 0.0f,
 *                      0.0f, 1.0f,
 *                      0.0f, 0.0f};
 * aitensor_t b = AITENSOR_2D_F32(b_shape, b_data);
 *
 * uint16_t c_shape[2] = {1, 2};
 * float c_data[1*2] = {2.0f, 5.0f};
 * aitensor_t c = AITENSOR_2D_F32(c_shape, c_data);
 *
 * uint16_t result_shape[2] = {3, 2};
 * float result_data[3*2];
 * aitensor_t result = AITENSOR_2D_F32(result_shape, result_data);
 *
 * aimath_f32_default_linear_at(&a, &b, &c, &result);
 *
 * print_aitensor(&result);
 * \endcode
 *
 * @param *a        F32 matrix a (2D tensor of shape [K x N])
 * @param *b        F32 matrix b (2D tensor of shape [K x M])
 * @param *c        F32 vector c (2D tensor of shape [1 x M] or 1D tensor of shape [M])
 * @param *result   Resulting F32 matrix (2D tensor of shape [N x M])
 */
void aimath_f32_default_linear_at(const aitensor_t *a, const aitensor_t *b, const aitensor_t *c, aitensor_t *result);

/** @brief Performs a matrix multiplication of \link aimath_f32.h F32 \endlink matrices a and b (transposed) and adds a vector c to each row
 *
 * Same operation as aimath_f32_default_linear() but with a transposed b matrix.
 *
 * The addition of the horizontal vector c is performed via broadcast, i.e. element wise in each column
 * Mathematically this broadcast is equal to multiplying c with an vertical vector (with the same number of elements as c)
 * and adding the result to \f$ a \cdot b^T \f$.
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
 * \code{.c}
 * uint16_t a_shape[2] = {3, 3};
 * float a_data[3*3] = {1.0f, 2.0f, 3.0f,
 *                      4.0f, 5.0f, 6.0f,
 *                      7.0f, 8.0f, 9.0f};
 * aitensor_t a = AITENSOR_2D_F32(a_shape, a_data);
 *
 * uint16_t b_shape[2] = {2, 3};
 * float b_data[2*3] = {1.0f, 0.0f, 0.0f,
 *                      0.0f, 1.0f, 0.0f};
 * aitensor_t b = AITENSOR_2D_F32(b_shape, b_data);
 *
 * uint16_t c_shape[2] = {1, 2};
 * float c_data[1*2] = {2.0f, 5.0f};
 * aitensor_t c = AITENSOR_2D_F32(c_shape, c_data);
 *
 * uint16_t result_shape[2] = {3, 2};
 * float result_data[3*2];
 * aitensor_t result = AITENSOR_2D_F32(result_shape, result_data);
 *
 * aimath_f32_default_linear_bt(&a, &b, &c, &result);
 *
 * print_aitensor(&result);
 * \endcode
 *
 * @param *a        F32 matrix a (2D tensor of shape [N x K])
 * @param *b        F32 matrix b (2D tensor of shape [M x K])
 * @param *c        F32 vector c (2D tensor of shape [1 x M] or 1D tensor of shape [M])
 * @param *result   Resulting F32 matrix (2D tensor of shape [N x M])
 */
void aimath_f32_default_linear_bt(const aitensor_t *a, const aitensor_t *b, const aitensor_t *c, aitensor_t *result);

/** @brief Performs a matrix multiplication with transposed result of \link aimath_f32.h F32 \endlink matrices a (transposed) and b and adds a vector c to each row
 *
 * Same operation as aimath_f32_default_linear() but with a transposed a matrix and transposed result.
 *
 * The addition of the horizontal vector c is performed via broadcast, i.e. element wise in each column
 * Mathematically this broadcast is equal to multiplying c with an vertical vector (with the same number of elements as c)
 * and adding the result to \f$ a^T \cdot b \f$.
 *
 * @f[
 *  result = \left( a \cdot b^T + \left( \begin{array}{c}
 							1  \\
							1 \\
							\vdots \\
							1  \\
							\end{array}\right)  \cdot c \right)^T
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
 * float b_data[3*2] = {1.0f, 0.0f,
 *                      0.0f, 1.0f,
 *                      0.0f, 0.0f};
 * aitensor_t b = AITENSOR_2D_F32(b_shape, b_data);
 *
 * uint16_t c_shape[2] = {1, 2};
 * float c_data[1*2] = {2.0f, 5.0f};
 * aitensor_t c = AITENSOR_2D_F32(c_shape, c_data);
 *
 * uint16_t result_shape[2] = {2, 3};
 * float result_data[2*3];
 * aitensor_t result = AITENSOR_2D_F32(result_shape, result_data);
 *
 * aimath_f32_default_linear_atrt(&a, &b, &c, &result);
 *
 * print_aitensor(&result);
 * \endcode
 *
 * @param *a        F32 matrix a (2D tensor of shape [K x N])
 * @param *b        F32 matrix b (2D tensor of shape [K x M])
 * @param *c        F32 vector c (2D tensor of shape [1 x M] or 1D tensor of shape [M])
 * @param *result   Resulting F32 matrix (2D tensor of shape [M x N])
 */
void aimath_f32_default_linear_atrt(const aitensor_t *a, const aitensor_t *b, const aitensor_t *c, aitensor_t *result);

/** @brief Performs a matrix multiplication of \link aimath_f32.h F32 \endlink matrices a and b
 *
 * @f[
 *  result = a \cdot b
 * @f]
 *
 * Example:
 * \code{.c}
 * uint16_t a_shape[2] = {2, 3};
 * float a_data[2*3] = {1.0f, 2.0f, 3.0f,
 *                      4.0f, 5.0f, 6.0f};
 * aitensor_t a = AITENSOR_2D_F32(a_shape, a_data);
 *
 * uint16_t b_shape[2] = {3, 2};
 * float b_data[3*2] = {1.0f, 0.0f,
 *                      0.0f, 1.0f,
 *                      0.0f, 0.0f};
 * aitensor_t b = AITENSOR_2D_F32(b_shape, b_data);
 *
 * uint16_t result_shape[2] = {2, 2};
 * float result_data[2*2];
 * aitensor_t result = AITENSOR_2D_F32(result_shape, result_data);
 *
 * aimath_f32_default_mat_mul(&a, &b, &result);
 *
 * print_aitensor(&result);
 * \endcode
 *
 * @param *a       F32 matrix a (2D tensor of shape [N x K])
 * @param *b       F32 matrix b (2D tensor of shape [K x M])
 * @param *result  Resulting F32 matrix of the multiplication (2D tensor of shape [N x M])
 */
void aimath_f32_default_mat_mul(const aitensor_t *a, const aitensor_t *b, aitensor_t *result);

/** @brief Performs a matrix multiplication of \link aimath_f32.h F32 \endlink matrices a (transposed) and b
  *
  * Same operation as aimath_f32_default_mat_mul() but with a transposed a matrix.
  *
  * @f[
  *  result = a^T \cdot b
  * @f]
  *
  * Example:
  * \code{.c}
  * uint16_t a_shape[2] = {3, 2};
  * float a_data[3*2] = {1.0f, 2.0f,
  *                      4.0f, 5.0f,
  *                      7.0f, 8.0f,};
  * aitensor_t a = AITENSOR_2D_F32(a_shape, a_data);
 *
 * uint16_t b_shape[2] = {3, 2};
 * float b_data[3*2] = {1.0f, 0.0f,
 *                      0.0f, 1.0f,
 *                      0.0f, 0.0f};
  * aitensor_t b = AITENSOR_2D_F32(b_shape, b_data);
  *
  * uint16_t result_shape[2] = {2, 2};
  * float result_data[2*2];
  * aitensor_t result = AITENSOR_2D_F32(result_shape, result_data);
  *
  * aimath_f32_default_mat_mul_at(&a, &b, &result);
  *
  * print_aitensor(&result);
  * \endcode
  *
  * @param *a       F32 matrix a (2D tensor of shape [K x N])
  * @param *b       F32 matrix b (2D tensor of shape [K x M])
  * @param *result  Resulting F32 matrix of the multiplication (2D tensor of shape [N x M])
  */
void aimath_f32_default_mat_mul_at(const aitensor_t *a, const aitensor_t *b, aitensor_t *result);

/** @brief Performs a matrix multiplication of \link aimath_f32.h F32 \endlink matrices a and b (transposed)
  *
  * Same operation as aimath_f32_default_mat_mul() but with a transposed b matrix.
  *
  * @f[
  *  result = a \cdot b^T
  * @f]
  *
  * Example:
  * \code{.c}
  * uint16_t a_shape[2] = {2, 3};
  * float a_data[2*3] = {1.0f, 2.0f, 3.0f,
  *                      4.0f, 5.0f, 6.0f};
  * aitensor_t a = AITENSOR_2D_F32(a_shape, a_data);
  *
  * uint16_t b_shape[2] = {2, 3};
  * float b_data[2*3] = {1.0f, 0.0f, 0.0f,
  *                      0.0f, 1.0f, 0.0f};
  * aitensor_t b = AITENSOR_2D_F32(b_shape, b_data);
  *
  * uint16_t result_shape[2] = {2, 2};
  * float result_data[2*2];
  * aitensor_t result = AITENSOR_2D_F32(result_shape, result_data);
  *
  * aimath_f32_default_mat_mul_bt(&a, &b, &result);
  *
  * print_aitensor(&result);
  * \endcode
  *
  * @param *a       F32 matrix a (2D tensor of shape [N x K])
  * @param *b       F32 matrix b (2D tensor of shape [M x K])
  * @param *result  Resulting F32 matrix of the multiplication (2D tensor of shape [N x M])
  */
void aimath_f32_default_mat_mul_bt(const aitensor_t *a, const aitensor_t *b, aitensor_t *result);

/** @brief Performs a matrix multiplication with transposed result of \link aimath_f32.h F32 \endlink matrices a (transposed) and b
  *
  * Same operation as aimath_f32_default_mat_mul() but with a transposed a matrix.
  *
  * @f[
  *  result = a^T \cdot b
  * @f]
  *
  * Example:
  * \code{.c}
  * uint16_t a_shape[2] = {3, 2};
  * float a_data[3*2] = {1.0f, 2.0f,
  *                      4.0f, 5.0f,
  *                      7.0f, 8.0f,};
  * aitensor_t a = AITENSOR_2D_F32(a_shape, a_data);
 *
 * uint16_t b_shape[2] = {3, 1};
 * float b_data[3*2] = {1.0f,
 *                      0.0f,
 *                      0.0f};
  * aitensor_t b = AITENSOR_2D_F32(b_shape, b_data);
  *
  * uint16_t result_shape[2] = {1, 2};
  * float result_data[1*2];
  * aitensor_t result = AITENSOR_2D_F32(result_shape, result_data);
  *
  * aimath_f32_default_mat_mul_atrt(&a, &b, &result);
  *
  * print_aitensor(&result);
  * \endcode
  *
  * @param *a       F32 matrix a (2D tensor of shape [K x N])
  * @param *b       F32 matrix b (2D tensor of shape [K x M])
  * @param *result  Resulting F32 matrix of the multiplication (2D tensor of shape [M x N])
  */
void aimath_f32_default_mat_mul_atrt(const aitensor_t *a, const aitensor_t *b, aitensor_t *result);

/** @brief Performs an element wise multiplication of \link aimath_f32.h F32 \endlink tensors a and b (Hadamard product)
  *
  * @f[
  *  result = a \circ b
  * @f]
  *
  * Example:
  * \code{.c}
  * uint16_t a_shape[2] = {2, 3};
  * float a_data[2*3] = {1.0f, 2.0f, 3.0f,
  *                      4.0f, 5.0f, 6.0f};
  * aitensor_t a = AITENSOR_2D_F32(a_shape, a_data);
  *
  * uint16_t b_shape[2] = {2, 3};
  * float b_data[2*3] = {1.0f, 2.0f, 3.0f,
  *                      4.0f, 5.0f, 6.0f};
  * aitensor_t b = AITENSOR_2D_F32(b_shape, b_data);
  *
  * uint16_t result_shape[2] = {2, 3};
  * float result_data[2*3];
  * aitensor_t result = AITENSOR_2D_F32(result_shape, result_data);
  *
  * aimath_f32_default_multiply(&a, &b, &result);
  *
  * print_aitensor(&result);
  * \endcode
  *
  * @param *a       F32 tensor a (N-D tensor)
  * @param *b       F32 tensor b (N-D tensor)
  * @param *result  Resulting F32 tensor of the element wise multiplication (N-D tensor)
  */
void aimath_f32_default_multiply(const aitensor_t *a, const aitensor_t *b, aitensor_t *result);

/** @brief Performs an element wise division of \link aimath_f32.h F32 \endlink tensors a and b (Hadamard division)
  *
  * @f[
  *  result = a \oslash b
  * @f]
  *
  * Example:
  * \code{.c}
  * uint16_t a_shape[2] = {2, 3};
  * float a_data[2*3] = {1.0f, 2.0f, 3.0f,
  *                      4.0f, 5.0f, 6.0f};
  * aitensor_t a = AITENSOR_2D_F32(a_shape, a_data);
  *
  * uint16_t b_shape[2] = {2, 3};
  * float b_data[2*3] = {1.0f, 2.0f, 3.0f,
  *                      4.0f, 5.0f, 6.0f};
  * aitensor_t b = AITENSOR_2D_F32(b_shape, b_data);
  *
  * uint16_t result_shape[2] = {2, 3};
  * float result_data[2*3];
  * aitensor_t result = AITENSOR_2D_F32(result_shape, result_data);
  *
  * aimath_f32_default_divide(&a, &b, &result);
  *
  * print_aitensor(&result);
  * \endcode
  *
  * @param *a       F32 tensor a (N-D tensor)
  * @param *b       F32 tensor b (N-D tensor)
  * @param *result  Resulting F32 tensor of the element wise division (N-D tensor)
  */
void aimath_f32_default_divide(const aitensor_t *a, const aitensor_t *b, aitensor_t *result);

/** @brief Performs a scalar multiplication (scaling) of \link aimath_f32.h F32 \endlink tensor a and a scalar
  *
  * @f[
  *  result = scalar \cdot a
  * @f]
  *
  * Example:
  * \code{.c}
  * uint16_t a_shape[2] = {2, 3};
  * float a_data[2*3] = {0.0f, 1.0f, 2.0f,
  *                      3.0f, 4.0f, 5.0f};
  * aitensor_t a = AITENSOR_2D_F32(a_shape, a_data);
  *
  * float scalar = 0.1f;
  *
  * uint16_t result_shape[2] = {2, 3};
  * float result_data[2*3];
  * aitensor_t result = AITENSOR_2D_F32(result_shape, result_data);
  *
  * aimath_f32_default_scalar_mul(&scalar, &a, &result);
  *
  * print_aitensor(&result);
  * \endcode
  *
  * @param *scalar  Scalar (type aiscalar_f32_t / float)
  * @param *a       F32 tensor a (N-D tensor)
  * @param *result  Resulting F32 tensor of the scalar multiplication (N-D tensor)
  */
void aimath_f32_default_scalar_mul(const void *scalar, const aitensor_t *a, aitensor_t *result);

/** @brief Performs an element wise addition of a scalar to a \link aimath_f32.h F32 \endlink tensor
  *
  * @f[
  *  result = a + \left( \begin{array}{ccc} 1 & \ldots & 1 \\
                          \vdots & \ddots & \vdots \\
                          1 & \ldots & 1
                   \end{array}\right) \cdot scalar
  * @f]
  *
  * Example:
  * \code{.c}
  * uint16_t a_shape[2] = {2, 3};
  * float a_data[2*3] = {0.0f, 1.0f, 2.0f,
  *                      3.0f, 4.0f, 5.0f};
  * aitensor_t a = AITENSOR_2D_F32(a_shape, a_data);
  *
  * float scalar = 0.1f;
  *
  * uint16_t result_shape[2] = {2, 3};
  * float result_data[2*3];
  * aitensor_t result = AITENSOR_2D_F32(result_shape, result_data);
  *
  * aimath_f32_default_scalar_add(&scalar, &a, &result);
  *
  * print_aitensor(&result);
  * \endcode
  *
  * @param *scalar  Scalar (type aiscalar_f32_t / float)
  * @param *a       F32 tensor a (N-D tensor)
  * @param *result  Resulting F32 tensor of the element wise scalar addition (N-D tensor)
  */
void aimath_f32_default_scalar_add(const void *scalar, const aitensor_t *a, aitensor_t *result);

/** @brief Performs an element wise addition of \link aimath_f32.h F32 \endlink tensors a and b
  *
  * @f[
  *  result = a + b
  * @f]
  *
  * Example:
  * \code{.c}
  * uint16_t a_shape[2] = {2, 3};
  * float a_data[2*3] = {1.0f, 2.0f, 3.0f,
  *                      4.0f, 5.0f, 6.0f};
  * aitensor_t a = AITENSOR_2D_F32(a_shape, a_data);
  *
  * uint16_t b_shape[2] = {2, 3};
  * float b_data[2*3] = {1.0f, 2.0f, 3.0f,
  *                      4.0f, 5.0f, 6.0f};
  * aitensor_t b = AITENSOR_2D_F32(b_shape, b_data);
  *
  * uint16_t result_shape[2] = {2, 3};
  * float result_data[2*3];
  * aitensor_t result = AITENSOR_2D_F32(result_shape, result_data);
  *
  * aimath_f32_default_tensor_add(&a, &b, &result);
  *
  * print_aitensor(&result);
  * \endcode
  *
  * @param *a       F32 tensor a (N-D tensor)
  * @param *b       F32 tensor b (N-D tensor)
  * @param *result  Resulting F32 tensor of the element wise addition (N-D tensor)
  */
void aimath_f32_default_tensor_add(const aitensor_t *a, const aitensor_t *b, aitensor_t *result);


/** @brief Performs a element wise subtraction of \link aimath_f32.h F32 \endlink tensors a and b
  *
  * @f[
  *  result = a - b
  * @f]
  *
  * Example:
  * \code{.c}
  * uint16_t a_shape[2] = {2, 3};
  * float a_data[2*3] = {1.0f, 2.0f, 3.0f,
  *                      4.0f, 5.0f, 6.0f};
  * aitensor_t a = AITENSOR_2D_F32(a_shape, a_data);
  *
  * uint16_t b_shape[2] = {2, 3};
  * float b_data[2*3] = {1.0f, 2.0f, 3.0f,
  *                      4.0f, 5.0f, 6.0f};
  * aitensor_t b = AITENSOR_2D_F32(b_shape, b_data);
  *
  * uint16_t result_shape[2] = {2, 3};
  * float result_data[2*3];
  * aitensor_t result = AITENSOR_2D_F32(result_shape, result_data);
  *
  * aimath_f32_default_tensor_sub(&a, &b, &result);
  *
  * print_aitensor(&result);
  * \endcode
  *
  * @param *a       F32 tensor a (N-D tensor)
  * @param *b       F32 tensor b (N-D tensor)
  * @param *result  Resulting F32 tensor of the element wise subtraction (N-D tensor)
  */
void aimath_f32_default_tensor_sub(const aitensor_t *a, const aitensor_t *b, aitensor_t *result);

/** @brief Performs a subtraction between a \link aimath_f32.h F32 \endlink matrix a and a \link aimath_u8.h U8 \endlink sparse matrix b
  *
  * This function can subtract a row wise one-hot encoded matrix in sparse representation
  * (just the integer index of the 1 is stored) from a normal F32 matrix a.
  *
  * For example the matrix
  * @f[
  *   \left( \begin{array}{ccc} 0 & 0 & 0 & 1 \\ 1 & 0 & 0 & 0 \\ 0 & 0 & 1 & 0 \end{array}\right)
  * @f]
  * in sparse representation is
  * @f[
  *   \left( \begin{array}{ccc} 3 \\ 0 \\ 2 \end{array}\right)
  * @f]
  *
  * The result is then calculated as
  * @f[
  *  result_{ij} = \begin{cases}
                    a_{ij} - 1 & \text{if } j = b_i\\
                    a_{ij} & \text{if } j \neq b_i
                    \end{cases}
  * @f]
  *
  * Example:
  * \code{.c}
  * uint16_t a_shape[2] = {2, 3};
  * float a_data[2*3] = {0.2f, 0.1f, 0.7f,
  *                      0.9f, 0.1f, 0.0f};
  * aitensor_t a = AITENSOR_2D_F32(a_shape, a_data);
  *
  * uint16_t b_shape[2] = {2, 1};
  * uint8_t b_data[2*1] = {2,
  *                        0};
  * aitensor_t b = AITENSOR_2D_U8(b_shape, b_data);
  *
  * uint16_t result_shape[2] = {2, 3};
  * float result_data[2*3];
  * aitensor_t result = AITENSOR_2D_F32(result_shape, result_data);
  *
  * aimath_f32_default_tensor_sub_sparse8(&a, &b, &result);
  *
  * print_aitensor(&result);
  * \endcode
  *
  * @param *a       F32 matrix a (2D tensor of shape [N x M])
  * @param *b       U8 sparse matrix b (2D tensor of shape [N x 1])
  * @param *result  Resulting F32 tensor of the subtraction (2D tensor of shape [N x M])
  */
void aimath_f32_default_tensor_sub_sparse8(const aitensor_t *a, const aitensor_t *b, aitensor_t *result);

/** @brief Performs an element wise copy of \link aimath_f32.h F32 \endlink tensors
  *
  * @f[
  *  to \leftarrow from
  * @f]
  *
  * Dimension and shape of from and to tensors have to be the same.
  *
  * Example:
  * \code{.c}
  * uint16_t from_shape[2] = {2, 3};
  * float from_data[2*3] = {1.0f, 2.0f, 3.0f,
  *                         4.0f, 5.0f, 6.0f};
  * aitensor_t from = AITENSOR_2D_F32(from_shape, from_data);
  *
  * uint16_t to_shape[2] = {2, 3};
  * float to_data[2*3];
  * aitensor_t to = AITENSOR_2D_F32(to_shape, to_data);
  *
  * aimath_f32_default_copy_tensor(&from, &to);
  *
  * print_aitensor(&to);
  * \endcode
  *
  * @param *from    F32 tensor to copy from (N-D tensor)
  * @param *to      F32 tensor to copy to (N-D tensor)
  */
void aimath_f32_default_copy_tensor(const aitensor_t *from, aitensor_t *to);

/** @brief Transposes a \link aimath_f32.h F32 \endlink vector
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
  * float vector_data[1*3] = {1.0f, 2.0f, 3.0f};
  * aitensor_t vector = AITENSOR_2D_F32(vector_shape, vector_data);
  *
  * aimath_f32_default_transpose_vector(&vector);
  *
  * print_aitensor(&vector);
  * \endcode
  *
  * @param *vector F32 vector (2D tensor of shape [1 x N] or [N x 1])
  */
void aimath_f32_default_transpose_vector(aitensor_t *vector);

/** @brief Transpose a \link aimath_f32.h F32 \endlink tensor
  *
  * @f[
  *  x \leftarrow x^T
  * @f]
  *
  * Example:
  * \code{.c}
  * uint16_t x_shape[2] = {2, 3};
  * float x_data[2*3] = {  2.0f, -4.0f,   6.0f,
  *                       -8.0f, 10.0f, -12.0f};
  * aitensor_t x = AITENSOR_2D_F32(x_shape, x_data);
  *
  * aimath_f32_default_transpose_matrix(&x);
  *
  * print_aitensor(&x);
  * \endcode
  *
  * @param *x F32 tensor to be transposed (2D tensor)
  */
void aimath_f32_default_transpose_matrix(aitensor_t *x);

/** @brief Calculates the squared sum of all elements in a \link aimath_f32.h F32 \endlink tensor
  *
  * @f[
  *  result = \sum_i x_{i}^2
  * @f]
  *
  * Example:
  * \code{.c}
  * uint16_t x_shape[2] = {2, 3};
  * float x_data[2*3] = {0.0f, 1.0f, 2.0f,
  *                      3.0f, 4.0f, 5.0f};
  * aitensor_t x = AITENSOR_2D_F32(x_shape, x_data);
  *
  * float result;
  *
  * aimath_f32_default_norm_squared(&x, &result);
  *
  * print_aiscalar(&result, aif32);
  * \endcode
  *
  * @param *x       F32 tensor x (N-D tensor)
  * @param *result  Scalar result (type aiscalar_f32_t / float)
  */
void aimath_f32_default_norm_squared(const aitensor_t *x, void *result);

/** @brief Calculates the sum of all elements in a \link aimath_f32.h F32 \endlink tensor
  *
  * @f[
  *  result = \sum_i x_{i}
  * @f]
  *
  * Example:
  * \code{.c}
  * uint16_t x_shape[2] = {2, 3};
  * float x_data[2*3] = {0.0f, 1.0f, 2.0f,
  *                      3.0f, 4.0f, 5.0f};
  * aitensor_t x = AITENSOR_2D_F32(x_shape, x_data);
  *
  * float result;
  *
  * aimath_f32_default_sum(&x, &result);
  *
  * print_aiscalar(&result, aif32);
  * \endcode
  *
  * @param *x       F32 tensor x (N-D tensor)
  * @param *result  Scalar result (type aiscalar_f32_t / float)
  */
void aimath_f32_default_sum(const aitensor_t *x, void *result);

/** @brief Identifies the minimum value in a \link aimath_f32.h F32 \endlink tensor
  *
  * @f[
  *  result = min(x)
  * @f]
  *
  * Example:
  * \code{.c}
  * uint16_t x_shape[2] = {2, 3};
  * float x_data[2*3] = {0.0f, 1.0f, 2.0f,
  *                      3.0f, 4.0f, 5.0f};
  * aitensor_t x = AITENSOR_2D_F32(x_shape, x_data);
  *
  * float result;
  *
  * aimath_f32_default_min(&x, &result);
  *
  * print_aiscalar(&result, aif32);
  * \endcode
  *
  * @param *x       F32 tensor x to get the minimum value of (N-D tensor)
  * @param *result  Scalar result (type aiscalar_f32_t / float)
  */
void aimath_f32_default_min(const aitensor_t *x, void *result);

/** @brief Identifies the maximum value in a \link aimath_f32.h F32 \endlink tensor
  *
  * @details
  *
  * @f[
  *  result = max(x)
  * @f]
  *
  * Example:
  * \code{.c}
  * uint16_t x_shape[2] = {2, 3};
  * float x_data[2*3] = {0.0f, 1.0f, 2.0f,
  *                      3.0f, 4.0f, 5.0f};
  * aitensor_t x = AITENSOR_2D_F32(x_shape, x_data);
  *
  * float result;
  *
  * aimath_f32_default_max(&x, &result);
  *
  * print_aiscalar(&result, aif32);
  * \endcode
  *
  * @param *x       F32 tensor x to get the maximum value of (N-D tensor)
  * @param *result  Scalar result (type aiscalar_f32_t / float)
  */
void aimath_f32_default_max(const aitensor_t *x, void *result);

/** @brief Calculates the sigmoid of each element in a \link aimath_f32.h F32 \endlink tensor
  *
  * @f[
  *   result_{i} = \sigma(x_{i}) = \frac{1}{1 + e^{-x_{i}}}
  * @f]
  *
  * Example:
  * \code{.c}
  * uint16_t x_shape[2] = {2, 3};
  * float x_data[2*3] = { 1.0f, -2.0f,  3.0f,
  *                      -4.0f,  5.0f, -6.0f};
  * aitensor_t x = AITENSOR_2D_F32(x_shape, x_data);
  *
  * uint16_t result_shape[2] = {2, 3};
  * float result_data[2*3];
  * aitensor_t result = AITENSOR_2D_F32(result_shape, result_data);
  *
  * aimath_f32_default_sigmoid(&x, &result);
  *
  * print_aitensor(&result);
  * \endcode
  *
  * @param *x       F32 tensor to calculate the sigmoid from (N-D tensor)
  * @param *result  Resulting F32 tensor (N-D tensor)
  */
void aimath_f32_default_sigmoid(const aitensor_t *x, aitensor_t *result);

/** @brief Calculates the derivative sigmoid of each element in a \link aimath_f32.h F32 \endlink tensor
  *
  * @f[
  *  result_{i} = \sigma'(x_{i}) = \sigma(x_{i}) \cdot (1 - \sigma(x_{i}))
  * @f]
  *
  * Example:
  * \code{.c}
  * uint16_t x_shape[2] = {2, 3};
  * float x_data[2*3] = { 1.0f, -2.0f,  3.0f,
  *                      -4.0f,  5.0f, -6.0f};
  * aitensor_t x = AITENSOR_2D_F32(x_shape, x_data);
  *
  * uint16_t result_shape[2] = {2, 3};
  * float result_data[2*3];
  * aitensor_t result = AITENSOR_2D_F32(result_shape, result_data);
  *
  * aimath_f32_default_sigmoid(&x, &result);
  * aimath_f32_default_d_sigmoid(&result, &result);
  *
  * print_aitensor(&result);
  * \endcode
  *
  * @param *sigmoid_x   F32 tensor with the sigmoid values \f$ \sigma(x_{i}) \f$ (N-D tensor)
  * @param *result      Resulting F32 tensor (N-D tensor)
  */
void aimath_f32_default_d_sigmoid(const aitensor_t *sigmoid_x, aitensor_t *result);

/** @brief Calculates the tanh of each element in a \link aimath_f32.h F32 \endlink tensor
 *
 * @f[
 *   result_{i} = \tanh(x_{i}) = \frac{e^{x_i} - e^{-x_i}}{e^{x_i} + e^{-x_i}}
 * @f]
 *
 * Example:
 * \code{.c}
 * uint16_t x_shape[2] = {2, 3};
 * float x_data[2*3] = { 1.0f, -2.0f,  3.0f,
 *                      -4.0f,  5.0f, -6.0f};
 * aitensor_t x = AITENSOR_2D_F32(x_shape, x_data);
 *
 * uint16_t result_shape[2] = {2, 3};
 * float result_data[2*3];
 * aitensor_t result = AITENSOR_2D_F32(result_shape, result_data);
 *
 * aimath_f32_default_tanh(&x, &result);
 *
 * print_aitensor(&result);
 * \endcode
 *
 * @param *x        F32 tensor to calculate the tanh from (N-D tensor)
 * @param *result   Resulting F32 tensor (N-D tensor)
 */
void aimath_f32_default_tanh(const aitensor_t *x, aitensor_t *result);

/** @brief Calculates the tanh derivative of each element in a \link aimath_f32.h F32 \endlink tensor
 *
 * @f[
 *   result_{i} = tanh'(x_{i}) = 1 - tanh(x_{i})^2
 * @f]
  *
  * Example:
  * \code{.c}
  * uint16_t x_shape[2] = {2, 3};
  * float x_data[2*3] = { 1.0f, -2.0f,  3.0f,
  *                      -4.0f,  5.0f, -6.0f};
  * aitensor_t x = AITENSOR_2D_F32(x_shape, x_data);
  *
  * uint16_t result_shape[2] = {2, 3};
  * float result_data[2*3];
  * aitensor_t result = AITENSOR_2D_F32(result_shape, result_data);
  *
  * aimath_f32_default_tanh(&x, &result);
  * aimath_f32_default_d_tanh(&result, &result);
  *
  * print_aitensor(&result);
  * \endcode
 *
 * @param *tanh_x   F32 tensor with the tanh values \f$ \tanh(x_{i}) \f$ (N-D tensor)
 * @param *result   Resulting F32 tensor (N-D tensor)
 */
void aimath_f32_default_d_tanh(const aitensor_t *tanh_x, aitensor_t *result);

/** @brief Calculates the rectifier (ReLU) value of each element in a \link aimath_f32.h F32 \endlink tensor
  *
  * @f[
  *  result_{i} = max(0, x_{i})
  * @f]
  *
  * Example:
  * \code{.c}
  * uint16_t x_shape[2] = {2, 3};
  * float x_data[2*3] = { 1.0f, -2.0f,  3.0f,
  *                      -4.0f,  5.0f, -6.0f};
  * aitensor_t x = AITENSOR_2D_F32(x_shape, x_data);
  *
  * uint16_t result_shape[2] = {2, 3};
  * float result_data[2*3];
  * aitensor_t result = AITENSOR_2D_F32(result_shape, result_data);
  *
  * aimath_f32_default_relu(&x, &result);
  *
  * print_aitensor(&result);
  * \endcode
  *
  * @param *x       F32 tensor to calculate the ReLU from (N-D tensor)
  * @param *result  Resulting F32 tensor (N-D tensor)
  */
void aimath_f32_default_relu(const aitensor_t *x, aitensor_t *result);


/** @brief Calculates the rectifier (ReLU) derivative of each element in a \link aimath_f32.h F32 \endlink tensor
  *
  * @f[
  *  result_{ij} = \begin{cases}
                    0 & \text{if } x_i < 0\\
                    1 & \text{if } x_i \geq 0
                    \end{cases}
  * @f]
  *
  * Example:
  * \code{.c}
  * uint16_t x_shape[2] = {2, 3};
  * float x_data[2*3] = { 1.0f, -2.0f,  3.0f,
  *                      -4.0f,  5.0f, -6.0f};
  * aitensor_t x = AITENSOR_2D_F32(x_shape, x_data);
  *
  * uint16_t result_shape[2] = {2, 3};
  * float result_data[2*3];
  * aitensor_t result = AITENSOR_2D_F32(result_shape, result_data);
  *
  * aimath_f32_default_d_relu(&x, &result);
  *
  * print_aitensor(&result);
  * \endcode
  *
  * @param *x       F32 tensor to calculate the ReLU derivative from (N-D tensor)
  * @param *result  Resulting F32 tensor (N-D tensor)
  */
void aimath_f32_default_d_relu(const aitensor_t *x, aitensor_t *result);

/** @brief Calculates the leaky rectifier (leaky ReLU) value of each element in a \link aimath_f32.h F32 \endlink tensor
 *
 * @f[
 *  result_{i} = \begin{cases}
        \alpha \cdot x_i & \text{if } x_i < 0 \\
          x_i & \text{if } x_i \geq 0
     \end{cases}
 * @f]
 *
 * Example:
 * \code{.c}
 * uint16_t x_shape[2] = {2, 3};
 * float x_data[2*3] = { 1.0f, -2.0f,  3.0f,
 *                      -4.0f,  5.0f, -6.0f};
 * aitensor_t x = AITENSOR_2D_F32(x_shape, x_data);
 *
 * float alpha = 0.01f;
 *
 * uint16_t result_shape[2] = {2, 3};
 * float result_data[2*3];
 * aitensor_t result = AITENSOR_2D_F32(result_shape, result_data);
 *
 * aimath_f32_default_leaky_relu(&x, &alpha, &result);
 *
 * print_aitensor(&result);
 * \endcode
 *
 * @param *x        F32 tensor to calculate the leaky ReLU from (N-D tensor)
 * @param *alpha    Scalar \f$ \alpha \f$ (type aiscalar_f32_t / float) for the leakage
 * @param *result   Resulting F32 tensor (N-D tensor)
 */
void aimath_f32_default_leaky_relu(const aitensor_t *x, const void *alpha, aitensor_t *result);

/** @brief Calculates the leaky rectifier (leaky ReLU) derivative of each element in a \link aimath_f32.h F32 \endlink tensor
 *
 * @f[
 *  result_{i} = \begin{cases}
                \alpha & \text{if } x_i < 0\\
                1 & \text{if } x_i \geq 0
                \end{cases}
 * @f]
 *
 * Example:
 * \code{.c}
 * uint16_t x_shape[2] = {2, 3};
 * float x_data[2*3] = { 1.0f, -2.0f,  3.0f,
 *                      -4.0f,  5.0f, -6.0f};
 * aitensor_t x = AITENSOR_2D_F32(x_shape, x_data);
 *
 * float alpha = 0.01f;
 *
 * uint16_t result_shape[2] = {2, 3};
 * float result_data[2*3];
 * aitensor_t result = AITENSOR_2D_F32(result_shape, result_data);
 *
 * aimath_f32_default_d_leaky_relu(&x, &alpha, &result);
 *
 * print_aitensor(&result);
 * \endcode
 *
 * @param *x        F32 tensor to calculate the leaky ReLU derivative from (N-D tensor)
 * @param *alpha    Scalar \f$ \alpha \f$ (type aiscalar_f32_t / float) for the leakage
 * @param *result   Resulting F32 tensor (N-D tensor)
 */
void aimath_f32_default_d_leaky_relu(const aitensor_t *x, const void *alpha, aitensor_t *result);

/** @brief Calculates the exponential rectifier (ELU) value of each element in a \link aimath_f32.h F32 \endlink tensor
 *
 * @f[
 *  result_{i} = \begin{cases}
        \alpha \cdot (e^{x_i} - 1) & \text{if } x_i < 0 \\
        x_i & \text{if } x_i \geq 0
    \end{cases}
 * @f]
 *
 * Example:
 * \code{.c}
 * uint16_t x_shape[2] = {2, 3};
 * float x_data[2*3] = { 1.0f, -2.0f,  3.0f,
 *                      -4.0f,  5.0f, -6.0f};
 * aitensor_t x = AITENSOR_2D_F32(x_shape, x_data);
 *
 * float alpha = 1.0f;
 *
 * uint16_t result_shape[2] = {2, 3};
 * float result_data[2*3];
 * aitensor_t result = AITENSOR_2D_F32(result_shape, result_data);
 *
 * aimath_f32_default_elu(&x, &alpha, &result);
 *
 * print_aitensor(&result);
 * \endcode
 *
 * @param *x        F32 tensor to calculate the ELU from (N-D tensor)
 * @param *alpha    Scalar \f$ \alpha \f$ (type aiscalar_f32_t / float)
 * @param *result   Resulting F32 tensor (N-D tensor)
 */
void aimath_f32_default_elu(const aitensor_t *x, const void *alpha, aitensor_t *result);

/** @brief Calculates the exponential rectifier (ELU) derivative of each element in a \link aimath_f32.h F32 \endlink tensor
 *
 * @f[
 *  result_{i} = \begin{cases}
                \alpha \cdot e^{x_i} & \text{if } x_i < 0\\
                1 & \text{if } x_i \geq 0
                \end{cases}
 * @f]
 *
 * Example:
 * \code{.c}
 * uint16_t x_shape[2] = {2, 3};
 * float x_data[2*3] = { 1.0f, -2.0f,  3.0f,
 *                      -4.0f,  5.0f, -6.0f};
 * aitensor_t x = AITENSOR_2D_F32(x_shape, x_data);
 *
 * float alpha = 1.0f;
 *
 * uint16_t result_shape[2] = {2, 3};
 * float result_data[2*3];
 * aitensor_t result = AITENSOR_2D_F32(result_shape, result_data);
 *
 * aimath_f32_default_d_elu(&x, &alpha, &result);
 *
 * print_aitensor(&result);
 * \endcode
 *
 * @param *x        F32 tensor to calculate the ELU derivative from (N-D tensor)
 * @param *alpha    Scalar \f$ \alpha \f$ (type aiscalar_f32_t / float)
 * @param *result   Resulting F32 tensor (N-D tensor)
 */
void aimath_f32_default_d_elu(const aitensor_t *x, const void *alpha, aitensor_t *result);

/** @brief Calculates the softmax value of each row of a \link aimath_f32.h F32 \endlink matrix
 *
 * @f[
 *  result_{i} = \frac{e^{x_i}}{\sum_{j=1}^{K} e^{x_j}}
 * @f]
 *
 * Example:
 * \code{.c}
 * uint16_t x_shape[2] = {2, 3};
 * float x_data[2*3] = { 1.0f, -2.0f,  3.0f,
 *                      -4.0f,  5.0f, -6.0f};
 * aitensor_t x = AITENSOR_2D_F32(x_shape, x_data);
 *
 * uint16_t result_shape[2] = {2, 3};
 * float result_data[2*3];
 * aitensor_t result = AITENSOR_2D_F32(result_shape, result_data);
 *
 * aimath_f32_default_softmax(&x, &result);
 *
 * print_aitensor(&result);
 * \endcode
 *
 * @param *x       F32 matrix to calculate the softmax from (2D tensor)
 * @param *result  Resulting F32 matrix (2D tensor)
 */
void aimath_f32_default_softmax(const aitensor_t *x, aitensor_t *result);

/** @brief Calculates the softsign value of each element in a \link aimath_f32.h F32 \endlink tensor
 *
 * @f[
 *  result_{i} = \frac {x_i} {1 + |x_i|}
 * @f]
 *
 * Example:
 * \code{.c}
 * uint16_t x_shape[2] = {2, 3};
 * float x_data[2*3] = { 1.0f, -2.0f,  3.0f,
 *                      -4.0f,  5.0f, -6.0f};
 * aitensor_t x = AITENSOR_2D_F32(x_shape, x_data);
 *
 * uint16_t result_shape[2] = {2, 3};
 * float result_data[2*3];
 * aitensor_t result = AITENSOR_2D_F32(result_shape, result_data);
 *
 * aimath_f32_default_softsign(&x, &result);
 *
 * print_aitensor(&result);
 * \endcode
 *
 * @param *x       F32 tensor to calculate the softsign from (N-D tensor)
 * @param *result  Resulting F32 tensor (N-D tensor)
 */
void aimath_f32_default_softsign(const aitensor_t *x, aitensor_t *result);

/** @brief Calculates the softsign derivative of each element in a \link aimath_f32.h F32 \endlink tensor
 *
 * @f[
 *  result_{i} = \frac {x_i} {(1 + |x_i|)^2}
 * @f]
 *
 * Example:
 * \code{.c}
 * uint16_t x_shape[2] = {2, 3};
 * float x_data[2*3] = { 1.0f, -2.0f,  3.0f,
 *                      -4.0f,  5.0f, -6.0f};
 * aitensor_t x = AITENSOR_2D_F32(x_shape, x_data);
 *
 * uint16_t result_shape[2] = {2, 3};
 * float result_data[2*3];
 * aitensor_t result = AITENSOR_2D_F32(result_shape, result_data);
 *
 * aimath_f32_default_d_softsign(&x, &result);
 *
 * print_aitensor(&result);
 * \endcode
 *
 * @param *x       F32 tensor to calculate the softsign derivative from (N-D tensor)
 * @param *result  Resulting F32 tensor (N-D tensor)
 */
void aimath_f32_default_d_softsign(const aitensor_t *x, aitensor_t *result);

/** @brief Calculates the binary cross entropy between the \link aimath_f32.h F32 \endlink predicted and the target data using a sum reduction
  *
  * @f[
  *  result =  - \sum_i (target_i \cdot \log(predicted_i) + (1 - target_i) \cdot \log(1 - predicted_i))
  * @f]
  *
  * Example:
  * \code{.c}
  * uint16_t p_shape[2] = {2, 3};
  * float p_data[2*3] = {0.8f, 0.1f, 0.7f,
  *                      0.2f, 0.3f, 0.0f};
  * aitensor_t p = AITENSOR_2D_F32(p_shape, p_data);
  *
  * uint16_t t_shape[2] = {2, 3};
  * float t_data[2*3] = {1.0f, 0.0f, 1.0f,
  *                      0.0f, 0.0f, 0.0f};
  * aitensor_t t = AITENSOR_2D_F32(t_shape, t_data);
  *
  * float result;
  *
  * aimath_f32_default_binary_crossentropy_sum(&p, &t, &result);
  *
  * print_aiscalar(&result, aif32);
  * \endcode
  *
  * @param *predicted_data  F32 matrix with the predicted or calculated values (2D tensor)
  * @param *target_data     F32 matrix with the target data / true values / labels (2D tensor with binary values 0 or 1)
  * @param *result          Resulting F32 matrix (2D tensor)
  */
void aimath_f32_default_binary_crossentropy_sum(const aitensor_t *predicted_data, const aitensor_t *target_data, void *result);

/** @brief Calculates the binary cross entropy between the \link aimath_f32.h F32 \endlink predicted and the target data using a mean reduction
  *
  * @f[
  *  result =  - \sum_i (target_i \cdot \log(predicted_i) + (1 - target_i) \cdot \log(1 - predicted_i))
  * @f]
  *
  * Example:
  * \code{.c}
  * uint16_t p_shape[2] = {2, 3};
  * float p_data[2*3] = {0.8f, 0.1f, 0.7f,
  *                      0.2f, 0.3f, 0.0f};
  * aitensor_t p = AITENSOR_2D_F32(p_shape, p_data);
  *
  * uint16_t t_shape[2] = {2, 3};
  * float t_data[2*3] = {1.0f, 0.0f, 1.0f,
  *                      0.0f, 0.0f, 0.0f};
  * aitensor_t t = AITENSOR_2D_F32(t_shape, t_data);
  *
  * float result;
  *
  * aimath_f32_default_binary_crossentropy_mean(&p, &t, &result);
  *
  * print_aiscalar(&result, aif32);
  * \endcode
  *
  * @param *predicted_data  F32 matrix with the predicted or calculated values (2D tensor)
  * @param *target_data     F32 matrix with the target data / true values / labels (2D tensor with binary values 0 or 1)
  * @param *result          Resulting F32 matrix (2D tensor)
  */
void aimath_f32_default_binary_crossentropy_mean(const aitensor_t *predicted_data, const aitensor_t *target_data, void *result);

/** @brief Calculates the categorical cross entropy between the \link aimath_f32.h F32 \endlink predicted and the target data using a sum reduction
  *
  * @f[
  *  result =  - \sum_i target_i \cdot \log(predicted_i)
  * @f]
  *
  * Example:
  * \code{.c}
  * uint16_t p_shape[2] = {2, 3};
  * float p_data[2*3] = {0.2f, 0.1f, 0.7f,
  *                      0.9f, 0.1f, 0.0f};
  * aitensor_t p = AITENSOR_2D_F32(p_shape, p_data);
  *
  * uint16_t t_shape[2] = {2, 3};
  * float t_data[2*3] = {0.0f, 0.0f, 1.0f,
  *                      1.0f, 0.0f, 0.0f};
  * aitensor_t t = AITENSOR_2D_F32(t_shape, t_data);
  *
  * float result;
  *
  * aimath_f32_default_categorical_crossentropy_sum(&p, &t, &result);
  *
  * print_aiscalar(&result, aif32);
  * \endcode
  *
  * @param *predicted_data  F32 matrix with the predicted or calculated values (2D tensor)
  * @param *target_data     F32 matrix with the target data / true values / labels (2D tensor, rows one-hot encoded)
  * @param *result          Resulting F32 matrix (2D tensor)
  */
void aimath_f32_default_categorical_crossentropy_sum(const aitensor_t *predicted_data, const aitensor_t *target_data, void *result);

/** @brief Calculates the categorical cross entropy between the \link aimath_f32.h F32 \endlink predicted and the target data using a mean reduction
  *
  * @f[
  *  result =  - \sum_i target_i \cdot \log(predicted_i)
  * @f]
  *
  * Example:
  * \code{.c}
  * uint16_t p_shape[2] = {2, 3};
  * float p_data[2*3] = {0.2f, 0.1f, 0.7f,
  *                      0.9f, 0.1f, 0.0f};
  * aitensor_t p = AITENSOR_2D_F32(p_shape, p_data);
  *
  * uint16_t t_shape[2] = {2, 3};
  * float t_data[2*3] = {0.0f, 0.0f, 1.0f,
  *                      1.0f, 0.0f, 0.0f};
  * aitensor_t t = AITENSOR_2D_F32(t_shape, t_data);
  *
  * float result;
  *
  * aimath_f32_default_categorical_crossentropy_mean(&p, &t, &result);
  *
  * print_aiscalar(&result, aif32);
  * \endcode
  *
  * @param *predicted_data  F32 matrix with the predicted or calculated values (2D tensor)
  * @param *target_data     F32 matrix with the target data / true values / labels (2D tensor, rows one-hot encoded)
  * @param *result          Resulting F32 matrix (2D tensor)
  */
void aimath_f32_default_categorical_crossentropy_mean(const aitensor_t *predicted_data, const aitensor_t *target_data, void *result);

/** @brief Calculates the categorical Cross-Entropy between the \link aimath_f32.h F32 \endlink predicted data and the \link aimath_u8.h U8 \endlink target data in sparse representation using a sum reduction
  *
  * This function can calculate the crossentropy between a row wise one-hot encoded matrix in sparse representation
  * (just the integer index of the 1 is stored) and a normal F32 matrix a.
  *
  * For example the matrix
  * @f[
  *   \left( \begin{array}{ccc} 0 & 0 & 0 & 1 \\ 1 & 0 & 0 & 0 \\ 0 & 0 & 1 & 0 \end{array}\right)
  * @f]
  * in sparse representation is
  * @f[
  *   \left( \begin{array}{ccc} 3 \\ 0 \\ 2 \end{array}\right)
  * @f]
  *
  * The result is then calculated from the one-hot encoded target matrix:
  * @f[
  *  result =  - \sum_i target_{one-hot,i} \cdot \log(predicted_i)
  * @f]
  *
  * Example:
  * \code{.c}
  * uint16_t p_shape[2] = {2, 3};
  * float p_data[2*3] = {0.2f, 0.1f, 0.7f,
  *                      0.9f, 0.1f, 0.0f};
  * aitensor_t p = AITENSOR_2D_F32(p_shape, p_data);
  *
  * uint16_t t_shape[2] = {2, 1};
  * uint8_t t_data[2*1] = {2,
  *                        0};
  * aitensor_t t = AITENSOR_2D_U8(t_shape, t_data);
  *
  * float result;
  *
  * aimath_f32_default_categorical_crossentropy_sum_sparse8(&p, &t, &result);
  *
  * print_aiscalar(&result, aif32);
  * \endcode
  *
  * @param *predicted_data  F32 matrix with the predicted or calculated values (2D tensor of shape [N x M])
  * @param *target_data     U8 matrix with the target data / true values / labels (2D tensor of shape [N x 1] with true class indices)
  * @param *result          Resulting F32 matrix (2D tensor of shape [N x M])
  */
void aimath_f32_default_categorical_crossentropy_sum_sparse8(const aitensor_t *predicted_data, const aitensor_t *target_data, void *result);

/** @brief Calculates the categorical Cross-Entropy between the \link aimath_f32.h F32 \endlink predicted data and the \link aimath_u8.h U8 \endlink target data in sparse representation using a mean reduction
  *
  * This function can calculate the crossentropy between a row wise one-hot encoded matrix in sparse representation
  * (just the integer index of the 1 is stored) and a normal F32 matrix a.
  *
  * For example the matrix
  * @f[
  *   \left( \begin{array}{ccc} 0 & 0 & 0 & 1 \\ 1 & 0 & 0 & 0 \\ 0 & 0 & 1 & 0 \end{array}\right)
  * @f]
  * in sparse representation is
  * @f[
  *   \left( \begin{array}{ccc} 3 \\ 0 \\ 2 \end{array}\right)
  * @f]
  *
  * The result is then calculated from the one-hot encoded target matrix:
  * @f[
  *  result =  - \sum_i target_{one-hot,i} \cdot \log(predicted_i)
  * @f]
  *
  * Example:
  * \code{.c}
  * uint16_t p_shape[2] = {2, 3};
  * float p_data[2*3] = {0.2f, 0.1f, 0.7f,
  *                      0.9f, 0.1f, 0.0f};
  * aitensor_t p = AITENSOR_2D_F32(p_shape, p_data);
  *
  * uint16_t t_shape[2] = {2, 1};
  * uint8_t t_data[2*1] = {2,
  *                        0};
  * aitensor_t t = AITENSOR_2D_U8(t_shape, t_data);
  *
  * float result;
  *
  * aimath_f32_default_categorical_crossentropy_mean_sparse8(&p, &t, &result);
  *
  * print_aiscalar(&result, aif32);
  * \endcode
  *
  * @param *predicted_data  F32 matrix with the predicted or calculated values (2D tensor of shape [N x M])
  * @param *target_data     U8 matrix with the target data / true values / labels (2D tensor of shape [N x 1] with true class indices)
  * @param *result          Resulting F32 matrix (2D tensor of shape [N x M])
  */
void aimath_f32_default_categorical_crossentropy_mean_sparse8(const aitensor_t *predicted_data, const aitensor_t *target_data, void *result);

/** @brief Calculates the element wise square root of a \link aimath_f32.h F32 \endlink tensor
  *
  * @f[
  *  result_{i} = \sqrt{x_{i}}
  * @f]
  *
  * Example:
  * \code{.c}
  * uint16_t x_shape[2] = {2, 3};
  * float x_data[2*3] = {1.0f, 2.0f, 3.0f,
  *                      4.0f, 5.0f, 6.0f};
  * aitensor_t x = AITENSOR_2D_F32(x_shape, x_data);
  *
  * uint16_t result_shape[2] = {2, 3};
  * float result_data[2*3];
  * aitensor_t result = AITENSOR_2D_F32(result_shape, result_data);
  *
  * aimath_f32_default_sqrt(&x, &result);
  *
  * print_aitensor(&result);
  * \endcode
  *
  * @param *x       F32 tensor to calculate the square root from (N-D tensor)
  * @param *result  Resulting F32 tensor (N-D tensor)
  */
void aimath_f32_default_sqrt(const aitensor_t *x, aitensor_t *result);

/** @brief Fills a \link aimath_f32.h F32 \endlink tensor with zeros
  *
  * @f[
  *  tensor_{i} = 0
  * @f]
  *
  * Example:
  * \code{.c}
  * uint16_t tensor_shape[2] = {2, 3};
  * float tensor_data[2*3];
  * aitensor_t tensor = AITENSOR_2D_F32(tensor_shape, tensor_data);
  *
  * aimath_f32_default_zero_tensor(&tensor);
  *
  * print_aitensor(&tensor);
  * \endcode
  *
  * In the F32 implementation of this function, there is no difference between aimath_f32_default_zero_tensor()
  * and aimath_f32_default_init_zeros().
  *
  * @param *tensor F32 tensor to set to zero (N-D tensor)
  */
void aimath_f32_default_zero_tensor(aitensor_t *tensor);

/** @brief Fills a \link aimath_f32.h F32 \endlink tensor with zeros
  *
  * @f[
  *  tensor_{i} = 0
  * @f]
  *
  * Example:
  * \code{.c}
  * uint16_t tensor_shape[2] = {2, 3};
  * float tensor_data[2*3];
  * aitensor_t tensor = AITENSOR_2D_F32(tensor_shape, tensor_data);
  *
  * aimath_f32_default_init_zeros(&tensor);
  *
  * print_aitensor(&tensor);
  * \endcode
  *
  * In the F32 implementation of this function, there is no difference between aimath_f32_default_zero_tensor()
  * and aimath_f32_default_init_zeros().
  *
  * @param *tensor F32 tensor to set to zero (N-D tensor)
  */
void aimath_f32_default_init_zeros(aitensor_t *tensor);

/** @brief Fills a \link aimath_f32.h F32 \endlink tensor with ones
  *
  * @f[
  *  tensor_{i} = 1
  * @f]
  *
  * Example:
  * \code{.c}
  * uint16_t tensor_shape[2] = {2, 3};
  * float tensor_data[2*3];
  * aitensor_t tensor = AITENSOR_2D_F32(tensor_shape, tensor_data);
  *
  * aimath_f32_default_init_ones(&tensor);
  *
  * print_aitensor(&tensor);
  * \endcode
  *
  * @param *tensor F32 tensor to set to zero (N-D tensor)
  */
void aimath_f32_default_init_ones(aitensor_t *tensor);

/** @brief Fills a \link aimath_f32.h F32 \endlink tensor with random numbers created from a uniform distribution within given range
  *
  * @f[
  *  tensor_i \in \mathcal{U(from, to)}
  * @f]
  *
  * Example:
  * \code{.c}
  * uint16_t tensor_shape[2] = {2, 3};
  * float tensor_data[2*3];
  * aitensor_t tensor = AITENSOR_2D_F32(tensor_shape, tensor_data);
  *
  * aimath_f32_default_tensor_init_uniform(&tensor, -1.5f, 1.5f);
  *
  * print_aitensor(&tensor);
  * \endcode
  *
  * @param *tensor  F32 tensor to initialize with random numbers (N-D tensor)
  * @param from     Minimum value of the uniform distribution
  * @param to       Maximum value of the uniform distribution
  */
void aimath_f32_default_tensor_init_uniform(aitensor_t *tensor, float from, float to);

/** @brief Fills a \link aimath_f32.h F32 \endlink tensor with random numbers uniformly within given range, according to Glorot et al.
  *
  * Same functionality as aimath_f32_default_init_glorot_uniform_cdim() with cin_axis = 0 and cout_axis = 1 (channels last dataformat).
  *
  * @f[
  *  fan_{avg} = \frac{fan_{in} + fan_{out}}{2}
  * @f]
  * @f[
  *  r = \sqrt{\frac{3}{fan_{avg}}}
  * @f]
  * @f[
  *  tensor_i \in \mathcal{U(-r, r)}
  * @f]
  *
  * Example:
  * \code{.c}
  * uint16_t tensor_shape[2] = {2, 3};
  * float tensor_data[2*3];
  * aitensor_t tensor = AITENSOR_2D_F32(tensor_shape, tensor_data);
  *
  * aimath_f32_default_init_glorot_uniform(&tensor);
  *
  * print_aitensor(&tensor);
  * \endcode
  *
  * @see Glorot et al., 2010 ( http://jmlr.org/proceedings/papers/v9/glorot10a/glorot10a.pdf )
  *
  * @param *tensor F32 tensor to initialize with random numbers (N-D tensor)
  */
void aimath_f32_default_init_glorot_uniform(aitensor_t *tensor);

/** @brief Fills a \link aimath_f32.h F32 \endlink tensor with random numbers uniformly within given range, according to Glorot et al.
  *
  * @f[
  *  fan_{avg} = \frac{fan_{in} + fan_{out}}{2}
  * @f]
  * @f[
  *  r = \sqrt{\frac{3}{fan_{avg}}}
  * @f]
  * @f[
  *  tensor_i \in \mathcal{U(-r, r)}
  * @f]
  *
  * Example:
  * \code{.c}
  * uint16_t tensor_shape[2] = {2, 3};
  * float tensor_data[2*3];
  * aitensor_t tensor = AITENSOR_2D_F32(tensor_shape, tensor_data);
  *
  * aimath_f32_default_init_glorot_uniform_cdim(&tensor, 0, 1);
  *
  * print_aitensor(&tensor);
  * \endcode
  *
  * @see Glorot et al., 2010 ( http://jmlr.org/proceedings/papers/v9/glorot10a/glorot10a.pdf )
  *
  * @param *tensor      F32 tensor to initialize with random numbers (N-D tensor)
  * @param cin_axis     Axis of the input channels (negative number means indexing from the end)
  * @param cout_axis    Axis of the output channels (negative number means indexing from the end)
  */
void aimath_f32_default_init_glorot_uniform_cdim(aitensor_t *tensor, int8_t cin_axis, int8_t cout_axis);

/** @brief Fills a \link aimath_f32.h F32 \endlink tensor with uniformly drawn random numbers within given range, according to He et al.
  *
  * Same functionality as aimath_f32_default_init_he_uniform_cdim() with cout_axis = 1 (channels last dataformat).
  *
  * @f[
  *  fan_{avg} = \frac{fan_{in}}{2}
  * @f]
  * @f[
  *  r = \sqrt{\frac{3}{fan_{avg}}}
  * @f]
  * @f[
  *  tensor_i \in \mathcal{U(-r, r)}
  * @f]
  *
  * Example:
  * \code{.c}
  * uint16_t tensor_shape[2] = {2, 3};
  * float tensor_data[2*3];
  * aitensor_t tensor = AITENSOR_2D_F32(tensor_shape, tensor_data);
  *
  * aimath_f32_default_init_he_uniform(&tensor);
  *
  * print_aitensor(&tensor);
  * \endcode
  *
  * @see He et al., 2015 ( https://www.cv-foundation.org/openaccess/content_iccv_2015/html/He_Delving_Deep_into_ICCV_2015_paper.html )
  *
  * @param *tensor F32 tensor to initialize with random numbers (N-D tensor)
  */
void aimath_f32_default_init_he_uniform(aitensor_t *tensor);

/** @brief Fills a \link aimath_f32.h F32 \endlink tensor with uniformly drawn random numbers within given range, according to He et al.
  *
  * @f[
  *  fan_{avg} = \frac{fan_{in}}{2}
  * @f]
  * @f[
  *  r = \sqrt{\frac{3}{fan_{avg}}}
  * @f]
  * @f[
  *  tensor_i \in \mathcal{U(-r, r)}
  * @f]
  *
  * Example:
  * \code{.c}
  * uint16_t tensor_shape[2] = {2, 3};
  * float tensor_data[2*3];
  * aitensor_t tensor = AITENSOR_2D_F32(tensor_shape, tensor_data);
  *
  * aimath_f32_default_init_he_uniform_cdim(&tensor, 1);
  *
  * print_aitensor(&tensor);
  * \endcode
  *
  * @see He et al., 2015 ( https://www.cv-foundation.org/openaccess/content_iccv_2015/html/He_Delving_Deep_into_ICCV_2015_paper.html )
  *
  * @param *tensor      F32 tensor to initialize with random numbers (N-D tensor)
  * @param cout_axis    Axis of the output channels (negative number means indexing from the end)
  */
void aimath_f32_default_init_he_uniform_cdim(aitensor_t *tensor, int8_t cout_axis);

/** @brief Fast approximation of the exponential function
  *
  * @see http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.9.4508&rep=rep1&type=pdf
  *
  * @param x Input of the exponential function
  */
float aimath_f32_default_expf_fast(const float x);

/** @brief Sums up all values of a channel of the \link aimath_f32.h F32 \endlink tensor x
 *
 * Calculates the sum of all elements of each channel c. The result tensor is 1D:
 * @f[
 *  result_c = \sum_i(x_{ci})
 * @f
 *
 * @param x             F32 input tensor (N-D)
 * @param channel_axis  Index of the channel axis (negative values mean indexing from the end).
 * @param result        F32 result vector (1D)
 */
void aimath_f32_default_sum_channelwise(const aitensor_t *x, int8_t channel_axis, aitensor_t *result);

/** @brief Calculate the channel-wise mean values of the \link aimath_f32.h F32 \endlink tensor x
 *
 * Calculates the empirical mean for each channel of the given axis:\n
 * @f[
 *  means_i = \frac{1}{m} \sum_{j=1}^{m} x_{i,j}
 * @f]
 *
 * @param x             F32 input tensor (N-D)
 * @param channel_axis  Index of the channel axis (negative values mean indexing from the end)
 * @param result        F32 result vector (1D)
 */
void aimath_f32_default_mean_channelwise(const aitensor_t *x, int8_t channel_axis, aitensor_t *result);

/** @brief Calculate the channel-wise variances of the \link aimath_f32.h F32 \endlink tensor x
 *
 * Calculates the empirical variance for each channel of the given axis:\n
 * @f[
 *  variances_i = \frac{1}{m} \sum_{j=1}^{m} (x_{i,j} - \mu_i)^2
 * @f]
 *
 * @param x             F32 input tensor (N-D)
 * @param channel_axis  Index of the channel axis (negative values mean indexing from the end)
 * @param means         F32 mean vector (1D) for variance calculation
 * @param result        F32 result vector (1D)
 */
void aimath_f32_default_variance_channelwise(const aitensor_t *x, int8_t channel_axis, const aitensor_t *means, aitensor_t *result);

/** @brief Perform an exponential moving average
 *
 * Updates the moving average with a new data point:\n
 * @f[
 *  average \leftarrow momentum \cdot average + (1 - momentum) \cdot newdata
 * @f]
 *
 * @param new_data  Input tensor (N-D) with the new data point.
 * @param momentum  aiscalar_t (float value) which controls the momentum of the average (range [0, 1]).
 * @param average   The average tensor (N-D) that is modified (input and output value),
 */
void aimath_f32_default_exponential_moving_average(const aitensor_t *new_data, const void *momentum, aitensor_t *average);

/** @brief Calculates the gradients of the mean squared error between the \link aimath_f32.h F32 \endlink predicted and the target data using a sum reduction
  *
  * @param *predicted_data  F32 matrix with the predicted or calculated values (2D tensor)
  * @param *target_data     F32 matrix with the target data / true values / labels (2D tensor, rows one-hot encoded)
  * @param *result          F32 tensor containing the gradients
  */
void aimath_f32_default_mse_gradients_sum(const aitensor_t *predicted, const aitensor_t *target, aitensor_t *result);

/** @brief Calculates the gradients of the mean squared error between the \link aimath_f32.h F32 \endlink predicted and the target data using a mean reduction
  *
  * @param *predicted_data  F32 matrix with the predicted or calculated values (2D tensor)
  * @param *target_data     F32 matrix with the target data / true values / labels (2D tensor, rows one-hot encoded)
  * @param *result          F32 tensor containing the gradients
  */
void aimath_f32_default_mse_gradients_mean(const aitensor_t *predicted, const aitensor_t *target, aitensor_t *result);

/** @brief Calculates the mean squared error between the \link aimath_f32.h F32 \endlink predicted and the target data using a sum reduction
  *
  * @param *predicted_data  F32 matrix with the predicted or calculated values (2D tensor)
  * @param *target_data     F32 matrix with the target data / true values / labels (2D tensor, rows one-hot encoded)
  * @param *result          Resulting loss (float)
  */
void aimath_f32_default_mse_loss_sum(const aitensor_t *predicted, const aitensor_t *target, void *result);

/** @brief Calculates the mean squared error between the \link aimath_f32.h F32 \endlink predicted and the target data using a mean reduction
  *
  * @param *predicted_data  F32 matrix with the predicted or calculated values (2D tensor)
  * @param *target_data     F32 matrix with the target data / true values / labels (2D tensor, rows one-hot encoded)
  * @param *result          Resulting loss (float)
  */
void aimath_f32_default_mse_loss_mean(const aitensor_t *predicted, const aitensor_t *target, void *result);

/** @brief Scales a \link aimath_f32.h F32 \endlink tensor by batch size (size of first dimension)
  *
  * @param *a               F32 tensor that is going to be scaled by its batch size
  * @param *result          Scaled F32 tensor
  */
void aimath_f32_default_scale_by_batch_size(const aitensor_t *a, aitensor_t *result);
#endif // AIMATH_F32_DEFAULT

