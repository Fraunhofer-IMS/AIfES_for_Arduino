/**
 * \file basic/default/ailayer/ailayer_dense_default.c
 * \version 2.0alpha
 * \date 28.10.2020
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
 * \brief See ailayer_dense_default.h for documentation.
 * \details
 */

#include "basic/default/ailayer/ailayer_dense_default.h"


ailayer_t *ailayer_dense_f32_default(ailayer_dense_f32_t *layer, ailayer_t *input_layer)
{
	layer->result_dtype = aif32;
	layer->weights_dtype = aif32;
	layer->bias_dtype = aif32;

	layer->base.calc_result_tensor_params = 0;

	layer->linear = aimath_f32_default_linear;
	layer->mat_mul = aimath_f32_default_mat_mul;
	layer->tensor_add = aimath_f32_default_tensor_add;

	return ailayer_dense(layer, input_layer);
}

/*
ailayer_t *ailayer_dense_f32_wt_default(ailayer_dense_f32_t *layer, ailayer_t *input_layer)
{
    ailayer_t *return_layer;

	layer->result_dtype = aif32;
	layer->weights_dtype = aif32;
	layer->bias_dtype = aif32;

	layer->base.calc_result_tensor_params = 0;

	layer->linear = aimath_f32_default_linear_bt;
	layer->mat_mul = aimath_f32_default_mat_mul_bt;
	layer->tensor_add = aimath_f32_default_tensor_add;

	// Call "constructor" of base "class"
	return_layer = ailayer_dense(layer, input_layer);

	// Change shape to match transposed weights
	layer->weights.shape[0] = layer->neurons;
	layer->weights.shape[1] = input_layer->result.shape[1];

	return return_layer;
}
*/

ailayer_t *ailayer_dense_q31_default(ailayer_dense_q31_t *layer, ailayer_t *input_layer)
{
	layer->result_dtype = aiq31;
	layer->weights_dtype = aiq31;
	layer->bias_dtype = aiq31;

	layer->base.calc_result_tensor_params = 0;

	layer->linear = aimath_q31_default_linear32;
	layer->mat_mul = aimath_q31_default_mat_mul;
	layer->tensor_add = aimath_q31_default_tensor_add_different_shift;

	return ailayer_dense(layer, input_layer);
}

ailayer_t *ailayer_dense_q7_default(ailayer_dense_q7_t *layer, ailayer_t *input_layer)
{
    ailayer_t *return_layer;

	layer->result_dtype = aiq7;
	layer->weights_dtype = aiq7;
	layer->bias_dtype = aiq31; // Higher precision (s_bias = s_input + s_weights)

	layer->base.calc_result_tensor_params = 0;

	// Call "constructor" of base "class"
	return_layer = ailayer_dense(layer, input_layer);

    // forward
	layer->linear = aimath_q7_default_linear32;

	// backward
	// Not supported for q7
	return_layer->backward = 0;

	return return_layer;
}

ailayer_t *ailayer_dense_wt_q7_default(ailayer_dense_q7_t *layer, ailayer_t *input_layer)
{
    ailayer_t *return_layer;

	layer->result_dtype = aiq7;
	layer->weights_dtype = aiq7;
	layer->bias_dtype = aiq31; // Higher precision (s_bias = s_input + s_weights)

	layer->base.calc_result_tensor_params = 0;

	// Call "constructor" of base "class"
	return_layer = ailayer_dense(layer, input_layer);

	// Change shape to match transposed weights
	layer->weights.shape[0] = layer->neurons;
	layer->weights.shape[1] = input_layer->result.shape[1];

    // forward
	layer->linear = aimath_q7_default_linear32_bt;

	// backward
	// Not supported for q7
	return_layer->backward = 0;

	return return_layer;
}

// The quantization params of the previous layer have to be calculated before calling this function
void ailayer_dense_quantize_q7_from_f32(ailayer_dense_f32_t *f32_layer_ptr, ailayer_dense_q7_t *q7_layer_ptr)
{
    float min_value, max_value;

    // quantize weights to q7
    aimath_f32_default_min(&f32_layer_ptr->weights, &min_value);
    aimath_f32_default_max(&f32_layer_ptr->weights, &max_value);
    if(max_value < -min_value){
        max_value = -min_value;
    }
    aimath_q7_calc_q_params_from_f32(-max_value, max_value, q7_layer_ptr->weights.tensor_params);
    aimath_q7_quantize_tensor_from_f32(&f32_layer_ptr->weights, &q7_layer_ptr->weights);

    // Quantize bias to q31
    // bias_shift = input_shift + weights_shift
    ((aimath_q31_params_t *) q7_layer_ptr->bias.tensor_params)->shift = ((aimath_q7_params_t *) q7_layer_ptr->base.input_layer->result.tensor_params)->shift
                                                                            + ((aimath_q7_params_t *) q7_layer_ptr->weights.tensor_params)->shift;
    ((aimath_q31_params_t *) q7_layer_ptr->bias.tensor_params)->zero_point = 0;

    aimath_q31_quantize_tensor_from_f32(&f32_layer_ptr->bias, &q7_layer_ptr->bias);
    return;
}
