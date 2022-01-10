/**
 * \file basic/avr_pgm/ailayer/ailayer_sigmoid_avr_pgm.c
 * \internal
 * \date 23.11.2021
 * \endinternal
 * \version 2.0alpha
 * \copyright  Copyright (C) 2020-2021  Fraunhofer Institute for Microelectronic Circuits and Systems.
    All rights reserved.

    AIfES is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include "aifes_config.h"

#if __AVR__
#ifdef AIFES_WITH_AVR_PGM

#include "basic/avr_pgm/ailayer/ailayer_sigmoid_avr_pgm.h"

ailayer_t *ailayer_sigmoid_q7_avr_pgm(ailayer_sigmoid_q7_t *layer, ailayer_t *input_layer)
{
    ailayer_t *return_layer;
	layer->dtype = aiq7;

	layer->base.calc_result_tensor_params = ailayer_sigmoid_calc_result_tensor_params_q7_default;

	//forward
	layer->sigmoid = aimath_q7_avr_pgm_sigmoid;

	return_layer = ailayer_sigmoid(layer, input_layer);

	// Disable backward pass
	return_layer->backward = 0;

	return return_layer;
}

#endif // AIFES_WITH_AVR_PGM
#endif // __AVR__
