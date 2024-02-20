/**
 * \file basic/avr_pgm/aimath/aimath_q7_avr_pgm.h
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
 * \brief Math functions for \link aimath_q7.h Q7 \endlink data type using AVR pgmspace.h library for constant data storage
 *
 * These functions modify the default implementation of the Q7 math functions to work with parameters,
 * stored in the program memory of AVR controllers.
 *
 * The library [avr/pgmspace.h](https://www.nongnu.org/avr-libc/user-manual/pgmspace_8h.html) is required.
 */

#include "aifes_config.h"

#if __AVR__
#ifdef AIFES_WITH_AVR_PGM

#ifndef AIMATH_Q7_AVR_PGM_H
#define AIMATH_Q7_AVR_PGM_H

#include "basic/default/aimath/aimath_q7_default.h"

/** @brief Performs a matrix multiplication of \link aimath_q7.h Q7 \endlink matrices a and b and adds a \link aimath_q31.h Q31 \endlink vector c to each row
 *
 * The data of b and c and the quantization parameters of a, b, c and result must be defined constant in PROGMEM.
 *
 * Same functionality as aimath_f32_default_linear32().
 *
 * The addition of the horizontal vector c is performed via broadcast, i.e. element wise in each column
 * Mathematically this broadcast is equal to multiplying c with an vertical vector (with the same number of elements as c)
 * and adding the result to a * b.
 *
 * ** The quantization parameters of the vector c have to be {zero_point = 0, shift = a.shift + b.shift}! **
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
 * const aimath_q7_params_t a_params PROGMEM = {1, 0}; // {shift, zero point}
 * int8_t a_data[3*3] = { 2,  4,  6,
 *                        8, 10, 12,
 *                       14, 16, 18};
 * aitensor_t a = AITENSOR_2D_Q7(a_shape, &a_params, a_data);
 *
 * uint16_t b_shape[2] = {3, 2};
 * const aimath_q7_params_t b_params PROGMEM = {2, 0}; // {shift, zero point}
 * const int8_t b_data[3*2] PROGMEM = {4, 0,
 *                                     0, 4,
 *                                     0, 0};
 * aitensor_t b = AITENSOR_2D_Q7(b_shape, &b_params, b_data);
 *
 * uint16_t c_shape[2] = {1, 2};
 * const aimath_q31_params_t c_params PROGMEM = {3, 0}; // {shift, zero point}
 * const int32_t c_data[1*2] PROGMEM = {16, 40};
 * aitensor_t c = AITENSOR_2D_Q31(c_shape, &c_params, c_data);
 *
 * uint16_t result_shape[2] = {3, 2};
 * const aimath_q7_params_t result_params PROGMEM = {1, 0}; // {shift, zero point}
 * int8_t result_data[3*2];
 * aitensor_t result = AITENSOR_2D_Q7(result_shape, &result_params, result_data);
 *
 * aimath_q7_avr_pgm_linear32_1(&a, &b, &c, &result);
 * \endcode
 *
 * @param *a        Q7 matrix a (2D tensor of shape [N x K]) (quantization parameters const in PROGMEM)
 * @param *b        Q7 matrix b (2D tensor of shape [K x M]) (data and quantization parameters const in PROGMEM)
 * @param *c        Q31 vector c (2D tensor of shape [1 x M]) (data and quantization parameters const in PROGMEM)
 * @param *result   Resulting Q7 matrix (2D tensor of shape [N x M]) (quantization parameters const in PROGMEM)
 */
void aimath_q7_avr_pgm_linear32_1(const aitensor_t *a, const aitensor_t *b, const aitensor_t *c, aitensor_t *result);

/** @brief Performs a matrix multiplication of \link aimath_q7.h Q7 \endlink matrices a and b and adds a \link aimath_q31.h Q31 \endlink vector c to each row
 *
 * The data of b and c and the quantization parameters of b, c and result (not of a!) must be defined constant in PROGMEM.
 *
 * Same functionality as aimath_f32_default_linear32().
 *
 * The addition of the horizontal vector c is performed via broadcast, i.e. element wise in each column
 * Mathematically this broadcast is equal to multiplying c with an vertical vector (with the same number of elements as c)
 * and adding the result to a * b.
 *
 * ** The quantization parameters of the vector c have to be {zero_point = 0, shift = a.shift + b.shift}! **
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
 * aimath_q7_params_t a_params = {1, 0}; // {shift, zero point}
 * int8_t a_data[3*3] = { 2,  4,  6,
 *                        8, 10, 12,
 *                       14, 16, 18};
 * aitensor_t a = AITENSOR_2D_Q7(a_shape, &a_params, a_data);
 *
 * uint16_t b_shape[2] = {3, 2};
 * const aimath_q7_params_t b_params PROGMEM = {2, 0}; // {shift, zero point}
 * const int8_t b_data[3*2] PROGMEM = {4, 0,
 *                                     0, 4,
 *                                     0, 0};
 * aitensor_t b = AITENSOR_2D_Q7(b_shape, &b_params, b_data);
 *
 * uint16_t c_shape[2] = {1, 2};
 * const aimath_q31_params_t c_params PROGMEM = {3, 0}; // {shift, zero point}
 * const int32_t c_data[1*2] PROGMEM = {16, 40};
 * aitensor_t c = AITENSOR_2D_Q31(c_shape, &c_params, c_data);
 *
 * uint16_t result_shape[2] = {3, 2};
 * const aimath_q7_params_t result_params PROGMEM = {1, 0}; // {shift, zero point}
 * int8_t result_data[3*2];
 * aitensor_t result = AITENSOR_2D_Q7(result_shape, &result_params, result_data);
 *
 * aimath_q7_avr_pgm_linear32_2(&a, &b, &c, &result);
 * \endcode
 *
 * @param *a        Q7 matrix a (2D tensor of shape [N x K])
 * @param *b        Q7 matrix b (2D tensor of shape [K x M]) (data and quantization parameters const in PROGMEM)
 * @param *c        Q31 vector c (2D tensor of shape [1 x M]) (data and quantization parameters const in PROGMEM)
 * @param *result   Resulting Q7 matrix (2D tensor of shape [N x M]) (quantization parameters const in PROGMEM)
 */
void aimath_q7_avr_pgm_linear32_2(const aitensor_t *a, const aitensor_t *b, const aitensor_t *c, aitensor_t *result);


/** @brief Performs a matrix multiplication of \link aimath_q7.h Q7 \endlink matrices a and b (transposed) and adds a \link aimath_q31.h Q31 \endlink vector c to each row
 *
 * The data of b and c and the quantization parameters of a, b, c and result must be defined constant in PROGMEM.
 *
 * The b matrix has to be transposed.
 *
 * Same functionality as aimath_f32_default_linear32_bt().
 *
 * The addition of the horizontal vector c is performed via broadcast, i.e. element wise in each column
 * Mathematically this broadcast is equal to multiplying c with an vertical vector (with the same number of elements as c)
 * and adding the result to \f$ a * b^T \f$.
 *
 * **The quantization parameters of the vector c have to be {zero_point = 0, shift = a.shift + b.shift}!**
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
 * const aimath_q7_params_t a_params PROGMEM = {1, 0}; // {shift, zero point}
 * int8_t a_data[3*3] = { 2,  4,  6,
 *                        8, 10, 12,
 *                       14, 16, 18};
 * aitensor_t a = AITENSOR_2D_Q7(a_shape, &a_params, a_data);
 *
 * uint16_t b_shape[2] = {2, 3};
 * const aimath_q7_params_t b_params PROGMEM = {2, 0}; // {shift, zero point}
 * const int8_t b_data[2*3] PROGMEM = {4, 0, 0,
 *                                     0, 4, 0};
 * aitensor_t b = AITENSOR_2D_Q7(b_shape, &b_params, b_data);
 *
 * uint16_t c_shape[2] = {1, 2};
 * const aimath_q31_params_t c_params PROGMEM = {3, 0}; // {shift, zero point}
 * const int32_t c_data[1*2] PROGMEM = {16, 40};
 * aitensor_t c = AITENSOR_2D_Q31(c_shape, &c_params, c_data);
 *
 * uint16_t result_shape[2] = {3, 2};
 * const aimath_q7_params_t result_params PROGMEM = {1, 0}; // {shift, zero point}
 * int8_t result_data[3*2];
 * aitensor_t result = AITENSOR_2D_Q7(result_shape, &result_params, result_data);
 *
 * aimath_q7_avr_pgm_linear32_bt_1(&a, &b, &c, &result);
 * \endcode
 *
 * @param *a        Q7 matrix a (2D tensor of shape [N x K]) (quantization parameters const in PROGMEM)
 * @param *b        Q7 matrix b (2D tensor of shape [M x K]) (data and quantization parameters const in PROGMEM)
 * @param *c        Q31 vector c (2D tensor of shape [1 x M]) (data and quantization parameters const in PROGMEM)
 * @param *result   Resulting Q7 matrix (2D tensor of shape [N x M]) (quantization parameters const in PROGMEM)
 */
void aimath_q7_avr_pgm_linear32_bt_1(const aitensor_t *a, const aitensor_t *b, const aitensor_t *c, aitensor_t *result);

/** @brief Performs a matrix multiplication of \link aimath_q7.h Q7 \endlink matrices a and b and adds a \link aimath_q31.h Q31 \endlink vector c to each row
 *
 * The data of b and c and the quantization parameters of b, c and result (not of a!) must be defined constant in PROGMEM.
 *
 * The b matrix has to be transposed.
 *
 * Same functionality as aimath_f32_default_linear32().
 *
 * The addition of the horizontal vector c is performed via broadcast, i.e. element wise in each column
 * Mathematically this broadcast is equal to multiplying c with an vertical vector (with the same number of elements as c)
 * and adding the result to \f$ a * b^T \f$.
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
 * \code{.c}
 * uint16_t a_shape[2] = {3, 3};
 * aimath_q7_params_t a_params = {1, 0}; // {shift, zero point}
 * int8_t a_data[3*3] = { 2,  4,  6,
 *                        8, 10, 12,
 *                       14, 16, 18};
 * aitensor_t a = AITENSOR_2D_Q7(a_shape, &a_params, a_data);
 *
 * uint16_t b_shape[2] = {2, 3};
 * const aimath_q7_params_t b_params PROGMEM = {2, 0}; // {shift, zero point}
 * const int8_t b_data[2*3] PROGMEM = {4, 0, 0,
 *                                     0, 4, 0};
 * aitensor_t b = AITENSOR_2D_Q7(b_shape, &b_params, b_data);
 *
 * uint16_t c_shape[2] = {1, 2};
 * const aimath_q31_params_t c_params PROGMEM = {3, 0}; // {shift, zero point}
 * const int32_t c_data[1*2] PROGMEM = {16, 40};
 * aitensor_t c = AITENSOR_2D_Q31(c_shape, &c_params, c_data);
 *
 * uint16_t result_shape[2] = {3, 2};
 * const aimath_q7_params_t result_params PROGMEM = {1, 0}; // {shift, zero point}
 * int8_t result_data[3*2];
 * aitensor_t result = AITENSOR_2D_Q7(result_shape, &result_params, result_data);
 *
 * aimath_q7_avr_pgm_linear32_bt_2(&a, &b, &c, &result);
 * \endcode
 *
 * @param *a        Q7 matrix a (2D tensor of shape [N x K])
 * @param *b        Q7 matrix b (2D tensor of shape [M x K]) (data and quantization parameters const in PROGMEM)
 * @param *c        Q31 vector c (2D tensor of shape [1 x M]) (data and quantization parameters const in PROGMEM)
 * @param *result   Resulting Q7 matrix (2D tensor of shape [N x M]) (quantization parameters const in PROGMEM)
 */
void aimath_q7_avr_pgm_linear32_bt_2(const aitensor_t *a, const aitensor_t *b, const aitensor_t *c, aitensor_t *result);

/** @brief Calculates the rectifier (ReLU) value of each element in a \link aimath_q7.h Q7 \endlink tensor
 *
 * The quantization parameters x must be defined constant in PROGMEM.
 *
 * This function wraps the function aimath_f32_default_relu() internally.
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
 * const aimath_q7_params_t x_params PROGMEM = {1, 0}; // {shift, zero point}
 * int8_t x_data[2*3] = {  2,  -4,   6,
 *                        -8,  10, -12};
 * aitensor_t x = AITENSOR_2D_Q7(x_shape, &x_params, x_data);
 *
 * uint16_t result_shape[2] = {2, 3};
 * aimath_q7_params_t result_params;
 * int8_t result_data[2*3];
 * aitensor_t result = AITENSOR_2D_Q7(result_shape, &result_params, result_data);
 *
 * aimath_q7_avr_pgm_relu(&x, &result);
 *
 * print_aitensor(&result);
 * \endcode
 *
 * @param *x       Q7 tensor to calculate the ReLU from (N-D tensor) (quantization parameters const in PROGMEM)
 * @param *result  Resulting Q7 tensor (N-D tensor)
 */
void aimath_q7_avr_pgm_relu(const aitensor_t *x, aitensor_t *result);

/** @brief Calculates the leaky rectifier (leaky ReLU) value of each element in a \link aimath_q7.h Q7 \endlink tensor
 *
 * The quantization parameters x must be defined constant in PROGMEM.
 *
 * This function wraps the function aimath_f32_default_leaky_relu() internally.
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
 * const aimath_q7_params_t x_params PROGMEM = {6, 0}; // {shift, zero point}
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
 * aimath_q7_avr_pgm_leaky_relu(&x, &alpha, &result);
 *
 * print_aitensor(&result);
 * \endcode
 *
 * @param *x        Q7 tensor to calculate the leaky ReLU from (N-D tensor) (quantization parameters const in PROGMEM)
 * @param *alpha    Scalar \f$ \alpha \f$ (type aiscalar_q7_t) for the leakage
 * @param *result   Resulting Q7 tensor (N-D tensor)
 */
void aimath_q7_avr_pgm_leaky_relu(const aitensor_t *x, const void *alpha, aitensor_t *result);

/** @brief Calculates the exponential rectifier (ELU) value of each element in a \link aimath_q7.h Q7 \endlink tensor
 *
 * The quantization parameters x must be defined constant in PROGMEM.
 *
 * This function wraps the function aimath_f32_default_elu() internally.
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
 * const aimath_q7_params_t x_params PROGMEM = {1, 0}; // {shift, zero point}
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
 * aimath_q7_avr_pgm_elu(&x, &alpha, &result);
 *
 * print_aitensor(&result);
 * \endcode
 *
 * @param *x        Q7 tensor to calculate the ELU from (N-D tensor) (quantization parameters const in PROGMEM)
 * @param *alpha    Scalar \f$ \alpha \f$ (type aiscalar_q7_t)
 * @param *result   Resulting Q7 tensor (N-D tensor)
 */
void aimath_q7_avr_pgm_elu(const aitensor_t *x, const void *alpha, aitensor_t *result);


/** @brief Calculates the sigmoid of each element in a \link aimath_q7.h Q7 \endlink tensor
 *
 * The quantization parameters x must be defined constant in PROGMEM.
 *
 * This function wraps the function aimath_f32_default_sigmoid() internally.
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
 * const aimath_q7_params_t x_params PROGMEM = {1, 0}; // {shift, zero point}
 * int8_t x_data[2*3] = {  2,  -4,   6,
 *                        -8,  10, -12};
 * aitensor_t x = AITENSOR_2D_Q7(x_shape, &x_params, x_data);
 *
 * uint16_t result_shape[2] = {2, 3};
 * aimath_q7_params_t result_params;
 * int8_t result_data[2*3];
 * aitensor_t result = AITENSOR_2D_Q7(result_shape, &result_params, result_data);
 *
 * aimath_q7_avr_pgm_sigmoid(&x, &result);
 *
 * print_aitensor(&result);
 * \endcode
 *
 * @see Sigmoid PLAN: https://www.researchgate.net/figure/Comparative-representation-of-the-sigmoid-function-and-PLAN-approximation_fig7_228618304
 *
 * @param *x       Q7 tensor to calculate the sigmoid from (N-D tensor) (quantization parameters const in PROGMEM)
 * @param *result  Resulting Q7 tensor (N-D tensor)
 */
void aimath_q7_avr_pgm_sigmoid(const aitensor_t *x, aitensor_t *result);

/** @brief Calculates the tanh of each element in a \link aimath_q7.h Q7 \endlink tensor
 *
 * The quantization parameters x must be defined constant in PROGMEM.
 *
 * This function wraps the function aimath_f32_default_tanh() internally.
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
 * const aimath_q7_params_t x_params PROGMEM= {1, 0}; // {shift, zero point}
 * int8_t x_data[2*3] = {  2,  -4,   6,
 *                        -8,  10, -12};
 * aitensor_t x = AITENSOR_2D_Q7(x_shape, &x_params, x_data);
 *
 * uint16_t result_shape[2] = {2, 3};
 * aimath_q7_params_t result_params;
 * int8_t result_data[2*3];
 * aitensor_t result = AITENSOR_2D_Q7(result_shape, &result_params, result_data);
 *
 * aimath_q7_avr_pgm_tanh(&x, &result);
 *
 * print_aitensor(&result);
 * \endcode
 *
 * @see Sigmoid PLAN: https://www.researchgate.net/figure/Comparative-representation-of-the-sigmoid-function-and-PLAN-approximation_fig7_228618304
 *
 * @param *x        Q7 tensor to calculate the tanh from (N-D tensor) (quantization parameters const in PROGMEM)
 * @param *result   Resulting Q7 tensor (N-D tensor)
 */
void aimath_q7_avr_pgm_tanh(const aitensor_t *x, aitensor_t *result);

/** @brief Calculates the softsign value of each element in a \link aimath_q7.h Q7 \endlink tensor
 *
 * The quantization parameters x must be defined constant in PROGMEM.
 *
 * This function wraps the function aimath_f32_default_softsign() internally.
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
 * const aimath_q7_params_t x_params PROGMEM = {1, 0}; // {shift, zero point}
 * int8_t x_data[2*3] = {  2,  -4,   6,
 *                        -8,  10, -12};
 * aitensor_t x = AITENSOR_2D_Q7(x_shape, &x_params, x_data);
 *
 * uint16_t result_shape[2] = {2, 3};
 * aimath_q7_params_t result_params;
 * int8_t result_data[2*3];
 * aitensor_t result = AITENSOR_2D_Q7(result_shape, &result_params, result_data);
 *
 * aimath_q7_avr_pgm_softsign(&x, &result);
 *
 * print_aitensor(&result);
 * \endcode
 *
 * @param *x       Q7 tensor to calculate the softsign from (N-D tensor) (quantization parameters const in PROGMEM)
 * @param *result  Resulting Q7 tensor (N-D tensor)
 */
void aimath_q7_avr_pgm_softsign(const aitensor_t *x, aitensor_t *result);

/** @brief Calculates the softmax value of each batch element (row) of a \link aimath_q7.h Q7 \endlink tensor
 *
 * The quantization parameters x must be defined constant in PROGMEM.
 *
 * This function wraps the function aimath_f32_default_softmax() internally.
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
 * const aimath_q7_params_t x_params PROGMEM = {1, 0}; // {shift, zero point}
 * int8_t x_data[2*3] = {  2,  -4,   6,
 *                        -8,  10, -12};
 * aitensor_t x = AITENSOR_2D_Q7(x_shape, &x_params, x_data);
 *
 * uint16_t result_shape[2] = {2, 3};
 * aimath_q7_params_t result_params;
 * int8_t result_data[2*3];
 * aitensor_t result = AITENSOR_2D_Q7(result_shape, &result_params, result_data);
 *
 * aimath_q7_avr_pgm_softmax(&x, &result);
 *
 * print_aitensor(&result);
 * \endcode
 *
 * @param *x       Q7 tensor to calculate the softmax from (N-D tensor) (quantization parameters const in PROGMEM)
 * @param *result  Resulting Q7 tensor (N-D tensor)
 */
void aimath_q7_avr_pgm_softmax(const aitensor_t *x, aitensor_t *result);

#endif // AIMATH_Q7_AVR_PGM_H

#endif // AIFES_WITH_AVR_PGM
#endif // __AVR__
