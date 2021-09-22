/**
 * \file basic/default/ailayer/ailayer_input_default.h
 * \internal
 * \date 19.11.2020
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
 * \brief Default implementation of the \link ailayer_input.h Input layer \endlink
 *
 * Hardware independent implementations of the Input layer in \link aimath_f32.h F32 \endlink data-type.
 * For more information about the Input layer refer to ailayer_input.h.
 */

#ifndef AILAYER_INPUT_DEFAULT
#define AILAYER_INPUT_DEFAULT

#include "basic/base/ailayer/ailayer_input.h"

#include "basic/default/aimath/aimath_f32_default.h"

typedef struct ailayer_input 	ailayer_input_f32_t;

/** @brief Initializes and connect an \link ailayer_input.h Input layer \endlink with the \link aimath_f32.h F32 \endlink default implementation
 *
 * Example: Create the layer structure:\n
 * \code{.c}
 * uint16_t input_layer_shape[] = {1, 2};
 * ailayer_input_f32_t input_layer = {
 *     .input_dim = 2,
 *     .input_shape = input_layer_shape
 * };
 * \endcode
 *
 * Example: Initialize and connect the layer:\n
 * \code{.c}
 * x = ailayer_input_f32_default(&input_layer);
 * \endcode
 *
 * @param *layer    The layer structure to initialize.
 * @return          The (successfully) initialized layer structure.
 */
ailayer_t *ailayer_input_f32_default(ailayer_input_f32_t *layer);

#endif // AILAYER_INPUT_DEFAULT
