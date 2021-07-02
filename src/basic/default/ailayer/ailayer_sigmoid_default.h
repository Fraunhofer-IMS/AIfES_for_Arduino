/**
 * \file basic/default/ailayer/ailayer_sigmoid_default.h
 * \internal
 * \date 12.11.2020
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
 * \brief Default implementation of the \link ailayer_sigmoid.h Sigmoid layer \endlink
 *
 * Hardware independent implementations of the Sigmoid layer in \link aimath_f32.h F32 \endlink data-type.
 * For more information about the Sigmoid layer refer to ailayer_sigmoid.h.
 */

#ifndef AILAYER_SIGMOID_DEFAULT
#define AILAYER_SIGMOID_DEFAULT

#include "basic/base/ailayer/ailayer_sigmoid.h"

#include "basic/default/aimath/aimath_f32_default.h"

typedef struct ailayer_sigmoid 	ailayer_sigmoid_f32_t;

/** @brief Initializes and connect a \link ailayer_sigmoid.h Sigmoid layer \endlink with the \link aimath_f32.h F32 \endlink default implementation
 *
 * Example: Create the layer structure:\n
 * \code{.c}
 * ailayer_sigmoid_f32_t sigmoid_layer;
 * \endcode
 *
 * Example: Initialize and connect the layer:\n
 * \code{.c}
 * x = ailayer_sigmoid_f32_default(&sigmoid_layer, x);
 * \endcode
 *
 * @param *layer        The layer structure to initialize.
 * @param *input_layer  The prior layer.
 * @return              The (successfully) initialized layer structure.
 */
ailayer_t *ailayer_sigmoid_f32_default(ailayer_sigmoid_f32_t *layer, ailayer_t *input_layer);

/** @brief Get the value bounds for Sigmoid layers
 *
 * For the float 32 representation only result bounds are defined.
 *
 * @param *self Referenced layer structure.
 * @param selector Select, which bound value to get. Possible values:
 *      - AILAYER_RESULT_LOWER_BOUND
 *      - AILAYER_RESULT_UPPER_BOUND
 *      - AILAYER_DELTAS_LOWER_BOUND
 *      - AILAYER_DELTAS_UPPER_BOUND
 * @param result_bound Pointer to the memory space, the function writes the requested value to.
 * @return
 *          - TRUE: requested value available and set to result_bound
 *          - FALSE: requestet value is _not_ available and result_bound is __not__ set
 */
uint8_t ailayer_sigmoid_get_result_bound_f32_default(const ailayer_t *self, const uint8_t selector, void *result_bound);

#endif // AILAYER_SIGMOID_DEFAULT
