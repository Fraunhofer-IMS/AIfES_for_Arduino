/**
 * \file basic/aifes_basic_avr_pgm.h
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
 * \brief Include all headers of the AIfES 2 basic module with AVR PGM implementations
 *
 * *Uses [avr/pgmspace.h library](https://www.nongnu.org/avr-libc/user-manual/pgmspace_8h.html)*
 */

#define AIFES_WITH_AVR_PGM

#ifdef __cplusplus
extern "C" {
#endif

// Include the math in avr pgm implementation
#include "basic/avr_pgm/aimath/aimath_f32_avr_pgm.h"
#include "basic/avr_pgm/aimath/aimath_q7_avr_pgm.h"

// Include the layers in avr pgm implementation
#include "basic/avr_pgm/ailayer/ailayer_dense_avr_pgm.h"
#include "basic/avr_pgm/ailayer/ailayer_relu_avr_pgm.h"
#include "basic/avr_pgm/ailayer/ailayer_leaky_relu_avr_pgm.h"
#include "basic/avr_pgm/ailayer/ailayer_elu_avr_pgm.h"
#include "basic/avr_pgm/ailayer/ailayer_sigmoid_avr_pgm.h"
#include "basic/avr_pgm/ailayer/ailayer_tanh_avr_pgm.h"
#include "basic/avr_pgm/ailayer/ailayer_softmax_avr_pgm.h"
#include "basic/avr_pgm/ailayer/ailayer_softsign_avr_pgm.h"

#ifdef __cplusplus
} // End extern "C"
#endif
