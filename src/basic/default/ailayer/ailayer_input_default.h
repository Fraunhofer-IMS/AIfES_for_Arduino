/**
 * \file basic/default/ailayer/ailayer_input_default.h
 * \internal
 * \date 19.11.2020
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
 * \brief Default implementation of the \link ailayer_input.h Input layer \endlink
 *
 * Hardware independent implementations of the Input layer in \link aimath_f32.h F32 \endlink,
 *  \link aimath_q31.h Q31 \endlink and \link aimath_q7.h Q7 \endlink data-type.
 * For more information about the Input layer refer to ailayer_input.h.
 */

#ifndef AILAYER_INPUT_DEFAULT
#define AILAYER_INPUT_DEFAULT

#include "basic/base/ailayer/ailayer_input.h"

#include "basic/default/aimath/aimath_f32_default.h"
#include "basic/default/aimath/aimath_q31_default.h"
#include "basic/default/aimath/aimath_q7_default.h"

#define AILAYER_INPUT_F32_A(input_dim, input_shape) {{0,},input_dim,input_shape}
#define AILAYER_INPUT_F32_M(input_dim, input_shape) {{0,},input_dim,input_shape}
#define AILAYER_INPUT_Q31_A(input_dim, input_shape) {{0,},input_dim,input_shape}
#define AILAYER_INPUT_Q31_M(input_dim, input_shape, input_qparams) {{0,0,0,0,0,0,0,{0,0,0,input_qparams,0}},input_dim,input_shape}
#define AILAYER_INPUT_Q7_A(input_dim, input_shape) {{0,},input_dim,input_shape}
#define AILAYER_INPUT_Q7_M(input_dim, input_shape, input_qparams) {{0,0,0,0,0,0,0,{0,0,0,input_qparams,0}},input_dim,input_shape}

typedef struct ailayer_input 	ailayer_input_f32_t;
typedef struct ailayer_input 	ailayer_input_q31_t;
typedef struct ailayer_input 	ailayer_input_q7_t;

/** @brief Initializes and connect an \link ailayer_input.h Input layer \endlink with the \link aimath_f32.h F32 \endlink default implementation
 *
 * **Example:** Create the layer structure:\n
 * In C:
 * \code{.c}
 * uint16_t input_layer_shape[] = {1, 2};
 * ailayer_input_f32_t input_layer = {
 *     .input_dim = 2,
 *     .input_shape = input_layer_shape
 * };
 * \endcode
 * In C, C++ and on Arduino:
 * \code{.c}
 * uint16_t input_layer_shape[] = {1, 2};
 * ailayer_input_f32_t input_layer  = AILAYER_INPUT_F32_A(2, input_layer_shape);
 * \endcode
 *
 * **Example:** Initialize and connect the layer:\n
 * \code{.c}
 * x = ailayer_input_f32_default(&input_layer);
 * \endcode
 *
 * @param *layer    The layer structure to initialize.
 * @return          The (successfully) initialized layer structure.
 */
ailayer_t *ailayer_input_f32_default(ailayer_input_f32_t *layer);

/** @brief Initializes and connect an \link ailayer_input.h Input layer \endlink with the \link aimath_q31.h Q31 \endlink default implementation
 *
 * **Example:** Create the layer structure:\n
 * In C:
 * \code{.c}
 * uint16_t input_layer_shape[] = {1, 2};
 * ailayer_input_q31_t input_layer = {
 *     .input_dim = 2,
 *     .input_shape = input_layer_shape
 * };
 * \endcode
 * In C, C++ and on Arduino:
 * \code{.c}
 * uint16_t input_layer_shape[] = {1, 2};
 * ailayer_input_q31_t input_layer  = AILAYER_INPUT_Q31_A(2, input_layer_shape);
 * \endcode
 *
 * **Example:** Initialize and connect the layer:\n
 * \code{.c}
 * x = ailayer_input_q31_default(&input_layer);
 * \endcode
 *
 * @param *layer    The layer structure to initialize.
 * @return          The (successfully) initialized layer structure.
 */
ailayer_t *ailayer_input_q31_default(ailayer_input_q31_t *layer);

/** @brief Initializes and connect an \link ailayer_input.h Input layer \endlink with the \link aimath_q7.h Q7 \endlink default implementation
 *
 * **Example:** Create the layer structure (Automatic version - The quantization parameters are set by aialgo_distribute_parameter_memory()):\n
 * In C:
 * \code{.c}
 * uint16_t input_layer_shape[] = {1, 2};
 * ailayer_input_q7_t input_layer = {
 *     .input_dim = 2,
 *     .input_shape = input_layer_shape
 * };
 * \endcode
 * In C, C++ and on Arduino:
 * \code{.c}
 * uint16_t input_layer_shape[] = {1, 2};
 * ailayer_input_q7_t input_layer  = AILAYER_INPUT_Q7_A(2, input_layer_shape);
 * \endcode
 *
 * **Example:** Create the layer structure (Manual version - The quantization parameters are set manually):\n
 * In C:
 * \code{.c}
 * uint16_t input_layer_shape[] = {1, 2};
 * aimath_q7_params_t input_layer_q_params;
 * ailayer_input_q7_t input_layer = {
 *     .input_dim = 2,
 *     .input_shape = input_layer_shape,
 *     .base.result.tensor_params = &input_layer_q_params
 * };
 * \endcode
 * In C, C++ and on Arduino:
 * \code{.c}
 * uint16_t input_layer_shape[] = {1, 2};
 * aimath_q7_params_t input_layer_q_params;
 * ailayer_input_q7_t input_layer  = AILAYER_INPUT_Q7_M(2, input_layer_shape, &input_layer_q_params);
 * \endcode
 *
 * **Example:** Initialize and connect the layer:\n
 * \code{.c}
 * x = ailayer_input_q7_default(&input_layer);
 * \endcode
 *
 * @param *layer    The layer structure to initialize.
 * @return          The (successfully) initialized layer structure.
 */
ailayer_t *ailayer_input_q7_default(ailayer_input_q7_t *layer);

#endif // AILAYER_INPUT_DEFAULT
