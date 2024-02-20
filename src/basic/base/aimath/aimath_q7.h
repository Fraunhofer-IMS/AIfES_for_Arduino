/**
 * \file basic/base/aimath/aimath_q7.h
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
 * \brief 	Definition of the Q7 (aiq7) data-type
 *
 * The Q7 (aiq7) data-type stores data as quantized 8 bit integer values.
 * In addition a scaling factor (shift) and a zero point (zero_point) is needed to fully define the value.
 *
 * The Q7 quantization is an asymmetric 8 bit integer quantization that allows integer-only calculations on real values.
 * A real value \f$ r \f$ is represented by an integer value \f$ q \f$, the scaling factor / shift \f$ s \f$ and the zero point \f$ z \f$
 * according to the following formula:
 * @f[
 *		r = 2^{-s} * (q - z)
 * @f]
 *
 * To get the quantized value \f$ q \f$ out of a real value you have to calculate
 * @f[
 *		q = round(\frac{x}{2^{-s}} + z)
 * @f]
 *
 * **Example: Create a Q7 tensor**\n
 * The tensor
 * @f[
 * \left( \begin{array}{rrr} 0 & 1 & 2 \\ 3 & 4 & 5 \end{array}\right)
 * @f]
 * can be created with
 * In C:
 * \code{.c}
 * int8_t example_data[] = {0, 1, 2
 *                          3, 4, 5};
 * uint16_t example_shape[] = {2, 3};
 * aimath_q7_params_t example_q_params = { .shift = 0, .zero_point = 0 };
 * aitensor_t example_tensor = {
 *     .dtype = aiq7,
 *     .dim = 2,
 *     .shape = example_shape,
 *     .tensor_params = example_q_params,
 *     .data = example_data
 * };
 * \endcode
 * In C, C++ and on Arduino:
 * \code{.c}
 * int8_t example_data[] = {0, 1, 2
 *                          3, 4, 5};
 * uint16_t example_shape[] = {2, 3};
 * aimath_q7_params_t example_q_params = { .shift = 0, .zero_point = 0 };
 * aitensor_t example_tensor = AITENSOR_2D_Q7(example_shape, &example_q_params, example_data);
 * \endcode
 *
 * **Example: Create a Q7 scalar**\n
 * Either create it with the helper macro
 * \code{.c}
 * aiscalar_q7_t scalar = AISCALAR_Q7(4.2f, 4, 0);
 * \endcode
 * or manually with
 * \code{.c}
 * aiscalar_q7_t scalar = { .value = 67, .shift = 4, .zero_point = 0};
 * \endcode
 *
 * **Example: Print a Q7 tensor to the console**
 * \code{.c}
 * print_aitensor(&example_tensor);
 * \endcode
 *
 * **Example: Print a Q7 scalar to the console**
 * \code{.c}
 * print_aiscalar(&scalar, aiq7);
 * \endcode
 */

#ifndef AIMATH_Q7
#define AIMATH_Q7

#include "core/aifes_math.h"
#include "basic/base/aimath/aimath_basic.h"

/** @brief Initialize a 2 dimensional Q7 tensor
 *
 * @param shape     A uint16_t array of length 2 for the shape
 * @param params    A aimath_q7_params_t struct with the quantization parameters
 * @param data      A int8_t array for the tensor data
 */
#define AITENSOR_2D_Q7(shape, params, data)    {aiq7, 2, shape, params, data}

/** @brief Convert a quantized Q7 value into a float value, with given scaling shift factor and zero point
 *
 * @f[
 *   F = 2^{-S} * (Q - Z)
 * @f]
 *
 * @param Q Q7 quantized value
 * @param S Shift used for the scaling factor
 * @param Z Zero point
 *
 */
#define Q7_TO_FLOAT(Q, S, Z)		((float) ((int16_t)(Q) - (int16_t)(Z)) / (float) (1 << (S)))

/** @brief Convert a float value into a quantized Q7 value, with given scaling shift factor and zero point
 *
 *	@f[
 *   Q = round(F \cdot 2^S) + Z
 * @f]
 *
 * @param F Float value
 * @param S Shift used for the scaling factor
 * @param Z Zero point
 */
#define FLOAT_TO_Q7(F, S, Z)		((int8_t) ((int16_t)((F) * (1 << (S)) + ((F) >= 0 ? 0.5f : -0.5f)) + (int16_t)(Z)))

/** Initialize a scalar variable of Q7 data-type with a float value, with given scaling shift factor and zero point
 *
 * @f[
 *   Q = round(F \cdot 2^S) + Z
 * @f]
 *
 * Example:\n
 * \code{.c}
 * aiscalar_q7_t scalar = AISCALAR_Q7(0.9f, 6, 0);
 * \endcode
 *
 * @param F Float value
 * @param S Shift used for the scaling factor
 * @param Z Zero point
 */
#define AISCALAR_Q7(F, S, Z)       {FLOAT_TO_Q7(F, S, Z), S, Z}

typedef struct aimath_q7_params	aimath_q7_params_t;
typedef struct aiscalar_q7	aiscalar_q7_t;

/** @brief Parameters used for the quantized \link aimath_q7.h Q7 \endlink values, used as property of a tensor
 */
struct aimath_q7_params {
	uint16_t shift;  /**< The scaling factor \f$ s \f$ of the quantization (The total scale is calculated with \f$ 2^{-s} \f$) */
	int8_t zero_point; /**< The zero point \f$ z \f$ of the quantization */
};

/** @brief Single quantized \link aimath_q7.h Q7 \endlink value/scalar
 */
struct aiscalar_q7 {
	int8_t value; /**< Quantized value \f$ q \f$ */
	uint16_t shift; /**< The scaling factor \f$ s \f$ of the quantization (The total scale is calculated with \f$ 2^{-s} \f$) */
	int8_t zero_point; /**< The zero point \f$ z \f$ of the quantization */
};

/** @brief Printing a Q7 tensor to console
 *
 * For users the function
 * \code{.c}
 * print_aitensor(&tensor);
 * \endcode
 * is prefered.
 *
 * @param *tensor	The tensor to print.
 */
void aimath_q7_print_aitensor(const aitensor_t *tensor);

/** @brief Printing a Q7 scalar to console
 *
 * For users the function
 * \code{.c}
 * print_aiscalar(&scalar, aiq7);
 * \endcode
 * is prefered.
 *
 * @param *scalar	The scalar (type: aiscalar_q7_t) to print.
 */
void aimath_q7_print_aiscalar(const void *scalar);

/** @brief Calculates the aimath_q7_params parameters
 *
 * @details Given a minimum and a maximum value, this function calculates the
 * 			scaling shift and zero shift parameters for aimath_q7_params
 *
 * @param min_value	Minimum value of a given range
 * @param max_value	Maximum value of a given range
 * @param *q_params	Pointer to write the results aimath_q7_params::zero_point and aimath_q7_params::shift into
 */
void aimath_q7_calc_q_params_from_f32(float min_value, float max_value, aimath_q7_params_t *q_params);

/** @brief Converts a float f32 tensor into a quantized q7 tensor
 *
 * @details Converts a float f32 tensor into a quantized q7 tensor by applying the aimath_q7_params_t given
 *			in the quantized q7 tensor
 *
 * @param *tensor_f32	Float f32 tensor to convert from
 * @param *tensor_q7	quantized q7 tensor to convert to, included aimath_q7_params must be set beforehand
 */
void aimath_q7_quantize_tensor_from_f32(const aitensor_t *tensor_f32, aitensor_t *tensor_q7);

/** @brief The Q7 data-type indicator
 *
 * Use this variable to configure some element with the \link aimath_q7.h Q7 \endlink data-type,
 */
extern const aimath_dtype_t *aiq7;

#endif // AIMATH_Q7
