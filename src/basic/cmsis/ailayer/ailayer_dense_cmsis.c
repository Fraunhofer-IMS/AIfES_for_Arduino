/**
 * \file basic/cmsis/ailayer/ailayer_dense_cmsis.c
 * \version 2.2.0
 * \date 15.03.2021
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
 * \brief
 * \details
 */

#include "aifes_config.h"

#if __arm__
#ifdef AIFES_WITH_CMSIS

#include "basic/default/ailayer/ailayer_dense_default.h"
#include "basic/cmsis/ailayer/ailayer_dense_cmsis.h"
#include "basic/cmsis/aimath/aimath_f32_cmsis.h"


ailayer_t *ailayer_dense_f32_cmsis(ailayer_dense_t *layer, ailayer_t *input_layer)
{
	layer->base.result.dtype = aif32;
	layer->base.deltas.dtype = aif32;
	layer->weights.dtype = aif32;
	layer->bias.dtype = aif32;

	layer->linear = aimath_f32_cmsis_linear;
	layer->mat_mul_at = aimath_f32_default_mat_mul_at;
	layer->mat_mul_bt = aimath_f32_default_mat_mul_bt;
	layer->tensor_add = aimath_f32_default_tensor_add;
	layer->sum_channelwise = aimath_f32_default_sum_channelwise;

	return ailayer_dense(layer, input_layer);
}

ailayer_t *ailayer_dense_wt_q7_cmsis(ailayer_dense_t *layer, ailayer_t *input_layer)
{
    ailayer_t *return_layer;

	layer->base.result.dtype = aiq7;
	layer->base.deltas.dtype = aiq7;
	layer->weights.dtype = aiq7;
	layer->bias.dtype = aiq31; // Higher precision (s_bias = s_input + s_weights)

	// Call "constructor" of base "class"
	return_layer = ailayer_dense(layer, input_layer);

	// Change shape to match transposed weights
	layer->weights.shape[0] = layer->neurons;
	layer->weights.shape[1] = input_layer->result.shape[1];

    // forward
	layer->linear = aimath_q7_cmsis_linear32_bt;

	// backward
	// Not supported for q7
	return_layer->backward = 0;

	return return_layer;
}

#endif // AIFES_WITH_CMSIS
#endif //__arm__
