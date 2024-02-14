/**
 * \file basic/cmsis/ailayer/ailayer_dense_cmsis.h
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
 * \brief [Arm CMSIS](https://developer.arm.com/tools-and-software/embedded/cmsis) implementation of the \link ailayer_dense.h Dense layer \endlink for Arm Cortex processors.
 *
 * Arm CMSIS implementations of the Dense layer in \link aimath_f32.h F32 \endlink,
 *  \link aimath_q31.h Q31 \endlink and \link aimath_q7.h Q7 \endlink data-type.
 * These implementations are specifically designed for the Arm Cortex processors and take advantage of SIMD instructions.
 * For more information about the Dense layer refer to ailayer_dense.h.
 */

#include "aifes_config.h"

#if __arm__
#ifdef AIFES_WITH_CMSIS

#ifndef AILAYER_DENSE_CMSIS
#define AILAYER_DENSE_CMSIS

#include "basic/base/ailayer/ailayer_dense.h"

#include "basic/cmsis/aimath/aimath_f32_cmsis.h"
#include "basic/cmsis/aimath/aimath_q31_cmsis.h"
#include "basic/cmsis/aimath/aimath_q7_cmsis.h"

/** @brief Initializes and connect a Dense layer with the \link aimath_f32.h F32 \endlink CMSIS implementation
 *
 * Example: Create the layer structure with pretrained weights:\n
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
 *
 * Example: Create the layer structure for training:\n
 * \code{.c}
 * ailayer_dense_f32_t dense_layer = {
 *     .neurons = 3
 * };
 * \endcode
 *
 * Example: Initialize and connect the layer:\n
 * \code{.c}
 * x = ailayer_dense_f32_cmsis(&dense_layer, x);
 * \endcode
 *
 * @param *layer        The layer structure to initialize.
 * @param *input_layer  The prior layer.
 * @return              The (successfully) initialized layer structure.
 */
ailayer_t *ailayer_dense_f32_cmsis(ailayer_dense_t *layer, ailayer_t *input_layer);

/** @brief Initializes and connect a \link ailayer_dense.h Dense layer \endlink with the \link aimath_q7.h Q7 \endlink AMR CMSIS implementation for transposed weights tensor
 *
 * The weights tensor has to be transposed for this implementation, like in ailayer_dense_wt_q7_default().
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
 * x = ailayer_dense_wt_q7_cmsis(&dense_layer, x);
 * \endcode
 *
 * @param *layer        The layer structure to initialize.
 * @param *input_layer  The prior layer.
 * @return              The (successfully) initialized layer structure.
 */
ailayer_t *ailayer_dense_wt_q7_cmsis(ailayer_dense_t *layer, ailayer_t *input_layer);

#endif // AILAYER_DENSE_CMSIS

#endif // AIFES_WITH_CMSIS
#endif //__arm__
