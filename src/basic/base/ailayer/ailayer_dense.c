/**
 * \file basic/base/ailayer/ailayer_dense.c
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

#include "basic/base/ailayer/ailayer_dense.h"
#include "basic/base/aimath/aimath_basic.h"

AISTRING_STORAGE_WRAPPER(aistring_layer_dense, "Dense");

const aicore_layertype_t ailayer_dense_type_s = {
#ifdef AIDEBUG_PRINT_MODULE_SPECS
    .name = aistring_layer_dense,
	.print_specs = ailayer_dense_print_specs
#else
    .name = 0,
    .print_specs = 0
#endif
};
const aicore_layertype_t *ailayer_dense_type = &ailayer_dense_type_s;

ailayer_t *ailayer_dense(ailayer_dense_t *layer, ailayer_t *input_layer)
{
    layer->base.layer_type = ailayer_dense_type;

    layer->base.settings = 0;
    AILAYER_SETTINGS_SET(layer->base.settings, 0b1, AILAYER_SETTINGS_TRAINABLE, TRUE);
    AILAYER_SETTINGS_SET(layer->base.settings, 0b1, AILAYER_SETTINGS_NO_INPUT_GRADIENT, FALSE);

	layer->base.input_layer = input_layer;
    layer->base.output_layer = 0;
	input_layer->output_layer = &(layer->base);

	layer->base.layer_configuration = layer;
	layer->base.result.dim = 2;
	layer->base.result.shape = layer->result_shape;
	layer->base.result.shape[1] = layer->neurons;

	layer->base.deltas.dim = 2;
	layer->base.deltas.shape = input_layer->result.shape;

	layer->weights.dim = 2;
	layer->weights.shape = layer->weights_shape;
	layer->weights.shape[0] = input_layer->result.shape[1];
	layer->weights.shape[1] = layer->neurons;

	layer->bias.dim = 1;
	layer->bias.shape = layer->bias_shape;
	layer->bias.shape[0] = layer->neurons;

	layer->base.forward = ailayer_dense_forward;
	layer->base.backward = ailayer_dense_backward;

	layer->base.calc_result_shape = ailayer_dense_calc_result_shape;
	layer->base.sizeof_paramem = ailayer_dense_sizeof_paramem;
	layer->base.set_paramem = ailayer_dense_set_paramem;
	layer->base.sizeof_trainmem = ailayer_dense_sizeof_trainmem;
	layer->base.set_trainmem = ailayer_dense_set_trainmem;
	layer->base.sizeof_fwdmem = 0;
	layer->base.sizeof_bwdmem = ailayer_dense_sizeof_bwdmem;


	layer->base.trainable_params_count = 2;
	layer->base.trainable_params = layer->trainable_params;
	layer->base.gradients = layer->gradients;
	layer->base.optimem = layer->optimem;

	layer->trainable_params[0] = &layer->weights;
	layer->trainable_params[1] = &layer->bias;

	ailayer_dense_calc_result_shape(&layer->base);

	return &layer->base;
}

void ailayer_dense_forward(ailayer_t *self)
{
	aitensor_t *x_in = &(self->input_layer->result);
	aitensor_t *x_out = &(self->result);
	ailayer_dense_t *layer = (ailayer_dense_t *)(self->layer_configuration);
	aitensor_t *weights = &(layer->weights);
	aitensor_t *bias = &(layer->bias);

	// z = x * W + b
	layer->linear(x_in, weights, bias, x_out);

	return;
}


void ailayer_dense_backward(ailayer_t *self)
{
	aitensor_t *delta_in = &(self->deltas);
	aitensor_t *delta_out = &(self->output_layer->deltas);
	aitensor_t *x_in = &(self->input_layer->result);
	ailayer_dense_t *layer = (ailayer_dense_t *)(self->layer_configuration);
	aitensor_t *weights = &(layer->weights);
	aitensor_t *d_weights = layer->gradients[0];
	aitensor_t *d_bias = layer->gradients[1];

	aitensor_t temp_result;
	temp_result.data = self->tempmem;

	if(AILAYER_SETTINGS_IS(self->settings, 0b1, AILAYER_SETTINGS_TRAINABLE)){

        // d_weights += x_in^T * delta_out
        temp_result.dim             = 2;
        temp_result.shape           = d_weights->shape;
        temp_result.dtype           = d_weights->dtype;
        temp_result.tensor_params   = d_weights->tensor_params;
        layer->mat_mul_at(x_in, delta_out, &temp_result);
        layer->tensor_add(d_weights, &temp_result, d_weights);

        // d_bias_j += sum_i(delta_out_ij)
        temp_result.dim             = 1;
        temp_result.shape           = d_bias->shape;
        temp_result.dtype           = d_bias->dtype;
        temp_result.tensor_params   = d_bias->tensor_params;
        layer->sum_channelwise(delta_out, 1, &temp_result);
        layer->tensor_add(d_bias, &temp_result, d_bias);
	}

	if(!AILAYER_SETTINGS_IS(self->settings, 0b1, AILAYER_SETTINGS_NO_INPUT_GRADIENT)){
        // Calculate delta for next layer. Do not before calculating gradients!!! May override x_in!!!
        // d_in = delta_out * W^T
        layer->mat_mul_bt(delta_out, weights, delta_in);
	}

	return;
}

void ailayer_dense_calc_result_shape(ailayer_t *self)
{
	ailayer_dense_t *layer = (ailayer_dense_t *)(self->layer_configuration);

	self->result.shape[0] = self->input_layer->result.shape[0];
	self->result.shape[1] = layer->neurons;

	return;
}

uint32_t ailayer_dense_sizeof_bwdmem(const ailayer_t *self)
{
	ailayer_dense_t *layer = (ailayer_dense_t *)(self->layer_configuration);
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

uint32_t ailayer_dense_sizeof_paramem(const ailayer_t *self)
{
	uint32_t memory = 0;
	ailayer_dense_t *layer = (ailayer_dense_t *)(self->layer_configuration);

	// Weights
	memory += layer->weights.dtype->tensor_params_size;
    AIFES_ALIGN_INTEGER(memory, AIFES_MEMORY_ALIGNMENT);
	memory += self->input_layer->result.shape[1] * layer->neurons * aimath_sizeof_dtype(layer->weights.dtype); // data
    AIFES_ALIGN_INTEGER(memory, AIFES_MEMORY_ALIGNMENT);

	// Bias
	memory += layer->bias.dtype->tensor_params_size;
    AIFES_ALIGN_INTEGER(memory, AIFES_MEMORY_ALIGNMENT);
	memory += layer->neurons * aimath_sizeof_dtype(layer->bias.dtype); // data
	return memory;
}

void ailayer_dense_set_paramem(ailayer_t *self, void *memory_ptr)
{
	uint32_t address_counter = 0;
	ailayer_dense_t *layer = (ailayer_dense_t *) (self->layer_configuration);

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
	//address_counter += aimath_sizeof_tensor_data(&(layer->bias));

	layer->trainable_params[0] = &(layer->weights);
	layer->trainable_params[1] = &(layer->bias);

	return;
}

uint32_t ailayer_dense_sizeof_trainmem(const ailayer_t *self)
{
	uint32_t memory = 0;
	ailayer_dense_t *layer = (ailayer_dense_t *)(self->layer_configuration);

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

void ailayer_dense_set_trainmem(ailayer_t *self, void *memory_ptr)
{
	uint32_t address_counter = 0;
	ailayer_dense_t *layer = (ailayer_dense_t *) (self->layer_configuration);

	// Weights gradients in gradients[0]
	self->gradients[0] = memory_ptr;
	address_counter += sizeof(aitensor_t);
	self->gradients[0]->data = memory_ptr + address_counter;
	self->gradients[0]->dtype = layer->weights.dtype;
	self->gradients[0]->dim = 2;
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
AISTRING_STORAGE_WRAPPER(aistring_print_layer_specs_dense_1, "neurons: ");

void ailayer_dense_print_specs(const ailayer_t *self)
{
    ailayer_dense_t *layer = (ailayer_dense_t *)(self->layer_configuration);

    AIPRINT(aistring_print_layer_specs_dense_1);
    AIPRINT_LONG_INT("%ld", (long int) layer->neurons);
}
#endif
