/**
 * \file basic/default/ailayer/ailayer_dense_default.h
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
 * \brief Default implementation of the \link ailayer_dense.h Dense layer \endlink
 *
 * Hardware independent implementations of the Dense layer in \link aimath_f32.h F32 \endlink,
 *  \link aimath_q31.h Q31 \endlink and \link aimath_q7.h Q7 \endlink data-type.
 * For more information about the Dense layer refer to ailayer_dense.h.
 */

#ifndef AILAYER_DENSE_DEFAULT
#define AILAYER_DENSE_DEFAULT

#include "basic/base/ailayer/ailayer_dense.h"

#include "basic/default/aimath/aimath_f32_default.h"
#include "basic/default/aimath/aimath_q31_default.h"
#include "basic/default/aimath/aimath_q7_default.h"

#define AILAYER_DENSE_F32_M(neurons, weights, bias)  {{0,},neurons,{0,0,0,0,(float *) weights},{0,0,0,0,(float *) bias}}
#define AILAYER_DENSE_F32_A(neurons)                 {{0,},neurons,{0,0,0,0,0},{0,0,0,0,0}}
#define AILAYER_DENSE_Q31_M(neurons, weights, weights_qparams, bias, bias_qparams, result_qparams)  {{0,0,0,0,0,0,0,{0,0,0,result_qparams,0}},neurons,{0,0,0,weights_qparams,(float *) weights},{0,0,0,bias_qparams,(float *) bias},}
#define AILAYER_DENSE_Q31_A(neurons)                 {{0,},neurons,{0,0,0,0,0},{0,0,0,0,0}}
#define AILAYER_DENSE_Q7_M(neurons, weights, weights_qparams, bias, bias_qparams, result_qparams)  {{0,0,0,0,0,0,0,{0,0,0,result_qparams,0}},neurons,{0,0,0,weights_qparams,(float *) weights},{0,0,0,bias_qparams,(float *) bias},}
#define AILAYER_DENSE_Q7_A(neurons)                  {{0,},neurons,{0,0,0,0,0},{0,0,0,0,0}}

typedef struct ailayer_dense 	ailayer_dense_f32_t;
typedef struct ailayer_dense 	ailayer_dense_q31_t;
typedef struct ailayer_dense 	ailayer_dense_q7_t;

/** @brief Initializes and connect a \link ailayer_dense.h Dense layer \endlink with the \link aimath_f32.h F32 \endlink default implementation
 *
 * **Example:** Create the layer structure with pretrained weights:\n
 * In C:
 * \code{.c}
 * // Use constant data only for inference. For training remove the const qualifier!!
 * const float weights_data_dense[] = {-10.1164f, -8.4212f, 5.4396f, 7.297f, -7.6482f, -9.0155f};
 * const float bias_data_dense[] = {-2.9653f,  2.3677f, -1.5968f};
 * ailayer_dense_f32_t dense_layer = {
 *     .neurons = 3,
 *     .weights.data = (float *) weights_data_dense,
 *     .bias.data = (float *) bias_data_dense
 * };
 * \endcode
 * In C, C++ and on Arduino:
 * \code{.c}
 * // Use constant data only for inference. For training remove the const qualifier!!
 * const float weights_data_dense[] = {-10.1164f, -8.4212f, 5.4396f, 7.297f, -7.6482f, -9.0155f};
 * const float bias_data_dense[] = {-2.9653f,  2.3677f, -1.5968f};
 * ailayer_dense_f32_t dense_layer = AILAYER_DENSE_F32_M(3, weights_data_dense, bias_data_dense);
 * \endcode
 *
 * **Example:** Create the layer structure for automatic parameter distribution:\n
 * In C:
 * \code{.c}
 * ailayer_dense_f32_t dense_layer = {
 *     .neurons = 3
 * };
 * \endcode
 * In C, C++ and on Arduino:
 * \code{.c}
 * ailayer_dense_f32_t dense_layer = AILAYER_DENSE_F32_A(3);
 * \endcode
 *
 * **Example:** Initialize and connect the layer:\n
 * \code{.c}
 * x = ailayer_dense_f32_default(&dense_layer, x);
 * \endcode
 *
 * @param *layer        The layer structure to initialize.
 * @param *input_layer  The prior layer.
 * @return              The (successfully) initialized layer structure.
 */
ailayer_t *ailayer_dense_f32_default(ailayer_dense_f32_t *layer, ailayer_t *input_layer);

/** @brief Initializes and connect a \link ailayer_dense.h Dense layer \endlink with the \link aimath_f32.h F32 \endlink default implementation
 *
 * This implementation is the same as ailayer_dense_f32_default() but with a transposed weights matrix/tensor.
 * This may lead to a speedup in runtime and makes hardware accelerator usage easier.
 *
 * **Example:** Create the layer structure with pretrained weights:\n
 * In C:
 * \code{.c}
 * // Use constant data only for inference. For training remove the const qualifier!!
 * const float weights_data_dense[] = {-10.1164f, 7.297f, -8.4212f, -7.6482f, 5.4396f, -9.0155f};
 * const float bias_data_dense[] = {-2.9653f,  2.3677f, -1.5968f};
 * ailayer_dense_f32_t dense_layer = {
 *     .neurons = 3,
 *     .weights.data = (float *) weights_data_dense,
 *     .bias.data = (float *) bias_data_dense
 * };
 * \endcode
 * In C, C++ and on Arduino:
 * \code{.c}
 * // Use constant data only for inference. For training remove the const qualifier!!
 * const float weights_data_dense[] = {-10.1164f, 7.297f, -8.4212f, -7.6482f, 5.4396f, -9.0155f};
 * const float bias_data_dense[] = {-2.9653f,  2.3677f, -1.5968f};
 * ailayer_dense_f32_t dense_layer = AILAYER_DENSE_F32_M(3, weights_data_dense, bias_data_dense);
 * \endcode
 *
 * **Example:** Create the layer structure for automatic parameter distribution:\n
 * In C:
 * \code{.c}
 * ailayer_dense_f32_t dense_layer = {
 *     .neurons = 3
 * };
 * \endcode
 * In C, C++ and on Arduino:
 * \code{.c}
 * ailayer_dense_f32_t dense_layer = AILAYER_DENSE_F32_A(3);
 * \endcode
 *
 * **Example:** Initialize and connect the layer:\n
 * \code{.c}
 * x = ailayer_dense_f32_wt_default(&dense_layer, x);
 * \endcode
 *
 * @param *layer        The layer structure to initialize.
 * @param *input_layer  The prior layer.
 * @return              The (successfully) initialized layer structure.
 */
ailayer_t *ailayer_dense_wt_f32_default(ailayer_dense_f32_t *layer, ailayer_t *input_layer);

/** @brief Initializes and connect a \link ailayer_dense.h Dense layer \endlink with the \link aimath_q31.h Q31 \endlink default implementation
 *
 * **Example:** Create the layer structure with pretrained weights:\n
 * In C:
 * \code{.c}
 * // Use constant data only for inference. For training remove the const qualifier!!
 * // Weights (32 bit quantized)
 * const aimath_q31_params_t weights_q_params_dense = { .shift = 26, .zero_point = 0 };
 * const int32_t weights_data_dense[] = {-484203965, -294377834, -415073361, 234042792,  514922145, -408068817};
 * // Bias (32 bit quantized)
 * const aimath_q31_params_t bias_q_params_dense = { .shift = 26, .zero_point = 0 };
 * const int32_t bias_data_dense[] = {-91841604, 133375416, 185587399};
 * // Result (32 bit quantized)
 * const aimath_q31_params_t result_q_params_dense = { .shift = 24, .zero_point = 0 };
 * ailayer_dense_q31_t dense_layer = {
 *     .neurons = 3,
 *     .weights = {
 *         .tensor_params = (aimath_q31_params_t *) &weights_q_params_dense,
 *         .data = (int32_t *) weights_data_dense
 *     },
 *     .bias = {
 *         .tensor_params = (aimath_q31_params_t *) &bias_q_params_dense,
 *         .data = (int32_t *) bias_data_dense
 *     },
 *     .base.result.tensor_params = (aimath_q31_params_t *) &result_q_params_dense
 * };
 * \endcode
 * In C, C++ and on Arduino:
 * \code{.c}
 * // Use constant data only for inference. For training remove the const qualifier!!
 * // Weights (32 bit quantized)
 * const aimath_q31_params_t weights_q_params_dense = { .shift = 26, .zero_point = 0 };
 * const int32_t weights_data_dense[] = {-484203965, -294377834, -415073361, 234042792,  514922145, -408068817};
 * // Bias (32 bit quantized)
 * const aimath_q31_params_t bias_q_params_dense = { .shift = 26, .zero_point = 0 };
 * const int32_t bias_data_dense[] = {-91841604, 133375416, 185587399};
 * // Result (32 bit quantized)
 * const aimath_q31_params_t result_q_params_dense = { .shift = 24, .zero_point = 0 };
 * ailayer_dense_q31_t dense_layer = AILAYER_DENSE_Q31_M(3,
 *                                                               weights_data_dense, &weights_q_params_dense,
 *                                                               bias_data_dense, &bias_q_params_dense,
 *                                                               &result_q_params_dense);
 * \endcode
 *
 * **Example:** Create the layer structure for automatic parameter distribution:\n
 * In C:
 * \code{.c}
 * ailayer_dense_q31_t dense_layer = {
 *     .neurons = 3
 * };
 * \endcode
 * In C, C++ and on Arduino:
 * \code{.c}
 * ailayer_dense_q31_t dense_layer = AILAYER_DENSE_Q31_A(3);
 * \endcode
 *
 * **Example:** Initialize and connect the layer:\n
 * \code{.c}
 * x = ailayer_dense_q31_default(&dense_layer, x);
 * \endcode
 *
 * @param *layer        The layer structure to initialize.
 * @param *input_layer  The prior layer.
 * @return              The (successfully) initialized layer structure.
 */
ailayer_t *ailayer_dense_q31_default(ailayer_dense_q31_t *layer, ailayer_t *input_layer);

/** @brief Initializes and connect a \link ailayer_dense.h Dense layer \endlink with the \link aimath_q7.h Q7 \endlink default implementation
 *
 * **Example:** Create the layer structure with pretrained weights:\n
 * In C:
 * \code{.c}
 * // Use constant data only for inference. For training remove the const qualifier!!
 * // Weights (8 bit quantized)
 * const aimath_q7_params_t weights_q_params_dense = { .shift = 3, .zero_point = 0 };
 * const int8_t weights_data_dense[] = {-81, -67, 44, 58, -61, -72};
 * // Bias (32 bit quantized)
 * const aimath_q31_params_t bias_q_params_dense = { .shift = 10, .zero_point = 0 };
 * const int32_t bias_data_dense[] = {-3036,  2425, -1635};
 * // Result (8 bit quantized)
 * const aimath_q7_params_t result_q_params_dense = { .shift = 3, .zero_point = 41 };
 * ailayer_dense_q7_t dense_layer = {
 *     .neurons = 3,
 *     .weights = {
 *         .tensor_params = (aimath_q7_params_t *) &weights_q_params_dense,
 *         .data = (int8_t *) weights_data_dense
 *     },
 *     .bias = {
 *         .tensor_params = (aimath_q31_params_t *) &bias_q_params_dense,
 *         .data = (int32_t *) bias_data_dense
 *     },
 *     .base.result.tensor_params = (aimath_q7_params_t *) &result_q_params_dense
 * };
 * \endcode
 * In C, C++ and on Arduino:
 * \code{.c}
 * // Use constant data only for inference. For training remove the const qualifier!!
 * // Weights (8 bit quantized)
 * const aimath_q7_params_t weights_q_params_dense = { .shift = 3, .zero_point = 0 };
 * const int8_t weights_data_dense[] = {-81, -67, 44, 58, -61, -72};
 * // Bias (32 bit quantized)
 * const aimath_q31_params_t bias_q_params_dense = { .shift = 10, .zero_point = 0 };
 * const int32_t bias_data_dense[] = {-3036,  2425, -1635};
 * // Result (8 bit quantized)
 * const aimath_q7_params_t result_q_params_dense = { .shift = 3, .zero_point = 41 };
 * ailayer_dense_q7_t dense_layer = AILAYER_DENSE_Q7_M(3,
 *                                                     weights_data_dense, &weights_q_params_dense,
 *                                                     bias_data_dense, &bias_q_params_dense,
 *                                                     &result_q_params_dense);
 * \endcode
 *
 * **Example:** Create the layer structure for automatic parameter distribution:\n
 * In C:
 * \code{.c}
 * ailayer_dense_q7_t dense_layer = {
 *     .neurons = 3
 * };
 * \endcode
 * In C, C++ and on Arduino:
 * \code{.c}
 * ailayer_dense_q7_t dense_layer = AILAYER_DENSE_Q7_A(3);
 * \endcode
 *
 * **Example:** Initialize and connect the layer:\n
 * \code{.c}
 * x = ailayer_dense_q7_default(&dense_layer, x);
 * \endcode
 *
 * @param *layer        The layer structure to initialize.
 * @param *input_layer  The prior layer.
 * @return              The (successfully) initialized layer structure.
 */
ailayer_t *ailayer_dense_q7_default(ailayer_dense_q7_t *layer, ailayer_t *input_layer);

/** @brief Initializes and connect a \link ailayer_dense.h Dense layer \endlink with the \link aimath_q7.h Q7 \endlink default implementation for transposed weights tensor
 *
 * This implementation is the same as ailayer_dense_q7_default() but with a transposed weights matrix/tensor.
 * This may lead to a speedup in runtime and makes hardware accelerator usage easier.
 *
 * **Example:** Create the layer structure with pretrained weights:\n
 * In C:
 * \code{.c}
 * // Use constant data only for inference. For training remove the const qualifier!!
 * // Weights (8 bit quantized)
 * const aimath_q7_params_t weights_q_params_dense = { .shift = 3, .zero_point = 0 };
 * const int8_t weights_data_dense[] = {-81, 58, -67, -61, 44, -72};
 * // Bias (32 bit quantized)
 * const aimath_q31_params_t bias_q_params_dense = { .shift = 10, .zero_point = 0 };
 * const int32_t bias_data_dense[] = {-3036,  2425, -1635};
 * // Result (8 bit quantized)
 * const aimath_q7_params_t result_q_params_dense = { .shift = 3, .zero_point = 41 };
 * ailayer_dense_q7_t dense_layer = {
 *     .neurons = 3,
 *     .weights = {
 *         .tensor_params = (aimath_q7_params_t *) &weights_q_params_dense,
 *         .data = (int8_t *) weights_data_dense
 *     },
 *     .bias = {
 *         .tensor_params = (aimath_q31_params_t *) &bias_q_params_dense,
 *         .data = (int32_t *) bias_data_dense
 *     },
 *     .base.result.tensor_params = (aimath_q7_params_t *) &result_q_params_dense
 * };
 * \endcode
 * In C, C++ and on Arduino:
 * \code{.c}
 * // Use constant data only for inference. For training remove the const qualifier!!
 * // Weights (8 bit quantized)
 * const aimath_q7_params_t weights_q_params_dense = { .shift = 3, .zero_point = 0 };
 * const int8_t weights_data_dense[] = {-81, 58, -67, -61, 44, -72};
 * // Bias (32 bit quantized)
 * const aimath_q31_params_t bias_q_params_dense = { .shift = 10, .zero_point = 0 };
 * const int32_t bias_data_dense[] = {-3036,  2425, -1635};
 * // Result (8 bit quantized)
 * const aimath_q7_params_t result_q_params_dense = { .shift = 3, .zero_point = 41 };
 * ailayer_dense_q7_t dense_layer = AILAYER_DENSE_Q7_M(3,
 *                                                     weights_data_dense, &weights_q_params_dense,
 *                                                     bias_data_dense, &bias_q_params_dense,
 *                                                     &result_q_params_dense);
 * \endcode
 *
 * **Example:** Create the layer structure for automatic parameter distribution:\n
 * In C:
 * \code{.c}
 * ailayer_dense_q7_t dense_layer = {
 *     .neurons = 3
 * };
 * \endcode
 * In C, C++ and on Arduino:
 * \code{.c}
 * ailayer_dense_q7_t dense_layer = AILAYER_DENSE_Q7_A(3);
 * \endcode
 *
 * **Example:** Initialize and connect the layer:\n
 * \code{.c}
 * x = ailayer_dense_wt_q7_default(&dense_layer, x);
 * \endcode
 *
 * @param *layer        The layer structure to initialize.
 * @param *input_layer  The prior layer.
 * @return              The (successfully) initialized layer structure.
 */
ailayer_t *ailayer_dense_wt_q7_default(ailayer_dense_q7_t *layer, ailayer_t *input_layer);

/** @brief \link aimath_f32.h F32 \endlink default implementation of the ailayer.init_params function for the Conv2D layer
 *
 * *Implementation of ailayer.init_params.*
 *
 * The function will initialize the weights and bias depending on the following activation function.
 *
 * | Activation function				| Weights-init	| Bias-init  |
 * |------------------------------------|---------------|------------|
 * | None, tanh, logistic, softmax	    | Glorot		| Zeros      |
 * | ReLu and variants				    | He			| Zeros      |
 *
 * @param *self  The layer structure
 */
void ailayer_dense_init_params_f32_default(ailayer_t *self);

/** @brief \link aimath_q31.h Q31 \endlink default implementation of the ailayer.init_params function for the Conv2D layer
 *
 * *Implementation of ailayer.init_params.*
 *
 * The function will initialize the weights and bias depending on the following activation function.
 *
 * | Activation function				| Weights-init	| Bias-init  |
 * |------------------------------------|---------------|------------|
 * | None, tanh, logistic, softmax	    | Glorot		| Zeros      |
 * | ReLu and variants				    | He			| Zeros      |
 *
 * @param *self  The layer structure
 */
void ailayer_dense_init_params_q31_default(ailayer_t *self);


/** @brief Convert a \link aimath_f32.h F32 \endlink dense layer to a \link aimath_q7.h Q7 \endlink representation
 *
 * The weights get 8 bit quantied and the bias gets 32 bit quantized for optimal results.
 *
 * Quantizsation parameters for the previous layer need to be calculated
 * before calling this function.
 *
 * @param *f32_layer_ptr    The source layer structure.
 * @param *q7_layer_ptr     The destination layer structure.
 */
void ailayer_dense_quantize_q7_from_f32(ailayer_dense_f32_t *f32_layer_ptr, ailayer_dense_q7_t *q7_layer_ptr);

#endif // AILAYER_DENSE_DEFAULT
