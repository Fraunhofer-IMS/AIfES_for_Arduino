/**
 * \file cnn/base/ailayer/ailayer_conv2d.c
 * \version 2.2.0
 * \date 20.10.2020
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

#include "cnn/base/ailayer/ailayer_conv2d.h"
#include "basic/base/aimath/aimath_basic.h"

AISTRING_STORAGE_WRAPPER(aistring_layer_conv2d, "Conv2D");

const aicore_layertype_t ailayer_conv2d_type_s = {
#ifdef AIDEBUG_PRINT_MODULE_SPECS
    .name = aistring_layer_conv2d,
	.print_specs = ailayer_conv2d_print_specs
#else
    .name = 0,
    .print_specs = 0
#endif
};
const aicore_layertype_t *ailayer_conv2d_type = &ailayer_conv2d_type_s;


AISTRING_STORAGE_WRAPPER(aistring_error_conv2d_1, "[ailayer_conv2d] Channel axis must be either 1 (-3) or 3 (-1).\n");

ailayer_t *ailayer_conv2d(ailayer_conv2d_t *layer, ailayer_t *input_layer)
{
    layer->base.layer_type = ailayer_conv2d_type;
    uint8_t channel_uaxis = layer->channel_axis < 0 ? 4 + layer->channel_axis : layer->channel_axis; // Negative axis = indexing from the end

    layer->base.settings = 0;
    AILAYER_SETTINGS_SET(layer->base.settings, 0b1, AILAYER_SETTINGS_TRAINABLE, TRUE);
    AILAYER_SETTINGS_SET(layer->base.settings, 0b1, AILAYER_SETTINGS_NO_INPUT_GRADIENT, FALSE);

	layer->base.input_layer = input_layer;
    layer->base.output_layer = 0;
	input_layer->output_layer = &(layer->base);

    // Set base params of layer
	layer->base.layer_configuration = layer;
	layer->base.result.dim = 4;
	layer->base.result.shape = layer->result_shape;
	layer->base.deltas.dim = 4;
	layer->base.deltas.shape = input_layer->result.shape;

	layer->weights.dim = 4;
	layer->weights.shape = layer->weights_shape;
	if(channel_uaxis == 1){ // Channels first
        layer->weights.shape[0] = layer->filter_count; // c_out
        layer->weights.shape[1] = input_layer->result.shape[1]; // c_in
        layer->weights.shape[2] = layer->kernel_size[0]; // k_h
        layer->weights.shape[3] = layer->kernel_size[1]; // k_w
	} else if(channel_uaxis == 3) { // Channels last
        layer->weights.shape[0] = layer->filter_count; // c_out
        layer->weights.shape[1] = layer->kernel_size[0]; // k_h
        layer->weights.shape[2] = layer->kernel_size[1]; // k_w
        layer->weights.shape[3] = input_layer->result.shape[3]; // c_in
	} else {
	    // Error
        AILOG_E(aistring_error_conv2d_1);
        return 0;
	}

	layer->bias.dim = 1;
	layer->bias.shape = layer->bias_shape;
	layer->bias.shape[0] = layer->filter_count;

	// Set forward and backward function pointers
	layer->base.forward = ailayer_conv2d_forward;
	layer->base.backward = ailayer_conv2d_backward;

	// Set additional function pointers
	layer->base.calc_result_shape = ailayer_conv2d_calc_result_shape;
	layer->base.sizeof_paramem = ailayer_conv2d_sizeof_paramem;
	layer->base.set_paramem = ailayer_conv2d_set_paramem;
	layer->base.sizeof_trainmem = ailayer_conv2d_sizeof_trainmem;
	layer->base.set_trainmem = ailayer_conv2d_set_trainmem;
	layer->base.sizeof_fwdmem = 0;
	layer->base.sizeof_bwdmem = ailayer_conv2d_sizeof_bwdmem;

	layer->base.trainable_params_count = 2;
	layer->base.trainable_params = layer->trainable_params;
	layer->base.gradients = layer->gradients;
	layer->base.optimem = layer->optimem;

	layer->trainable_params[0] = &layer->weights;
	layer->trainable_params[1] = &layer->bias;

	ailayer_conv2d_calc_result_shape(&layer->base);

	return &layer->base;
}

void ailayer_conv2d_forward(ailayer_t *self)
{
	aitensor_t *x_in = &(self->input_layer->result);
	aitensor_t *x_out = &(self->result);
	ailayer_conv2d_t *layer = (ailayer_conv2d_t *)(self->layer_configuration);
	aitensor_t *weights = &layer->weights;
	aitensor_t *bias = &layer->bias;

    layer->conv2d_fwd(x_in,
                      layer->stride,
                      layer->dilation,
                      layer->padding,
                      weights,
                      bias,
                      layer->channel_axis,
                      0,
                      x_out);

	return;
}

void ailayer_conv2d_backward(ailayer_t *self)
{
	aitensor_t *delta_in = &(self->deltas);
	aitensor_t *delta_out = &(self->output_layer->deltas);
	aitensor_t *x_in = &(self->input_layer->result);
	ailayer_conv2d_t *layer = (ailayer_conv2d_t *)(self->layer_configuration);
	aitensor_t *weights = &layer->weights;
	aitensor_t *d_weights = layer->gradients[0];
	aitensor_t *d_bias = layer->gradients[1];

	aitensor_t temp_result;
	temp_result.data = self->tempmem;

    // Calculate d_weights
    // d_w = x_in * delta_out
    temp_result.dim             = 4;
    temp_result.shape           = d_weights->shape;
    temp_result.dtype           = d_weights->dtype;
    temp_result.tensor_params   = d_weights->tensor_params;
    layer->conv2d_bwd(x_in,
                      layer->stride,
                      layer->dilation,
                      layer->padding,
                      delta_out,
                      layer->channel_axis,
                      0,
                      &temp_result);
    layer->tensor_add(d_weights, &temp_result, d_weights);

    // Calculate d_bias
	// for all f: b_f = sum_hw{dy_fhw}
    temp_result.dim             = 1;
    temp_result.shape           = d_bias->shape;
    temp_result.dtype           = d_bias->dtype;
    temp_result.tensor_params   = d_bias->tensor_params;
	layer->sum_channelwise(delta_out, layer->channel_axis, &temp_result);
    layer->tensor_add(d_bias, &temp_result, d_bias);

    // Calculate delta_in
    // delta_in = delta_out * w'    <- Full convolution (180° rotated kernel and zero padding)
    layer->conv2d_bwd_full(delta_out,
                           layer->stride,
                           layer->dilation,
                           layer->padding,
                           weights,
                           layer->channel_axis,
                           0,
                           delta_in);

	return;
}

void ailayer_conv2d_calc_result_shape(ailayer_t *self)
{
	ailayer_conv2d_t *layer = (ailayer_conv2d_t *)(self->layer_configuration);
	uint16_t s_h = layer->stride[0];
	uint16_t s_w = layer->stride[1];
	uint16_t d_h = layer->dilation[0];
	uint16_t d_w = layer->dilation[1];
	uint16_t n_h;
	uint16_t n_w;
	uint16_t k_h = layer->kernel_size[0];
	uint16_t k_w = layer->kernel_size[1];
    int16_t p_h = layer->padding[0];
    int16_t p_w = layer->padding[1];

    uint8_t channel_uaxis = layer->channel_axis < 0 ? 4 + layer->channel_axis : layer->channel_axis; // Negative axis = indexing from the end

	if(channel_uaxis == 1){ // Channels first
        n_h = self->input_layer->result.shape[2];
        n_w = self->input_layer->result.shape[3];

        // Output dimensions: floor((n - d * (k - 1) - 1) / s + 1
        self->result.shape[0] = self->input_layer->result.shape[0];     // N
        self->result.shape[1] = layer->filter_count;                    // C_OUT
        self->result.shape[2] = (n_h + 2 * p_h - d_h * (k_h - 1) - 1) / s_h + 1;  // H
        self->result.shape[3] = (n_w + 2 * p_w - d_w * (k_w - 1) - 1) / s_w + 1;  // W
	} else if(channel_uaxis == 3){ // Channels last
        n_h = self->input_layer->result.shape[1];
        n_w = self->input_layer->result.shape[2];

        // Output dimensions: floor((n - d * (k - 1) - 1) / s + 1)
        self->result.shape[0] = self->input_layer->result.shape[0];     // N
        self->result.shape[1] = (n_h + 2 * p_h - d_h * (k_h - 1) - 1) / s_h + 1;  // H
        self->result.shape[2] = (n_w + 2 * p_w - d_w * (k_w - 1) - 1) / s_w + 1;  // W
        self->result.shape[3] = layer->filter_count;                    // C_OUT
	} else {
        // Error
        AILOG_E(aistring_error_conv2d_1);
        return;
	}

	return;
}

uint32_t ailayer_conv2d_sizeof_bwdmem(const ailayer_t *self)
{
	const ailayer_conv2d_t *layer = (ailayer_conv2d_t *)(self->layer_configuration);
    uint32_t d_weights_mem, d_bias_mem;

    if(AILAYER_SETTINGS_IS(self->settings, 0b1, AILAYER_SETTINGS_TRAINABLE)){
        // Shared memory buffer for d_weights and d_bias
        d_weights_mem = aimath_sizeof_tensor_data(&layer->weights);
        d_bias_mem = aimath_sizeof_tensor_data(&layer->bias);
        return d_weights_mem > d_bias_mem ? d_weights_mem : d_bias_mem;
    } else {
        // No temp memory is needed
        return 0;
    }
}

uint32_t ailayer_conv2d_sizeof_paramem(const ailayer_t *self)
{
	uint32_t memory = 0;
	ailayer_conv2d_t *layer = (ailayer_conv2d_t *)(self->layer_configuration);

	// Weights
	memory += layer->weights.dtype->tensor_params_size;
    AIFES_ALIGN_INTEGER(memory, AIFES_MEMORY_ALIGNMENT);
	memory += aimath_sizeof_tensor_data(&(layer->weights));
    AIFES_ALIGN_INTEGER(memory, AIFES_MEMORY_ALIGNMENT);

	// Bias
	memory += layer->bias.dtype->tensor_params_size;
    AIFES_ALIGN_INTEGER(memory, AIFES_MEMORY_ALIGNMENT);
	memory += aimath_sizeof_tensor_data(&(layer->bias));
	return memory;
}

void ailayer_conv2d_set_paramem(ailayer_t *self, void *memory_ptr)
{
	uint32_t address_counter = 0;
	ailayer_conv2d_t *layer = (ailayer_conv2d_t *)(self->layer_configuration);

	layer->weights.tensor_params = memory_ptr + address_counter;
	address_counter += layer->weights.dtype->tensor_params_size;
    AIFES_ALIGN_INTEGER(address_counter, AIFES_MEMORY_ALIGNMENT);

	layer->weights.data = memory_ptr + address_counter;
	address_counter += aimath_sizeof_tensor_data(&(layer->weights));
    AIFES_ALIGN_INTEGER(address_counter, AIFES_MEMORY_ALIGNMENT);

	layer->bias.tensor_params = memory_ptr + address_counter;
	address_counter += layer->bias.dtype->tensor_params_size;
    AIFES_ALIGN_INTEGER(address_counter, AIFES_MEMORY_ALIGNMENT);

	layer->bias.data = memory_ptr + address_counter;

	layer->trainable_params[0] = &(layer->weights);
	layer->trainable_params[1] = &(layer->bias);

	return;
}

uint32_t ailayer_conv2d_sizeof_trainmem(const ailayer_t *self)
{
	uint32_t memory = 0;
	ailayer_conv2d_t *layer = (ailayer_conv2d_t *)(self->layer_configuration);

	// Weights
	memory += sizeof(aitensor_t);
	memory += aimath_sizeof_tensor_data(&layer->weights);
    AIFES_ALIGN_INTEGER(memory, AIFES_MEMORY_ALIGNMENT);
	memory += aimath_sizeof_tensor_params(&layer->weights);
    AIFES_ALIGN_INTEGER(memory, AIFES_MEMORY_ALIGNMENT);

	// Bias
	memory += sizeof(aitensor_t);
	memory += aimath_sizeof_tensor_data(&layer->bias);
    AIFES_ALIGN_INTEGER(memory, AIFES_MEMORY_ALIGNMENT);
	memory += aimath_sizeof_tensor_params(&layer->bias);
	return memory;
}

void ailayer_conv2d_set_trainmem(ailayer_t *self, void *memory_ptr)
{
	uint32_t address_counter = 0;
	ailayer_conv2d_t *layer = (ailayer_conv2d_t *) (self->layer_configuration);

	// Weights gradients in gradients[0]
	self->gradients[0] = memory_ptr;
	address_counter += sizeof(aitensor_t);
	self->gradients[0]->data = memory_ptr + address_counter;
	self->gradients[0]->dtype = layer->weights.dtype;
	self->gradients[0]->dim = 4;
	self->gradients[0]->shape = layer->weights.shape;
	address_counter += aimath_sizeof_tensor_data(layer->gradients[0]);
    AIFES_ALIGN_INTEGER(address_counter, AIFES_MEMORY_ALIGNMENT);
	self->gradients[0]->tensor_params = memory_ptr + address_counter;
	address_counter += aimath_sizeof_tensor_params(layer->gradients[0]);
    AIFES_ALIGN_INTEGER(address_counter, AIFES_MEMORY_ALIGNMENT);

	// Bias gradients in gradients[1]
	self->gradients[1] = memory_ptr + address_counter;
	address_counter += sizeof(aitensor_t);
	self->gradients[1]->data = memory_ptr + address_counter;
	self->gradients[1]->dtype = layer->bias.dtype;
	self->gradients[1]->dim = 1;
	self->gradients[1]->shape = layer->bias.shape;
	address_counter += aimath_sizeof_tensor_data(layer->gradients[1]);
    AIFES_ALIGN_INTEGER(address_counter, AIFES_MEMORY_ALIGNMENT);
	self->gradients[1]->tensor_params = memory_ptr + address_counter;
	address_counter += aimath_sizeof_tensor_params(layer->gradients[1]);

	return;
}

#ifdef AIDEBUG_PRINT_MODULE_SPECS
AISTRING_STORAGE_WRAPPER(aistring_print_layer_specs_conv2d_1, "filter_count: ");
AISTRING_STORAGE_WRAPPER(aistring_print_layer_specs_conv2d_2, "; kernel_size: (");
AISTRING_STORAGE_WRAPPER(aistring_print_layer_specs_conv2d_3, "); stride: (");
AISTRING_STORAGE_WRAPPER(aistring_print_layer_specs_conv2d_4, "); dilation: (");
AISTRING_STORAGE_WRAPPER(aistring_print_layer_specs_conv2d_5, "); padding: (");
AISTRING_STORAGE_WRAPPER(aistring_print_layer_specs_conv2d_6, "); channel_axis: ");
AISTRING_STORAGE_WRAPPER(aistring_print_layer_specs_conv2d_7, ", ");

void ailayer_conv2d_print_specs(const ailayer_t *self)
{
    ailayer_conv2d_t *layer = (ailayer_conv2d_t *)(self->layer_configuration);

    AIPRINT(aistring_print_layer_specs_conv2d_1);
    AIPRINT_LONG_INT("%ld", (long int) layer->filter_count);
    AIPRINT(aistring_print_layer_specs_conv2d_2);
    AIPRINT_LONG_INT("%ld", (long int) layer->kernel_size[0]);
    AIPRINT(aistring_print_layer_specs_conv2d_7);
    AIPRINT_LONG_INT("%ld", (long int) layer->kernel_size[1]);

    AIPRINT(aistring_print_layer_specs_conv2d_3);
    AIPRINT_LONG_INT("%ld", (long int) layer->stride[0]);
    AIPRINT(aistring_print_layer_specs_conv2d_7);
    AIPRINT_LONG_INT("%ld", (long int) layer->stride[1]);

    AIPRINT(aistring_print_layer_specs_conv2d_4);
    AIPRINT_LONG_INT("%ld", (long int) layer->dilation[0]);
    AIPRINT(aistring_print_layer_specs_conv2d_7);
    AIPRINT_LONG_INT("%ld", (long int) layer->dilation[1]);

    AIPRINT(aistring_print_layer_specs_conv2d_5);
    AIPRINT_LONG_INT("%ld", (long int) layer->padding[0]);
    AIPRINT(aistring_print_layer_specs_conv2d_7);
    AIPRINT_LONG_INT("%ld", (long int) layer->padding[1]);

    AIPRINT(aistring_print_layer_specs_conv2d_6);
    AIPRINT_LONG_INT("%ld", (long int) layer->channel_axis);
}
#endif
