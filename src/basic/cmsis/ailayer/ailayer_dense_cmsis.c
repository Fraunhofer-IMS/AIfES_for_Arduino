/**
 * \file basic/cmsis/ailayer/ailayer_dense_cmsis.c
 * \version 2.0alpha
 * \date 15.03.2021
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
	layer->result_dtype = aif32;
	layer->weights_dtype = aif32;
	layer->bias_dtype = aif32;

	layer->linear = aimath_f32_cmsis_linear;
	layer->mat_mul = aimath_f32_cmsis_mat_mul;
	layer->tensor_add = aimath_f32_default_tensor_add;

	return ailayer_dense(layer, input_layer);
}
/*
ailayer_t *ailayer_dense_q31_cmsis(ailayer_dense_t *layer, ailayer_t *input_layer)
{
	layer->result_dtype = aiq31;
	layer->weights_dtype = aiq31;
	layer->bias_dtype = aiq31;

	layer->linear = aimath_q31_default_linear32;
	layer->mat_mul = aimath_q31_default_mat_mul;
	layer->tensor_add = aimath_q31_default_tensor_add_different_shift;

	return ailayer_dense(layer, input_layer);
}
*/

ailayer_t *ailayer_dense_wt_q7_cmsis(ailayer_dense_t *layer, ailayer_t *input_layer)
{
    ailayer_t *return_layer;

	layer->result_dtype = aiq7;
	layer->weights_dtype = aiq7;
	layer->bias_dtype = aiq31; // Higher precision (s_bias = s_input + s_weights)

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
