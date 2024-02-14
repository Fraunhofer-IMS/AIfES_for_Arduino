/**
 * \file basic/base/aimath/aimath_q31.h
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
 * \brief 	Definition of the Q31 (aiq31) data-type
 *
 * The Q31 (aiq31) data-type stores data as quantized 32 bit integer values.
 * In addition a scaling factor (shift) and a zero point (zero_point) is needed to fully define the value.
 *
 * The Q31 quantization is a asymmetric 32 bit integer quantization that allows integer-only calculations on real values.
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
 * **Example: Create a Q31 tensor**\n
 * The tensor
 * @f[
 * \left( \begin{array}{rrr} 0 & 1 & 2 \\ 3 & 4 & 5 \end{array}\right)
 * @f]
 * can be created with
 * In C:
 * \code{.c}
 * int32_t example_data[] = {0, 1, 2
 *                           3, 4, 5};
 * uint16_t example_shape[] = {2, 3};
 * aimath_q31_params_t example_q_params = { .shift = 0, .zero_point = 0 };
 * aitensor_t example_tensor = {
 *     .dtype = aiq31,
 *     .dim = 2,
 *     .shape = example_shape,
 *     .tensor_params = example_q_params,
 *     .data = example_data
 * };
 * \endcode
 * In C, C++ and on Arduino:
 * \code{.c}
 * int32_t example_data[] = {0, 1, 2
 *                           3, 4, 5};
 * uint16_t example_shape[] = {2, 3};
 * aimath_q31_params_t example_q_params = { .shift = 0, .zero_point = 0 };
 * aitensor_t example_tensor = AITENSOR_2D_Q31(example_shape, &example_q_params, example_data);
 * \endcode
 *
 * **Example: Create a Q31 scalar**\n
 * Either create it with the helper macro
 * \code{.c}
 * aiscalar_q31_t scalar = AISCALAR_Q31(42.0f, 16, 0);
 * \endcode
 * or manual with
 * \code{.c}
 * aiscalar_q31_t scalar = { .value = 2552512, .shift = 16, .zero_point = 0};
 * \endcode
 *
 * **Example: Print a Q31 tensor to the console**
 * \code{.c}
 * print_aitensor(&example_tensor);
 * \endcode
 *
 * **Example: Print a Q31 scalar to the console**
 * \code{.c}
 * print_aiscalar(&scalar, aiq31);
 * \endcode
 */

#ifndef AIMATH_Q31
#define AIMATH_Q31

#include "core/aifes_math.h"
#include "basic/base/aimath/aimath_basic.h"

/** @brief Initialize a 2 dimensional Q31 tensor
 *
 * @param shape     A uint16_t array of length 2 for the shape
 * @param params    A aimath_q32_params_t struct with the quantization parameters
 * @param data      A int32_t array for the tensor data
 */
#define AITENSOR_2D_Q31(shape, params, data)    {aiq31, 2, shape, params, data}


/** @brief Convert a quantized Q31 value into a float value, with given scaling shift factor and zero point
 *
 * @f[
 *   F = 2^{-S} * (Q - Z)
 * @f]
 *
 * @param Q Q31 quantized value
 * @param S Shift used for the scaling factor
 * @param Z Zero point
 *
 */
#define Q31_TO_FLOAT(Q, S, Z)		((float) ((int64_t)(Q) - (int64_t)(Z)) / (float) ((int64_t) 1ULL << (S)))

/** @brief Convert a float value into a quantized Q31 value, with given scaling shift factor and zero point
 *
 *	@f[
 *   Q = round(F \cdot 2^S) + Z
 * @f]
 *
 * @param F Float value
 * @param S Shift used for the scaling factor
 * @param Z Zero point
 */
#define FLOAT_TO_Q31(F, S, Z)       ((int32_t) ((int64_t)((F) * (1ULL << (S)) + ((F) >= 0 ? 0.5f : -0.5f)) + (int64_t)(Z)))

/** Initialize a scalar variable of Q31 data-type with a float value, with given scaling shift factor and zero point
 *
 * @f[
 *   Q = round(F \cdot 2^S) + Z
 * @f]
 *
 * Example:\n
 * \code{.c}
 * aiscalar_q31_t scalar = AISCALAR_Q31(0.9f, 27, 0);
 * \endcode
 *
 * @param F Float value
 * @param S Shift used for the scaling factor
 * @param Z Zero point
 */
#define AISCALAR_Q31(F, S, Z)       {FLOAT_TO_Q31(F, S, Z), S, Z}

typedef struct aimath_q31_params	aimath_q31_params_t;
typedef struct aiscalar_q31			aiscalar_q31_t;

/** @brief Parameters used for the quantized \link aimath_q31.h Q31 \endlink values, used as property of a tensor
 */
struct aimath_q31_params {
	uint16_t shift; /**< The scaling factor \f$ s \f$ of the quantization (The total scale is calculated with \f$ 2^{-s} \f$) */
	int32_t zero_point; /**< The zero point \f$ z \f$ of the quantization */
};

/** @brief Single quantized \link aimath_q31.h Q31 \endlink value/scalar
 */
struct aiscalar_q31 {
	int32_t value; /**< Quantized value \f$ q \f$ */
	uint16_t shift; /**< The scaling factor \f$ s \f$ of the quantization (The total scale is calculated with \f$ 2^{-s} \f$) */
	int32_t zero_point; /**< The zero point \f$ z \f$ of the quantization */
};

/** @brief Printing a Q31 tensor to console
 *
 * For users the function
 * \code{.c}
 * print_aitensor(&tensor);
 * \endcode
 * is prefered.
 *
 * @param *tensor	The tensor to print.
 */
void aimath_q31_print_aitensor(const aitensor_t *tensor);

/** @brief Printing a Q31 scalar to console
 *
 * For users the function
 * \code{.c}
 * print_aiscalar(&scalar, aiq31);
 * \endcode
 * is prefered.
 *
 * @param *scalar	The scalar (type: aiscalar_q31_t) to print.
 */
void aimath_q31_print_aiscalar(const void *scalar);

/** @brief Calculates the aimath_q31_params parameters
 *
 * @details Given a minimum and a maximum value, this function calculates the
 * 			scaling shift and zero shift parameters for aimath_q31_params
 *
 * @param min_value	Minimum value of a given range
 * @param max_value	Maximum value of a given range
 * @param *q_params	Pointer to write the results aimath_q31_params::zero_point and aimath_q31_params::shift into
 */
void aimath_q31_calc_q_params_from_f32(float min_value, float max_value, aimath_q31_params_t *q_params);

/** @brief Converts a float f32 tensor into a quantized q31 tensor
 *
 * @details Converts a float f32 tensor into a quantized q31 tensor by applying the aimath_q31_params_t given
 *			in the quantized q31 tensor
 *
 * @param *tensor_f32	Float f32 tensor to convert from
 * @param *tensor_q31	quantized q31 tensor to convert to, included aimath_q31_params must be set beforehand
 */
void aimath_q31_quantize_tensor_from_f32(const aitensor_t *tensor_f32, aitensor_t *tensor_q31);

/** @brief The Q31 data-type indicator
 *
 * Use this variable to configure some element with the \link aimath_q31.h Q31 \endlink data-type,
 */
extern const aimath_dtype_t *aiq31;

#endif // AIMATH_Q31
