
/**
 * \file basic/default/ailayer/ailayer_leaky_relu_default.h
 * \internal
 * \date 16.03.2021
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
 * \brief Default implementation of the \link ailayer_leaky_relu.h Leaky ReLU layer \endlink
 *
 * Hardware independent implementations of the Leaky ReLU layer in \link aimath_f32.h F32 \endlink data-type.
 * For more information about the Leaky ReLU layer refer to ailayer_leaky_relu.h.
 */

#ifndef AILAYER_LEAKY_RELU_DEFAULT
#define AILAYER_LEAKY_RELU_DEFAULT

#include "basic/base/ailayer/ailayer_leaky_relu.h"

#include "basic/default/aimath/aimath_f32_default.h"
#include "basic/default/aimath/aimath_q31_default.h"
#include "basic/default/aimath/aimath_q7_default.h"

#define AILAYER_LEAKY_RELU_F32_A(alpha)         {{{0,},},alpha}
#define AILAYER_LEAKY_RELU_F32_M(alpha)         {{{0,},},alpha}
#define AILAYER_LEAKY_RELU_Q31_M(alpha)         {{{0,},},alpha}
#define AILAYER_LEAKY_RELU_Q31_A(alpha)         {{{0,},},alpha}
#define AILAYER_LEAKY_RELU_Q7_M(alpha)          {{{0,},},alpha}
#define AILAYER_LEAKY_RELU_Q7_A(alpha)          {{{0,},},alpha}

typedef struct ailayer_leaky_relu_f32 	ailayer_leaky_relu_f32_t;
typedef struct ailayer_leaky_relu_q31 	ailayer_leaky_relu_q31_t;
typedef struct ailayer_leaky_relu_q7 	ailayer_leaky_relu_q7_t;

/** @brief Data-type specific Leaky ReLU layer struct for \link aimath_f32.h F32 \endlink
 *
 * Adds a data field for the constant alpha in \link aimath_f32.h F32 \endlink to the base implementation.
 */
struct ailayer_leaky_relu_f32 {
	ailayer_leaky_relu_t base; /**< Inherited field members from general ailayer_leaky_relu struct. */
	aiscalar_f32_t alpha; /**< Data type-specific parameter used to calculate Leaky ReLU function for input values < 0. */
};

/** @brief Data-type specific Leaky ReLU layer struct for \link aimath_q31.h Q31 \endlink
 *
 * Adds a data field for the constant alpha in \link aimath_q31.h Q31 \endlink to the base implementation.
 */
struct ailayer_leaky_relu_q31 {
	ailayer_leaky_relu_t base; /**< Inherited field members from general ailayer_leaky_relu struct. */
	aiscalar_q31_t alpha; /**< Data type-specific parameter used to calculate Leaky ReLU function for input values < 0. */
};

/** @brief Data-type specific Leaky ReLU layer struct for \link aimath_q7.h Q7 \endlink
 *
 * Adds a data field for the constant alpha in \link aimath_q7.h Q7 \endlink to the base implementation.
 */
struct ailayer_leaky_relu_q7 {
	ailayer_leaky_relu_t base; /**< Inherited field members from general ailayer_leaky_relu struct. */
	aiscalar_q7_t alpha; /**< Data type-specific parameter used to calculate Leaky ReLU function for input values < 0. */
};

/** @brief Initializes and connect a \link ailayer_leaky_relu.h Leaky ReLU layer \endlink with the \link aimath_f32.h F32 \endlink default implementation
 *
 * **Example:** Create the layer structure:\n
 * In C:
 * \code{.c}
 * ailayer_leaky_relu_f32_t leaky_relu_layer = {
 *     .alpha = 0.01f
 * };
 * \endcode
 * In C, C++ and on Arduino:
 * \code{.c}
 * ailayer_leaky_relu_f32_t leaky_relu_layer = AILAYER_LEAKY_RELU_F32_A(0.01f);
 * \endcode
 *
 * **Example:** Initialize and connect the layer:\n
 * \code{.c}
 * x = ailayer_leaky_relu_f32_default(&leaky_relu_layer, x);
 * \endcode
 *
 * @param *layer the layer structure to be initialized
 * @param *input_layer the prior layer that provides the input to this Leaky ReLU layer
 * @return the initialized Leaky ReLU layer structure
 */
ailayer_t *ailayer_leaky_relu_f32_default(ailayer_leaky_relu_f32_t *layer, ailayer_t *input_layer);

/** @brief Initializes and connect a \link ailayer_leaky_relu.h Leaky ReLU layer \endlink with the \link aimath_q31.h Q31 \endlink default implementation
 *
 * **The quantization parameters of the result tensor are automatically set to {shift = input_layer.result.shift, zero_point = input_layer.result.zero_point}
 * because the output values are in the interval (alpha * min(input_layer.result), max(input_layer.result)].**
 *
 * **Example:** Create the layer structure:\n
 * In C:
 * \code{.c}
 * ailayer_leaky_relu_q31_t leaky_relu_layer = {
 *     .alpha = AISCALAR_Q31(0.01f, 25, 0)
 * };
 * \endcode
 * In C, C++ and on Arduino:
 * \code{.c}
 * ailayer_leaky_relu_q31_t leaky_relu_layer = AILAYER_LEAKY_RELU_Q31_M(AISCALAR_Q31(0.01f, 25, 0));
 * \endcode
 *
 * **Example:** Initialize and connect the layer:\n
 * \code{.c}
 * x = ailayer_leaky_relu_q31_default(&leaky_relu_layer, x);
 * \endcode
 *
 * @param *layer        The layer structure to initialize.
 * @param *input_layer  The prior layer.
 * @return              The (successfully) initialized layer structure.
 */
ailayer_t *ailayer_leaky_relu_q31_default(ailayer_leaky_relu_q31_t *layer, ailayer_t *input_layer);

/** @brief Initializes and connect a \link ailayer_leaky_relu.h Leaky ReLU layer \endlink with the \link aimath_q7.h Q7 \endlink default implementation
 *
 * **The quantization parameters of the result tensor are automatically set to {shift = input_layer.result.shift, zero_point = input_layer.result.zero_point}
 * because the output values are in the interval (alpha * min(input_layer.result), max(input_layer.result)].**
 *
 * **Example:** Create the layer structure of a pre-trained model (qparams must be set manually):\n
 * In C:
 * \code{.c}
 * ailayer_leaky_relu_q7_t leaky_relu_layer = {
 *     .alpha = AISCALAR_Q7(0.01f, 7, 0)
 * };
 * \endcode
 * In C, C++ and on Arduino:
 * \code{.c}
 * ailayer_leaky_relu_q7_t leaky_relu_layer = AILAYER_LEAKY_RELU_Q7_M(AISCALAR_Q7(0.01f, 7, 0));
 * \endcode
 *
 * **Example:** Initialize and connect the layer:\n
 * \code{.c}
 * x = ailayer_leaky_relu_q7_default(&leaky_relu_layer, x);
 * \endcode
 *
 * @param *layer        The layer structure to initialize.
 * @param *input_layer  The prior layer.
 * @return              The (successfully) initialized layer structure.
 */
ailayer_t *ailayer_leaky_relu_q7_default(ailayer_leaky_relu_q7_t *layer, ailayer_t *input_layer);

/** @brief Calculate and set the quantization parameters for the result tensor of the Leaky ReLU \link aimath_q31.h Q31 \endlink default implementation
 *
 * *Implementation of ailayer.calc_result_tensor_params.*
 *
 * The quantization parameters are pre-defined and the same as in the corresponding math function aimath_q31_default_leaky_relu().
 *
 * @param *self     The layer structure
 */
void ailayer_leaky_relu_calc_result_tensor_params_q31_default(ailayer_t *self);

/** @brief Calculate and set the quantization parameters for the result tensor of the Leaky ReLU \link aimath_q7.h Q7 \endlink default implementation
 *
 * *Implementation of ailayer.calc_result_tensor_params.*
 *
 * The quantization parameters are pre-defined and the same as in the corresponding math function aimath_q7_default_leaky_relu().
 *
 * @param *self     The layer structure
 */
void ailayer_leaky_relu_calc_result_tensor_params_q7_default(ailayer_t *self);

#endif // AILAYER_LEAKY_RELU_DEFAULT
