/**
 * \file cnn/base/ailayer/ailayer_reshape.c
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

#include "cnn/base/ailayer/ailayer_reshape.h"
#include "basic/base/aimath/aimath_basic.h"

AISTRING_STORAGE_WRAPPER(aistring_layer_reshape, "Reshape");

const aicore_layertype_t ailayer_reshape_type_s = {
#ifdef AIDEBUG_PRINT_MODULE_SPECS
    .name = aistring_layer_reshape,
	.print_specs = ailayer_reshape_print_specs
#else
    .name = 0,
    .print_specs = 0
#endif
};
const aicore_layertype_t *ailayer_reshape_type = &ailayer_reshape_type_s;

ailayer_t *ailayer_reshape(ailayer_reshape_t *layer, ailayer_t *input_layer)
{
    layer->base.layer_type = ailayer_reshape_type;

	layer->base.input_layer = input_layer;
    layer->base.output_layer = 0;
	input_layer->output_layer = &(layer->base);

	layer->base.layer_configuration = layer;

    layer->base.settings = 0;
    AILAYER_SETTINGS_SET(layer->base.settings, 0b1, AILAYER_SETTINGS_KEEP_INPUT_BUFFER_FOR_RESULT, TRUE);

	layer->base.result.shape = layer->output_shape;
	layer->base.result.dim = layer->output_dim;

	layer->base.deltas.shape = input_layer->result.shape;
	layer->base.deltas.dim = input_layer->result.dim;

	layer->base.forward = ailayer_reshape_forward;
	layer->base.backward = ailayer_reshape_backward;

	layer->base.calc_result_shape = ailayer_reshape_calc_result_shape;
	layer->base.sizeof_paramem = 0;
	layer->base.set_paramem = 0;
	layer->base.sizeof_trainmem = 0;
	layer->base.set_trainmem = 0;
	layer->base.sizeof_fwdmem = 0;
	layer->base.sizeof_bwdmem = 0;

	layer->base.trainable_params_count = 0;

	ailayer_reshape_calc_result_shape(&layer->base);

	return &layer->base;
}

void ailayer_reshape_forward(ailayer_t *self)
{
	ailayer_reshape_t *layer = (ailayer_reshape_t *)(self->layer_configuration);
	aitensor_t *x_in = &(self->input_layer->result);
	aitensor_t *x_out = &(self->result);

	if(layer->reshape != 0){
        layer->reshape(x_in, x_out);
	} else {
        x_out->data = x_in->data;
	}
	return;
}


void ailayer_reshape_backward(ailayer_t *self)
{
	ailayer_reshape_t *layer = (ailayer_reshape_t *)(self->layer_configuration);
	aitensor_t *delta_in = &(self->deltas);
	aitensor_t *delta_out = &(self->output_layer->deltas);

	if(layer->reshape != 0){
        layer->reshape(delta_out, delta_in);
	} else {
        delta_in->data = delta_out->data;
	}
	return;
}

void ailayer_reshape_calc_result_shape(ailayer_t *self)
{
	ailayer_reshape_t *layer = (ailayer_reshape_t *)(self->layer_configuration);
	uint8_t i;
	uint32_t shape_acc_in, shape_acc_out;

	// Batch dimension
	self->result.shape[0] = self->input_layer->result.shape[0];

	if(layer->infer_axis != 0){
        // Calculate missing shape element at position "infer_axis"
        shape_acc_in = 1;
        for(i = 1; i < self->input_layer->result.dim; i++){
            shape_acc_in *= self->input_layer->result.shape[i];
        }
        shape_acc_out = 1;
        for(i = 1; i < self->result.dim; i++){
            if(i != layer->infer_axis){
                shape_acc_out *= self->result.shape[i];
            }
        }
        self->result.shape[layer->infer_axis] = shape_acc_in / shape_acc_out;
	}
	return;
}

#ifdef AIDEBUG_PRINT_MODULE_SPECS
AISTRING_STORAGE_WRAPPER(aistring_print_layer_specs_reshape1, "output_shape: [");
AISTRING_STORAGE_WRAPPER(aistring_print_layer_specs_reshape2, ", ");
AISTRING_STORAGE_WRAPPER(aistring_print_layer_specs_reshape3, "]");

void ailayer_reshape_print_specs(const ailayer_t *self)
{
    ailayer_reshape_t *layer = (ailayer_reshape_t *) self->layer_configuration;
    uint8_t i;

    AIPRINT(aistring_print_layer_specs_reshape1);
    AIPRINT_LONG_INT("%ld", (long int) layer->output_shape[0]);
    for(i = 1; i < layer->output_dim; i++){
        AIPRINT(aistring_print_layer_specs_reshape2);
        AIPRINT_LONG_INT("%ld", (long int) layer->output_shape[i]);
    }
    AIPRINT(aistring_print_layer_specs_reshape3);
    return;
}
#endif

