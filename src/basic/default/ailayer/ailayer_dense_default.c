/**
 * \file basic/default/ailayer/ailayer_dense_default.c
 * \version 2.2.0
 * \date 28.10.2020
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
 * \brief See ailayer_dense_default.h for documentation.
 * \details
 */

#include "basic/default/ailayer/ailayer_dense_default.h"

// For auto initializer function
#include "basic/base/ailayer/ailayer_relu.h"
#include "basic/base/ailayer/ailayer_leaky_relu.h"
#include "basic/base/ailayer/ailayer_elu.h"


ailayer_t *ailayer_dense_f32_default(ailayer_dense_f32_t *layer, ailayer_t *input_layer)
{
	layer->base.result.dtype = aif32;
	layer->base.deltas.dtype = aif32;
	layer->weights.dtype = aif32;
	layer->bias.dtype = aif32;

	layer->base.calc_result_tensor_params = 0;
	layer->base.init_params = ailayer_dense_init_params_f32_default;

	// Forward pass
	layer->linear = aimath_f32_default_linear;

	// Backward pass
	layer->mat_mul_at = aimath_f32_default_mat_mul_at;
	layer->mat_mul_bt = aimath_f32_default_mat_mul_bt;
	layer->tensor_add = aimath_f32_default_tensor_add;
	layer->sum_channelwise = aimath_f32_default_sum_channelwise;

	return ailayer_dense(layer, input_layer);
}

ailayer_t *ailayer_dense_wt_f32_default(ailayer_dense_f32_t *layer, ailayer_t *input_layer)
{
    ailayer_t *return_layer;

	layer->base.result.dtype = aif32;
	layer->base.deltas.dtype = aif32;
	layer->weights.dtype = aif32;
	layer->bias.dtype = aif32;

	layer->base.calc_result_tensor_params = 0;
	layer->base.init_params = ailayer_dense_init_params_f32_default;

	// Forward pass
	layer->linear = aimath_f32_default_linear_bt;

	// Backward pass
	layer->mat_mul_at = aimath_f32_default_mat_mul_atrt;
	layer->mat_mul_bt = aimath_f32_default_mat_mul;
	layer->tensor_add = aimath_f32_default_tensor_add;
	layer->sum_channelwise = aimath_f32_default_sum_channelwise;

	// Call "constructor" of base "class"
	return_layer = ailayer_dense(layer, input_layer);

	// Change shape to match transposed weights
	layer->weights.shape[0] = layer->neurons;
	layer->weights.shape[1] = input_layer->result.shape[1];

	return return_layer;
}

ailayer_t *ailayer_dense_q31_default(ailayer_dense_q31_t *layer, ailayer_t *input_layer)
{
	layer->base.result.dtype = aiq31;
	layer->base.deltas.dtype = aiq31;
	layer->weights.dtype = aiq31;
	layer->bias.dtype = aiq31;

	layer->base.calc_result_tensor_params = 0;
	layer->base.init_params = ailayer_dense_init_params_q31_default;

	layer->linear = aimath_q31_default_linear32;
	layer->mat_mul_at = aimath_q31_default_mat_mul;
	layer->tensor_add = aimath_q31_default_tensor_add_different_shift;
	layer->sum_channelwise = aimath_q31_default_sum_channelwise;

	return ailayer_dense(layer, input_layer);
}

ailayer_t *ailayer_dense_q7_default(ailayer_dense_q7_t *layer, ailayer_t *input_layer)
{
    ailayer_t *return_layer;

	layer->base.result.dtype = aiq7;
	layer->base.deltas.dtype = aiq7;
	layer->weights.dtype = aiq7;
	layer->bias.dtype = aiq31; // Higher precision (s_bias = s_input + s_weights)

	layer->base.calc_result_tensor_params = 0;
	layer->base.init_params = 0;

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

	layer->base.result.dtype = aiq7;
	layer->base.deltas.dtype = aiq7;
	layer->weights.dtype = aiq7;
	layer->bias.dtype = aiq31; // Higher precision (s_bias = s_input + s_weights)

	layer->base.calc_result_tensor_params = 0;
	layer->base.init_params = 0;

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

void ailayer_dense_init_params_f32_default(ailayer_t *self)
{
	ailayer_dense_t *layer = (ailayer_dense_t *) (self->layer_configuration);
	// Switch axis when weights are transposed
	int8_t cin_axis = (layer->weights.shape[1] == layer->neurons)? 0 : 1;
	int8_t cout_axis = 1 - cin_axis;

	// Initialize weights and biases based on the activation function
	// Recommended:
	// Activation function				| Weights-init	| Bias-init
	// --------------------------------------------------------------
	// None, tanh, logistic, softmax	| Glorot		| Zeros
	// ReLu and variants				| He			| Zeros
	// SELU								| LeCun			| Zeros
	if(self->output_layer != 0){
        if(self->output_layer->layer_type == ailayer_relu_type
           || self->output_layer->layer_type == ailayer_leaky_relu_type
           || self->output_layer->layer_type == ailayer_elu_type){
            aimath_f32_default_init_he_uniform_cdim(&layer->weights, cout_axis);
        } else {
            aimath_f32_default_init_glorot_uniform_cdim(&layer->weights, cin_axis, cout_axis);
        }
	} else {
        aimath_f32_default_init_glorot_uniform_cdim(&layer->weights, cin_axis, cout_axis);
	}

	aimath_f32_default_init_zeros(&layer->bias);

	return;
}

void ailayer_dense_init_params_q31_default(ailayer_t *self)
{
	ailayer_dense_t *layer = (ailayer_dense_t *) (self->layer_configuration);
	// Switch axis when weights are transposed
	int8_t cin_axis = (layer->weights.shape[1] == layer->neurons)? 0 : 1;
	int8_t cout_axis = 1 - cin_axis;

	// Initialize weights and biases based on the activation function
	// Recommended:
	// Activation function				| Weights-init	| Bias-init
	// --------------------------------------------------------------
	// None, tanh, logistic, softmax	| Glorot		| Zeros
	// ReLu and variants				| He			| Zeros
	// SELU								| LeCun			| Zeros
	if(self->output_layer != 0){
        if(self->output_layer->layer_type == ailayer_relu_type
           || self->output_layer->layer_type == ailayer_leaky_relu_type
           || self->output_layer->layer_type == ailayer_elu_type){
            aimath_q31_default_init_he_uniform_cdim(&layer->weights, cout_axis);
        } else {
            aimath_q31_default_init_glorot_uniform_cdim(&layer->weights, cin_axis, cout_axis);
        }
	} else {
        // tensor_params must be preconfigured
        aimath_q31_default_init_glorot_uniform_cdim(&layer->weights, cin_axis, cout_axis);
	}

    // tensor_params must be preconfigured
	aimath_q31_default_zero_tensor(&layer->bias);

	return;
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
