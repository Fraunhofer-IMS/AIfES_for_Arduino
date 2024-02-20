/**
 * \file basic/base/ailayer/ailayer_template.c
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

#include "basic/base/ailayer/ailayer_template.h"
#include "basic/base/aimath/aimath_basic.h"

AISTRING_STORAGE_WRAPPER(aistring_layer_template, "Template");

const aicore_layertype_t ailayer_template_type_s = {
#ifdef AIDEBUG_PRINT_MODULE_SPECS
    .name = aistring_layer_template,
	.print_specs = ailayer_template_print_specs
#else
    .name = 0,
    .print_specs = 0
#endif
};
const aicore_layertype_t *ailayer_template_type = &ailayer_template_type_s;

ailayer_t *ailayer_template(ailayer_template_t *layer, ailayer_t *input_layer)
{
    layer->base.settings = 0;
    AILAYER_SETTINGS_SET(layer->base.settings, 0b1, AILAYER_SETTINGS_TRAINABLE, TRUE);
    AILAYER_SETTINGS_SET(layer->base.settings, 0b1, AILAYER_SETTINGS_NO_INPUT_GRADIENT, FALSE);

	// Connect the layer with its input
	layer->base.input_layer = input_layer;
    layer->base.output_layer = 0;
	input_layer->output_layer = &(layer->base);

	// Init basic layer variables
	layer->base.layer_configuration = layer;
	layer->base.result.shape = layer->result_shape;
	layer->base.result.dim = 2;

	layer->base.deltas.shape = input_layer->result.shape;
	layer->base.deltas.dim = 2;

	// Set the function pointers
	layer->base.forward = ailayer_template_forward;
	layer->base.backward = ailayer_template_backward;

	layer->base.calc_result_shape = ailayer_template_calc_result_shape;
	layer->base.sizeof_paramem = ailayer_template_sizeof_paramem;
	layer->base.set_paramem = ailayer_template_set_paramem;
	layer->base.sizeof_paramem = ailayer_template_sizeof_trainmem;
	layer->base.set_trainmem = ailayer_template_set_trainmem;
	layer->base.sizeof_fwdmem = 0;
	layer->base.sizeof_bwdmem = 0;

	ailayer_template_calc_result_shape(&layer->base);

	return &layer->base;
}

/*
*  Layer naming:
*               -------
*       x_in -> |  L  | -> x_out
*               |  A  |
*               |  Y  |
*               |  E  |
*   delta_in <- |  R  | <- delta_out
*               -------
*/


void ailayer_template_forward(ailayer_t *self)
{
	ailayer_template_t *layer = (ailayer_template_t *)(self->layer_configuration);
	aitensor_t *x_in = &(self->input_layer->result);
	aitensor_t *x_out = &(self->result);

	// Example forward: x_out = x_in + params
	layer->tensor_add(x_in, layer->params, x_out);

	return;
}

void ailayer_template_backward(ailayer_t *self)
{
	ailayer_template_t *layer = (ailayer_template_t *)(self->layer_configuration);
	aitensor_t *x_in = &(self->input_layer->result);
//	aitensor_t *x_out = &(self->result);
	aitensor_t *delta_in = &(self->deltas);
	aitensor_t *delta_out = &(self->output_layer->deltas);

	// Temporary result for calculation can be created here. Document the needed amount of temp memory!
	// Remove code if unused!
	float temp_result_data[delta_out->shape[0] * delta_out->shape[1]];
	uint16_t temp_result_shape[] = {delta_out->shape[0], delta_out->shape[1]};
	aitensor_t temp_result = {
		.dim = 2,
		.shape = temp_result_shape,
		.data = temp_result_data,
		.dtype = x_in->dtype
	};

	// 1) Calculate the gradients and add to the gradients tensor
	layer->copy_tensor(delta_out, &temp_result); // (Unnecessary, just show the use of temp_result)
	layer->tensor_add(layer->d_params, &temp_result, layer->d_params);

	// 2) Calculate delta for next layer. Do not before calculating gradients!!! May override x_in!!!
	layer->copy_tensor(&temp_result, delta_in);

	return;
}

void ailayer_template_calc_result_shape(ailayer_t *self)
{
//	ailayer_template_t *layer = (ailayer_template_t *)(self->layer_configuration);
	aitensor_t *x_in = &(self->input_layer->result);
	aitensor_t *x_out = &(self->result);

	x_out->shape[0] = x_in->shape[0];
	x_out->shape[1] = x_in->shape[1];

	return;
}

uint32_t ailayer_template_sizeof_paramem(const ailayer_t *self)
{
	//ailayer_template_t *layer = (ailayer_template_t *)(self->layer_configuration);
//	const aitensor_t *x_in = &(self->input_layer->result);
	const aitensor_t *x_out = &(self->result);

	uint32_t memory = 0;

	// Memory amount for params. Attention: result shape is calculated but params tensor is not available yet.
	memory += sizeof(aitensor_t);	// struct
	memory += 2 * sizeof(uint16_t);	    // shape array
	memory += x_out->shape[0] * x_out->shape[1] * aimath_sizeof_dtype(self->result.dtype); // data

	return memory;
}

void ailayer_template_set_paramem(ailayer_t *self, void *memory_ptr)
{
	ailayer_template_t *layer = (ailayer_template_t *)(self->layer_configuration);
	aitensor_t *x_in = &(self->input_layer->result);
//	aitensor_t *x_out = &(self->result);

	uint32_t address_counter = 0;

	// Params memory distribution
	// tensor struct:
	layer->params = memory_ptr + address_counter;
	address_counter += sizeof(aitensor_t);
	layer->params->dim = 2;
	// shape array:
	layer->params->shape = memory_ptr + address_counter;
	address_counter += 2 * sizeof(uint16_t);
	layer->params->shape[0] = x_in->shape[0];
	layer->params->shape[1] = x_in->shape[1];
	// data:
	layer->params->data = memory_ptr + address_counter;
	address_counter += aimath_sizeof_tensor_data(layer->params);

	return;
}

uint32_t ailayer_template_sizeof_trainmem(const ailayer_t *self)
{
	ailayer_template_t *layer = (ailayer_template_t *)(self->layer_configuration);

	uint32_t memory = 0;

	// Sum up the memory sizes for every parameter tensor
	memory += aimath_sizeof_tensor(layer->params); // Size of tensor struct and data but not the shape array, because shape array is shared with params

	return memory;
}

void ailayer_template_set_trainmem(ailayer_t *self, void *memory_ptr)
{
	ailayer_template_t *layer = (ailayer_template_t *)(self->layer_configuration);

	uint32_t address_counter = 0;

	// Params gradient memory distribution
	// tensor struct:
	layer->d_params = memory_ptr + address_counter;
	address_counter += sizeof(aitensor_t);
	layer->d_params->dim = 2;
	layer->d_params->shape = layer->params->shape; // shared shape
	// data:
	layer->d_params->data = memory_ptr + address_counter;
	address_counter += aimath_sizeof_tensor_data(layer->d_params);

	return;
}

#ifdef AIDEBUG_PRINT_MODULE_SPECS
void ailayer_template_print_specs(const ailayer_t *self)
{
    return;
}
#endif
