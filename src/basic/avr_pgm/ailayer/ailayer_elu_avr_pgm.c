/**
 * \file basic/avr_pgm/ailayer/ailayer_elu_avr_pgm.c
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
 */


#include "aifes_config.h"

#if __AVR__
#ifdef AIFES_WITH_AVR_PGM

#include "basic/avr_pgm/ailayer/ailayer_elu_avr_pgm.h"

ailayer_t *ailayer_elu_q7_avr_pgm(ailayer_elu_q7_t *layer, ailayer_t *input_layer)
{
    ailayer_t *return_layer;
	layer->base.base.result.dtype = aiq7;
	layer->base.base.deltas.dtype = aiq7;
	layer->base.alpha_dtype = aiq7;

	layer->base.alpha = &(layer->alpha);

	layer->base.base.calc_result_tensor_params = ailayer_elu_calc_result_tensor_params_q7_default;

	//forward
	layer->base.elu = aimath_q7_avr_pgm_elu;

	return_layer = ailayer_elu(&layer->base, input_layer);

	// Disable backward pass
	return_layer->backward = 0;

	return return_layer;
}

#endif // AIFES_WITH_AVR_PGM
#endif // __AVR__
