
/**
 * \file basic/default/ailayer/ailayer_tanh_default.h
 * \internal
 * \date 17.03.2021
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
 * \brief Default implementation of the \link ailayer_tanh.h Tanh layer \endlink
 *
 * Hardware independent implementations of the Tanh layer in \link aimath_f32.h F32 \endlink data-type.
 * For more information about the Tanh layer refer to ailayer_tanh.h.
 */

#ifndef AILAYER_TANH_DEFAULT
#define AILAYER_TANH_DEFAULT

#include "basic/base/ailayer/ailayer_tanh.h"

#include "basic/default/aimath/aimath_f32_default.h"
#include "basic/default/aimath/aimath_q31_default.h"
#include "basic/default/aimath/aimath_q7_default.h"

#define AILAYER_TANH_F32_M()        {{0,}}
#define AILAYER_TANH_F32_A()        {{0,}}
#define AILAYER_TANH_Q31_M()        {{0,}}
#define AILAYER_TANH_Q31_A()        {{0,}}
#define AILAYER_TANH_Q7_M()         {{0,}}
#define AILAYER_TANH_Q7_A()         {{0,}}

typedef struct ailayer_tanh 	ailayer_tanh_f32_t;
typedef struct ailayer_tanh 	ailayer_tanh_q31_t;
typedef struct ailayer_tanh 	ailayer_tanh_q7_t;

/** @brief Initializes and connect an \link ailayer_tanh.h Tanh layer \endlink with the \link aimath_f32.h F32 \endlink default implementation
 *
 * **Example:** Create the layer structure:\n
 * \code{.c}
 * ailayer_tanh_f32_t tanh_layer;
 * \endcode
 * or
 * \code{.c}
 * ailayer_tanh_f32_t tanh_layer = AILAYER_TANH_F32_A();
 * \endcode
 *
 * **Example:** Initialize and connect the layer:\n
 * \code{.c}
 * x = ailayer_tanh_f32_default(&tanh_layer, x);
 * \endcode
 *
 * Initializes the tanh layer structure with the float 32 default implementation
 *
 * @param *layer        The layer structure to initialize.
 * @param *input_layer  The prior layer.
 * @return              The (successfully) initialized layer structure.
 */
ailayer_t *ailayer_tanh_f32_default(ailayer_tanh_f32_t *layer, ailayer_t *input_layer);

/** @brief Initializes and connect a \link ailayer_tanh.h Tanh layer \endlink with the \link aimath_q31.h Q31 \endlink default implementation
 *
 * **The quantization parameters of the result tensor are automatically set to {shift = 31, zero_point = 0}
 * because the output values are in the interval (-1, 1).**
 *
 * **Example:** Create the layer structure:\n
 * \code{.c}
 * ailayer_tanh_q31_t tanh_layer;
 * \endcode
 * or
 * \code{.c}
 * ailayer_tanh_q31_t tanh_layer = AILAYER_TANH_Q31_M();
 * \endcode
 *
 * **Example:** Initialize and connect the layer:\n
 * \code{.c}
 * x = ailayer_tanh_q31_default(&tanh_layer, x);
 * \endcode
 *
 * @param *layer        The layer structure to initialize.
 * @param *input_layer  The prior layer.
 * @return              The (successfully) initialized layer structure.
 */
ailayer_t *ailayer_tanh_q31_default(ailayer_tanh_q31_t *layer, ailayer_t *input_layer);

/** @brief Initializes and connect a \link ailayer_tanh.h Tanh layer \endlink with the \link aimath_q7.h Q7 \endlink default implementation
 *
 * **The quantization parameters of the result tensor are automatically set to {shift = 7, zero_point = 0}
 * because the output values are in the interval (-1, 1).**
 *
 * **Example:** Create the layer structure:\n
 * \code{.c}
 * ailayer_tanh_q7_t tanh_layer;
 * \endcode
 * or
 * \code{.c}
 * ailayer_tanh_q7_t tanh_layer = AILAYER_TANH_Q7_M();
 * \endcode
 *
 * **Example:** Initialize and connect the layer:\n
 * \code{.c}
 * x = ailayer_tanh_q7_default(&tanh_layer, x);
 * \endcode
 *
 * @param *layer        The layer structure to initialize.
 * @param *input_layer  The prior layer.
 * @return              The (successfully) initialized layer structure.
 */
ailayer_t *ailayer_tanh_q7_default(ailayer_tanh_q7_t *layer, ailayer_t *input_layer);

/** @brief Calculate and set the quantization parameters for the result tensor of the tanh \link aimath_q31.h Q31 \endlink default implementation
 *
 * *Implementation of ailayer.calc_result_tensor_params.*
 *
 * The quantization parameters are pre-defined and the same as in the corresponding math function aimath_q31_default_tanh().
 *
 * @param *self     The layer structure
 */
void ailayer_tanh_calc_result_tensor_params_q31_default(ailayer_t *self);

/** @brief Calculate and set the quantization parameters for the result tensor of the tanh \link aimath_q7.h Q7 \endlink default implementation
 *
 * *Implementation of ailayer.calc_result_tensor_params.*
 *
 * The quantization parameters are pre-defined and the same as in the corresponding math function aimath_q7_default_tanh().
 *
 * @param *self     The layer structure
 */
void ailayer_tanh_calc_result_tensor_params_q7_default(ailayer_t *self);

#endif // AILAYER_TANH_DEFAULT
