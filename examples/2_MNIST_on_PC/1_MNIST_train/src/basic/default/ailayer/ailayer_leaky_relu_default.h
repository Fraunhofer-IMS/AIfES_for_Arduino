
/**
 * \file basic/default/ailayer/ailayer_leaky_relu_default.h
 * \internal
 * \date 16.03.2021
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
 * \brief Default implementation of the \link ailayer_leaky_relu.h Leaky ReLU layer \endlink
 *
 * Hardware independent implementations of the Leaky ReLU layer in \link aimath_f32.h F32 \endlink data-type.
 * For more information about the Leaky ReLU layer refer to ailayer_leaky_relu.h.
 */

#ifndef AILAYER_LEAKY_RELU_DEFAULT
#define AILAYER_LEAKY_RELU_DEFAULT

#include "basic/base/ailayer/ailayer_leaky_relu.h"

#include "basic/default/aimath/aimath_f32_default.h"

typedef struct ailayer_leaky_relu_f32 	ailayer_leaky_relu_f32_t;

/** @brief Data-type specific Leaky ReLU layer struct for \link aimath_f32.h F32 \endlink
 *
 * Adds a data field for the constant alpha in \link aimath_f32.h F32 \endlink to the base implementation.
 */
struct ailayer_leaky_relu_f32 {
	ailayer_leaky_relu_t base; /**< Inherited field members from general ailayer_leaky_relu struct. */
	aiscalar_f32_t alpha; /**< Data type-specific parameter used to calculate Leaky ReLU function for input values < 0. */
};

/** @brief Initializes and connect a \link ailayer_leaky_relu.h Leaky ReLU layer \endlink with the \link aimath_f32.h F32 \endlink default implementation
 *
 * Example: Create the layer structure:\n
 * \code{.c}
 * ailayer_leaky_relu_f32_t leaky_relu_layer = {
 *     .alpha = 0.01f
 * };
 * \endcode
 *
 * Example: Initialize and connect the layer:\n
 * \code{.c}
 * x = ailayer_leaky_relu_f32_default(&leaky_relu_layer, x);
 * \endcode
 *
 * @param *layer the layer structure to be initialized
 * @param *input_layer the prior layer that provides the input to this Leaky ReLU layer
 * @return the initialized Leaky ReLU layer structure
 */
ailayer_t *ailayer_leaky_relu_f32_default(ailayer_leaky_relu_f32_t *layer, ailayer_t *input_layer);

#endif // AILAYER_LEAKY_RELU_DEFAULT
