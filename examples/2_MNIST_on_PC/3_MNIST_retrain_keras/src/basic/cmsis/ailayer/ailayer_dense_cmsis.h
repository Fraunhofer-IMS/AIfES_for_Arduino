/**
 * \file basic/cmsis/ailayer/ailayer_dense_cmsis.h
 * \internal
 * \date 15.03.2021
 * \endinternal
 * \version 2.0alpha
 * \copyright  Copyright (C) 2020-2021  Fraunhofer Institute for Microelectronic Circuits and Systems.
    All rights reserved.

    AIfES is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * \brief [Arm CMSIS](https://developer.arm.com/tools-and-software/embedded/cmsis) implementation of the \link ailayer_dense.h Dense layer \endlink for Arm Cortex processors.
 *
 * Arm CMSIS implementations of the Dense layer in \link aimath_f32.h F32 \endlink data-type.
 * These implementations are specifically designed for the Arm Cortex processors and take advantage of SIMD instructions.
 * For more information about the Dense layer refer to ailayer_dense.h.
 */

#ifndef AILAYER_DENSE_CMSIS
#define AILAYER_DENSE_CMSIS

#include "../../../aifes.h"
 
#if __arm__
#ifdef AIFES_WITH_CMSIS

#include "basic/base/ailayer/ailayer_dense.h"

#include "basic/cmsis/aimath/aimath_f32_cmsis.h"

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

#endif // AILAYER_DENSE_CMSIS

#endif // AIFES_WITH_CMSIS
#endif //__arm__
