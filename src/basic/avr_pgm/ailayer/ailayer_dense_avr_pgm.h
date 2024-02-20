/**
 * \file basic/avr_pgm/ailayer/ailayer_dense_avr_pgm.h
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
 * \brief Implementation of the \link ailayer_dense.h Dense layer \endlink with parameters in PROGMEM for AVR contoller
 *
 * AVR controller implementations of the Dense layer in \link aimath_f32.h F32 \endlink
 * and \link aimath_q7.h Q7 \endlink data-type.
 * For more information about the Dense layer refer to ailayer_dense.h.
 *
 * This implementation allows access of weights and biases directly from the program memory of AVR controllers.
 * This is useful if there are too many weights to fit into the RAM.
 *
 * *Requires [avr/pgmspace.h library](https://www.nongnu.org/avr-libc/user-manual/pgmspace_8h.html)*
 */

#include "aifes_config.h"

#if __AVR__
#ifdef AIFES_WITH_AVR_PGM

#ifndef AILAYER_DENSE_AVR_PGM_H
#define AILAYER_DENSE_AVR_PGM_H

#include "basic/default/ailayer/ailayer_dense_default.h"
#include "basic/avr_pgm/aimath/aimath_f32_avr_pgm.h"
#include "basic/avr_pgm/aimath/aimath_q7_avr_pgm.h"

/** @brief Initializes and connect a \link ailayer_dense.h Dense layer \endlink with the \link aimath_f32.h F32 \endlink AVR PGM implementation
 *
 * The weights and bias have to be defined constant in program memory (PROGMEM).
 * The rest of the layer configuration is the same as with ailayer_dense_f32_default().
 *
 * **Example:** Create the layer structure with pretrained weights:\n
 * In C:
 * \code{.c}
 * // Use constant data only for inference. For training remove the const qualifier!!
 * const float weights_data_dense[] PROGMEM = {-10.1164f, -8.4212f, 5.4396f, 7.297f, -7.6482f, -9.0155f};
 * const float bias_data_dense[] PROGMEM = {-2.9653f,  2.3677f, -1.5968f};
 * ailayer_dense_f32_t dense_layer = {
 *     .neurons = 3,
 *     .weights.data = (float *) weights_data_dense,
 *     .bias.data = (float *) bias_data_dense
 * };
 * \endcode
 * In C, C++ and on Arduino:
 * \code{.c}
 * // Use constant data only for inference. For training remove the const qualifier!!
 * const float weights_data_dense[] PROGMEM = {-10.1164f, -8.4212f, 5.4396f, 7.297f, -7.6482f, -9.0155f};
 * const float bias_data_dense[] PROGMEM = {-2.9653f,  2.3677f, -1.5968f};
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
 * x = ailayer_dense_f32_avr_pgm(&dense_layer, x);
 * \endcode
 *
 * @param *layer        The layer structure to initialize.
 * @param *input_layer  The prior layer.
 * @return              The (successfully) initialized layer structure.
 */
ailayer_t *ailayer_dense_f32_avr_pgm(ailayer_dense_f32_t *layer, ailayer_t *input_layer);

/** @brief Initializes and connect a \link ailayer_dense.h Dense layer \endlink with the \link aimath_q7.h Q7 \endlink AVR PGM implementation
 *
 * The weights, bias and quantization parameter have to be defined constant in program memory (PROGMEM).
 * The rest of the layer configuration is the same as with ailayer_dense_q7_default().
 *
 * **Example:** Create the layer structure with pretrained weights:\n
 * In C:
 * \code{.c}
 * // Use constant data only for inference. For training remove the const qualifier!!
 * // Weights (8 bit quantized)
 * const aimath_q7_params_t weights_q_params_dense PROGMEM = { .shift = 3, .zero_point = 0 };
 * const int8_t weights_data_dense[] PROGMEM = {-81, -67, 44, 58, -61, -72};
 * // Bias (32 bit quantized)
 * const aimath_q31_params_t bias_q_params_dense PROGMEM = { .shift = 10, .zero_point = 0 };
 * const int32_t bias_data_dense[] PROGMEM = {-3036,  2425, -1635};
 * // Result (8 bit quantized)
 * const aimath_q7_params_t result_q_params_dense PROGMEM = { .shift = 3, .zero_point = 41 };
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
 * const aimath_q7_params_t weights_q_params_dense PROGMEM = { .shift = 3, .zero_point = 0 };
 * const int8_t weights_data_dense[] PROGMEM = {-81, -67, 44, 58, -61, -72};
 * // Bias (32 bit quantized)
 * const aimath_q31_params_t bias_q_params_dense PROGMEM = { .shift = 10, .zero_point = 0 };
 * const int32_t bias_data_dense[] PROGMEM = {-3036,  2425, -1635};
 * // Result (8 bit quantized)
 * const aimath_q7_params_t result_q_params_dense PROGMEM = { .shift = 3, .zero_point = 41 };
 * ailayer_dense_q7_t dense_layer = AILAYER_DENSE_Q7_M(3,
 *                                                     weights_data_dense, &weights_q_params_dense,
 *                                                     bias_data_dense, &bias_q_params_dense,
 *                                                     &result_q_params_dense);
 * \endcode
 *
 * **Example:** Create the layer structure for automatic parameter distribution (parameter buffer must be in PROGMEM):\n
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
 * x = ailayer_dense_q7_avr_pgm(&dense_layer, x);
 * \endcode
 *
 * @param *layer        The layer structure to initialize.
 * @param *input_layer  The prior layer.
 * @return              The (successfully) initialized layer structure.
 */
ailayer_t *ailayer_dense_q7_avr_pgm(ailayer_dense_q7_t *layer, ailayer_t *input_layer);

/** @brief Initializes and connect a \link ailayer_dense.h Dense layer \endlink with the \link aimath_q7.h Q7 \endlink AVR PGM implementation
 *
 * This implementation is the same as ailayer_dense_q7_avr_pgm() but with a transposed weights matrix/tensor.
 *
 * The weights, bias and quantization parameter have to be defined constant in program memory (PROGMEM).
 * The rest of the layer configuration is the same as with ailayer_dense_q7_default().
 *
 * **Example:** Create the layer structure with pretrained weights:\n
 * In C:
 * \code{.c}
 * // Use constant data only for inference. For training remove the const qualifier!!
 * // Weights (8 bit quantized)
 * const aimath_q7_params_t weights_q_params_dense PROGMEM = { .shift = 3, .zero_point = 0 };
 * const int8_t weights_data_dense[] PROGMEM = {-81, 58, -67, -61, 44, -72};
 * // Bias (32 bit quantized)
 * const aimath_q31_params_t bias_q_params_dense PROGMEM = { .shift = 10, .zero_point = 0 };
 * const int32_t bias_data_dense[] PROGMEM = {-3036,  2425, -1635};
 * // Result (8 bit quantized)
 * const aimath_q7_params_t result_q_params_dense PROGMEM = { .shift = 3, .zero_point = 41 };
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
 * const aimath_q7_params_t weights_q_params_dense PROGMEM = { .shift = 3, .zero_point = 0 };
 * const int8_t weights_data_dense[] PROGMEM = {-81, 58, -67, -61, 44, -72};
 * // Bias (32 bit quantized)
 * const aimath_q31_params_t bias_q_params_dense PROGMEM = { .shift = 10, .zero_point = 0 };
 * const int32_t bias_data_dense[] PROGMEM = {-3036,  2425, -1635};
 * // Result (8 bit quantized)
 * const aimath_q7_params_t result_q_params_dense PROGMEM = { .shift = 3, .zero_point = 41 };
 * ailayer_dense_q7_t dense_layer = AILAYER_DENSE_Q7_M(3,
 *                                                     weights_data_dense, &weights_q_params_dense,
 *                                                     bias_data_dense, &bias_q_params_dense,
 *                                                     &result_q_params_dense);
 * \endcode
 *
 * **Example:** Create the layer structure for automatic parameter distribution (parameter buffer must be in PROGMEM):\n
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
 * x = ailayer_dense_wt_q7_avr_pgm(&dense_layer, x);
 * \endcode
 *
 * @param *layer        The layer structure to initialize.
 * @param *input_layer  The prior layer.
 * @return              The (successfully) initialized layer structure.
 */
ailayer_t *ailayer_dense_wt_q7_avr_pgm(ailayer_dense_q7_t *layer, ailayer_t *input_layer);

#endif // AILAYER_DENSE_AVR_PGM_H

#endif // AIFES_WITH_AVR_PGM
#endif // __AVR__
