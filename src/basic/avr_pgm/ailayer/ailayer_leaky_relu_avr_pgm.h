/**
 * \file basic/avr_pgm/ailayer/ailayer_leaky_relu_avr_pgm.h
 * \internal
 * \date 23.11.2021
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
 * \brief AVR PGM implementation of the \link ailayer_leaky_relu.h Leaky ReLU layer \endlink
 *
 * AVR controller specific implementation of the Leaky ReLU layer in \link aimath_q7.h Q7 \endlink data-type.
 * For more information about the Leaky ReLU layer refer to ailayer_leaky_relu.h.
 *
 * *Requires [avr/pgmspace.h library](https://www.nongnu.org/avr-libc/user-manual/pgmspace_8h.html)*
 */

#include "aifes_config.h"

#if __AVR__
#ifdef AIFES_WITH_AVR_PGM

#ifndef AILAYER_LEAKY_RELU_AVR_PGM_H
#define AILAYER_LEAKY_RELU_AVR_PGM_H

#include "basic/default/ailayer/ailayer_leaky_relu_default.h"
#include "basic/avr_pgm/aimath/aimath_q7_avr_pgm.h"


#define ailayer_leaky_relu_f32_avr_pgm     ailayer_leaky_relu_f32_default

/** @brief Initializes and connect a \link ailayer_leaky_relu.h Leaky ReLU layer \endlink with the \link aimath_q7.h Q7 \endlink AVR PGM implementation
 *
 * The quantization parameters of the result tensor of the input layer must be defined constant in program memory (PROGMEM).
 * The layer configuration is the same as with ailayer_leaky_relu_q7_default().
 *
 * **The quantization parameters of the result tensor are automatically set to {shift = input_layer.result.shift, zero_point = input_layer.result.zero_point}
 * because the output values are in the interval (alpha * min(input_layer.result), max(input_layer.result)].**
 *
 * **Example:** Create the layer structure of a pre-trained model (qparams must be set manually):\n
 * In C:
 * \code{.c}
 * ailayer_leaky_relu_q7_t leaky_relu_layer = {
 *     .alpha = AISCALAR_Q7(0.01f, 7, 0)
 * };
 * \endcode
 * In C, C++ and on Arduino:
 * \code{.c}
 * ailayer_leaky_relu_q7_t leaky_relu_layer = AILAYER_LEAKY_RELU_Q7_M(AISCALAR_Q7(0.01f, 7, 0));
 * \endcode
 *
 * **Example:** Initialize and connect the layer:\n
 * \code{.c}
 * x = ailayer_leaky_relu_q7_avr_pgm(&leaky_relu_layer, x);
 * \endcode
 *
 * @param *layer        The layer structure to initialize.
 * @param *input_layer  The prior layer.
 * @return              The (successfully) initialized layer structure.
 */
ailayer_t *ailayer_leaky_relu_q7_avr_pgm(ailayer_leaky_relu_q7_t *layer, ailayer_t *input_layer);

#endif // AILAYER_LEAKY_RELU_AVR_PGM_H

#endif // AIFES_WITH_AVR_PGM
#endif // __AVR__
