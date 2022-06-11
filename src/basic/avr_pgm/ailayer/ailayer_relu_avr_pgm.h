/**
 * \file basic/avr_pgm/ailayer/ailayer_relu_avr_pgm.h
 * \internal
 * \date 23.11.2021
 * \endinternal
 * \version 2.0alpha
 * \copyright  Copyright (C) 2020-2021  Fraunhofer Institute for Microelectronic Circuits and Systems.
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
 * \brief AVR PGM implementation of the \link ailayer_relu.h ReLU layer \endlink
 *
 * AVR controller specific implementation of the ReLU layer in \link aimath_q7.h Q7 \endlink data-type.
 * For more information about the ReLU layer refer to ailayer_relu.h.
 *
 * *Requires [avr/pgmspace.h library](https://www.nongnu.org/avr-libc/user-manual/pgmspace_8h.html)*
 */

#include "aifes_config.h"

#if __AVR__
#ifdef AIFES_WITH_AVR_PGM

#ifndef AILAYER_RELU_AVR_PGM_H
#define AILAYER_RELU_AVR_PGM_H

#include "basic/default/ailayer/ailayer_relu_default.h"
#include "basic/avr_pgm/aimath/aimath_q7_avr_pgm.h"


#define ailayer_relu_f32_avr_pgm     ailayer_relu_f32_default

/** @brief Initializes and connect a \link ailayer_relu.h ReLU layer \endlink with the \link aimath_q7.h Q7 \endlink AVR PGM implementation
 *
 * The quantization parameters of the result tensor of the input layer must be defined constant in program memory (PROGMEM).
 * The layer configuration is the same as with ailayer_relu_q7_default().
 *
 * **The quantization parameters of the result tensor are automatically set to {shift = input_layer.result.shift, zero_point = input_layer.result.zero_point}
 * because the output values are in the interval [0, max(input_layer.result)].**
 *
 * **Example:** Create the layer structure:\n
 * \code{.c}
 * ailayer_relu_q7_t relu_layer;
 * \endcode
 * or
 * \code{.c}
 * ailayer_relu_q7_t relu_layer = AILAYER_RELU_Q7_M();
 * \endcode
 *
 * **Example:** Initialize and connect the layer:\n
 * \code{.c}
 * x = ailayer_relu_q7_avr_pgm(&relu_layer, x);
 * \endcode
 *
 * @param *layer        The layer structure to initialize.
 * @param *input_layer  The prior layer.
 * @return              The (successfully) initialized layer structure.
 */
ailayer_t *ailayer_relu_q7_avr_pgm(ailayer_relu_q7_t *layer, ailayer_t *input_layer);

#endif // AILAYER_RELU_AVR_PGM_H

#endif // AIFES_WITH_AVR_PGM
#endif // __AVR__
