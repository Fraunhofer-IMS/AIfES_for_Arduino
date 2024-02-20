/**
 * \file cnn/default/ailayer/ailayer_conv2d_default.c
 * \version 2.2.0
 * \date 18.10.2021
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
 * \brief See ailayer_conv2D_default.h for documentation.
 * \details
 */

#include "cnn/default/ailayer/ailayer_conv2d_default.h"

// For auto initializer function
#include "basic/base/ailayer/ailayer_relu.h"
#include "basic/base/ailayer/ailayer_leaky_relu.h"
#include "basic/base/ailayer/ailayer_elu.h"

ailayer_t *ailayer_conv2d_f32_default(ailayer_conv2d_f32_t *layer, ailayer_t *input_layer)
{
	layer->base.result.dtype = aif32;
	layer->base.deltas.dtype = aif32;
	layer->weights.dtype = aif32;
	layer->bias.dtype = aif32;

	layer->base.calc_result_tensor_params = 0;
	layer->base.init_params = ailayer_conv2d_init_params_f32_default;

    layer->conv2d_fwd = aimath_f32_default_conv2d_fwd;
    layer->conv2d_bwd = aimath_f32_default_conv2d_bwd;
    layer->conv2d_bwd_full = aimath_f32_default_conv2d_bwd_full;
    layer->tensor_add = aimath_f32_default_tensor_add;
    layer->sum_channelwise = aimath_f32_default_sum_channelwise;

    return ailayer_conv2d(layer, input_layer);
}

ailayer_t *ailayer_conv2d_cfirst_f32_default(ailayer_conv2d_f32_t *layer, ailayer_t *input_layer)
{
    layer->channel_axis = AIFES_CHANNELS_FIRST;
    return ailayer_conv2d_f32_default(layer, input_layer);
}

ailayer_t *ailayer_conv2d_chw_f32_default(ailayer_conv2d_f32_t *layer, ailayer_t *input_layer)
{
    layer->channel_axis = AIFES_CHANNELS_FIRST;
    return ailayer_conv2d_f32_default(layer, input_layer);
}

ailayer_t *ailayer_conv2d_clast_f32_default(ailayer_conv2d_f32_t *layer, ailayer_t *input_layer)
{
    layer->channel_axis = AIFES_CHANNELS_LAST;
    return ailayer_conv2d_f32_default(layer, input_layer);
}

ailayer_t *ailayer_conv2d_hwc_f32_default(ailayer_conv2d_f32_t *layer, ailayer_t *input_layer)
{
    layer->channel_axis = AIFES_CHANNELS_LAST;
    return ailayer_conv2d_f32_default(layer, input_layer);
}

void ailayer_conv2d_init_params_f32_default(ailayer_t *self)
{
	ailayer_conv2d_t *layer = (ailayer_conv2d_t *) (self->layer_configuration);
	// Switch axis when weights are transposed
	int8_t cin_axis = layer->channel_axis;
	int8_t cout_axis = 0;

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
