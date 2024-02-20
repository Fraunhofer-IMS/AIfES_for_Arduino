/**
 * \file basic/base/ailayer/ailayer_sigmoid.c
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

#include "basic/base/ailayer/ailayer_sigmoid.h"
#include "basic/base/aimath/aimath_basic.h"

AISTRING_STORAGE_WRAPPER(aistring_layer_sigmoid, "Sigmoid");

const aicore_layertype_t ailayer_sigmoid_type_s = {
#ifdef AIDEBUG_PRINT_MODULE_SPECS
    .name = aistring_layer_sigmoid,
	.print_specs = ailayer_sigmoid_print_specs
#else
    .name = 0,
    .print_specs = 0
#endif
};
const aicore_layertype_t *ailayer_sigmoid_type = &ailayer_sigmoid_type_s;


ailayer_t *ailayer_sigmoid(ailayer_sigmoid_t *layer, ailayer_t *input_layer)
{
    layer->base.layer_type = ailayer_sigmoid_type;

    layer->base.settings = 0;
    AILAYER_SETTINGS_SET(layer->base.settings, 0b1, AILAYER_SETTINGS_TRAINABLE, FALSE);
    AILAYER_SETTINGS_SET(layer->base.settings, 0b1, AILAYER_SETTINGS_NO_INPUT_GRADIENT, FALSE);

	layer->base.input_layer = input_layer;
    layer->base.output_layer = 0;
	input_layer->output_layer = &(layer->base);

	layer->base.layer_configuration = layer;
	layer->base.result.dim = input_layer->result.dim;
	layer->base.result.shape = input_layer->result.shape;

	layer->base.deltas.dim = 2;
	layer->base.deltas.shape = layer->base.result.shape;

	layer->base.forward = ailayer_sigmoid_forward;
	layer->base.backward = ailayer_sigmoid_backward;

	layer->base.calc_result_shape = ailayer_sigmoid_calc_result_shape;
	layer->base.sizeof_paramem = 0;
	layer->base.set_paramem = 0;
	layer->base.sizeof_trainmem = 0;
	layer->base.set_trainmem = 0;
	layer->base.sizeof_fwdmem = 0;
	layer->base.sizeof_bwdmem = ailayer_sigmoid_sizeof_bwdmem;

	layer->base.trainable_params_count = 0;

	ailayer_sigmoid_calc_result_shape(&layer->base);

	return &(layer->base);
}

void ailayer_sigmoid_forward(ailayer_t *self)
{
	ailayer_sigmoid_t *layer = (ailayer_sigmoid_t *)(self->layer_configuration);
	aitensor_t *x_in = &(self->input_layer->result);
	aitensor_t *x_out = &(self->result);

	layer->sigmoid(x_in, x_out);
	return;
}


void ailayer_sigmoid_backward(ailayer_t *self)
{
	ailayer_sigmoid_t *layer = (ailayer_sigmoid_t *)(self->layer_configuration);
	aitensor_t *delta_in = &(self->deltas);
	aitensor_t *delta_out = &(self->output_layer->deltas);
	aitensor_t *x_in = &(self->input_layer->result);

	uint32_t address_counter = 0;

	aitensor_t temp_result;
	temp_result.dim = x_in->dim;
	temp_result.shape = x_in->shape;
	temp_result.dtype = self->result.dtype;
	temp_result.tensor_params = self->tempmem;
	address_counter += aimath_sizeof_tensor_params(&self->result);
    AIFES_ALIGN_INTEGER(address_counter, AIFES_MEMORY_ALIGNMENT);
	temp_result.data = self->tempmem + address_counter;

	// delta_in = delta_out .* sigmoid'(x_in)
	layer->sigmoid(x_in, &temp_result);
	layer->d_sigmoid(&temp_result, &temp_result);
	layer->multiply(&temp_result, delta_out, delta_in);

	return;
}

void ailayer_sigmoid_calc_result_shape(ailayer_t *self)
{
	// Unused: Shape is already defined (Pointer)
	return;
}

uint32_t ailayer_sigmoid_sizeof_bwdmem(const ailayer_t *self)
{
    uint32_t memory = 0;

    memory += aimath_sizeof_tensor_params(&self->result);
    AIFES_ALIGN_INTEGER(memory, AIFES_MEMORY_ALIGNMENT);
    memory += aimath_sizeof_tensor_data(&self->result);
    AIFES_ALIGN_INTEGER(memory, AIFES_MEMORY_ALIGNMENT);

    // Shared memory buffer for d_weights and d_bias
    return memory;
}

#ifdef AIDEBUG_PRINT_MODULE_SPECS
void ailayer_sigmoid_print_specs(const ailayer_t *self)
{
    return;
}
#endif

