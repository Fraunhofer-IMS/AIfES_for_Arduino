/**
 * \file basic/avr_pgm/ailayer/ailayer_dense_avr_pgm.c
 * \internal
 * \date 16.11.2021
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
 */

#include "aifes_config.h"


#if __AVR__
#ifdef AIFES_WITH_AVR_PGM

#include "basic/avr_pgm/ailayer/ailayer_dense_avr_pgm.h"

ailayer_t *ailayer_dense_f32_avr_pgm(ailayer_dense_f32_t *layer, ailayer_t *input_layer)
{
	layer->base.result.dtype = aif32;
	layer->base.deltas.dtype = aif32;
	layer->weights.dtype = aif32;
	layer->bias.dtype = aif32;


    layer->base.calc_result_tensor_params = 0;

    ailayer_t *return_layer = ailayer_dense(layer, input_layer);

    layer->linear = aimath_f32_avr_pgm_linear;

    // The math function is different if the result tensor params of the previous layer are in progmem or in RAM
    return_layer->backward = 0;

    return return_layer;
}

ailayer_t *ailayer_dense_q7_avr_pgm(ailayer_dense_f32_t *layer, ailayer_t *input_layer)
{
    ailayer_t *return_layer;

	layer->base.result.dtype = aiq7;
	layer->base.deltas.dtype = aiq7;
	layer->weights.dtype = aiq7;
	layer->bias.dtype = aiq31; // Higher precision (s_bias = s_input + s_weights)

    layer->base.calc_result_tensor_params = 0;

    return_layer = ailayer_dense(layer, input_layer);

    // The math function is different if the result tensor params of the previous layer are in progmem or in RAM
    if(return_layer->input_layer->calc_result_tensor_params == 0){
        layer->linear = aimath_q7_avr_pgm_linear32_1;
    } else {
        layer->linear = aimath_q7_avr_pgm_linear32_2;
    }

    // No training supported, because the parameters are constant
    return_layer->backward = 0;

    return return_layer;
}

ailayer_t *ailayer_dense_wt_q7_avr_pgm(ailayer_dense_q7_t *layer, ailayer_t *input_layer)
{
    ailayer_t *return_layer;

	layer->base.result.dtype = aiq7;
	layer->base.deltas.dtype = aiq7;
	layer->weights.dtype = aiq7;
	layer->bias.dtype = aiq31; // Higher precision (s_bias = s_input + s_weights)

    layer->base.calc_result_tensor_params = 0;

    return_layer = ailayer_dense(layer, input_layer);

	// Change shape to match transposed weights
	layer->weights.shape[0] = layer->neurons;
	layer->weights.shape[1] = input_layer->result.shape[1];

    // The math function is different if the result tensor params of the previous layer are in progmem or in RAM
    if(return_layer->input_layer->calc_result_tensor_params == 0){
        layer->linear = aimath_q7_avr_pgm_linear32_bt_1;
    } else {
        layer->linear = aimath_q7_avr_pgm_linear32_bt_2;
    }

    // No training supported, because the parameters are constant
    return_layer->backward = 0;

    return return_layer;
}

#endif // AIFES_WITH_AVR_PGM
#endif // __AVR__
