
/**
 * \file basic/default/ailayer/ailayer_tanh_default.h
 * \internal
 * \date 17.03.2021
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
 * \brief Default implementation of the \link ailayer_tanh.h Tanh layer \endlink
 *
 * Hardware independent implementations of the Tanh layer in \link aimath_f32.h F32 \endlink data-type.
 * For more information about the Tanh layer refer to ailayer_tanh.h.
 */

#ifndef AILAYER_TANH_DEFAULT
#define AILAYER_TANH_DEFAULT

#include "basic/base/ailayer/ailayer_tanh.h"

#include "basic/default/aimath/aimath_f32_default.h"

typedef struct ailayer_tanh 	ailayer_tanh_f32_t;

/** @brief Initializes and connect an \link ailayer_tanh.h Tanh layer \endlink with the \link aimath_f32.h F32 \endlink default implementation
 *
 * Example: Create the layer structure:\n
 * \code{.c}
 * ailayer_tanh_f32_t tanh_layer;
 * \endcode
 *
 * Example: Initialize and connect the layer:\n
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

/** @brief Get valid value range of output of tanh layer
*
* Gives the upper and lower bound of the results obtained by interference with the tanh activation function
*
* @param *self the layer to get the upper and lower limit for
* @param *selector the type of the bound (result/deltas, upper/lower)
* @param *result_bound value of the limit, if exists
* @return TRUE or FALSE in case no limit exists
*/
uint8_t ailayer_tanh_get_result_bound_f32_default(const ailayer_t *self, const uint8_t selector, void *result_bound);

#endif // AILAYER_TANH_DEFAULT
