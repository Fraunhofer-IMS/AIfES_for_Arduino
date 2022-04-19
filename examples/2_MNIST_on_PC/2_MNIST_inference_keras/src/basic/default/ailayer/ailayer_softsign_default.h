/**
 * \file basic/default/ailayer/ailayer_softsign_default.h
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
 * \brief Default implementation of the \link ailayer_softsign.h Softsign layer \endlink
 *
 * Hardware independent implementations of the Softsign layer in \link aimath_f32.h F32 \endlink data-type.
 * For more information about the Softsign layer refer to ailayer_softsign.h.
 */


#ifndef AILAYER_SOFTSIGN_DEFAULT
#define AILAYER_SOFTSIGN_DEFAULT

#include "basic/base/ailayer/ailayer_softsign.h"

#include "basic/default/aimath/aimath_f32_default.h"

typedef struct ailayer_softsign 	ailayer_softsign_f32_t;

/** @brief Initializes and connect a \link ailayer_softsign.h Softsign layer \endlink with the \link aimath_f32.h F32 \endlink default implementation
 *
 * Example: Create the layer structure:\n
 * \code{.c}
 * ailayer_softsign_f32_t softsign_layer;
 * \endcode
 *
 * Example: Initialize and connect the layer:\n
 * \code{.c}
 * x = ailayer_softsign_f32_default(&softsign_layer, x);
 * \endcode
 *
 * @param *layer        The layer structure to initialize.
 * @param *input_layer  The prior layer.
 * @return              The (successfully) initialized layer structure.
 */
ailayer_t *ailayer_softsign_f32_default(ailayer_softsign_f32_t *layer, ailayer_t *input_layer);

#endif // AILAYER_SOFTSIGN_DEFAULT
