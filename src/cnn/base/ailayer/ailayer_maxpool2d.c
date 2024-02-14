/**
 * \file cnn/base/ailayer/ailayer_maxpool2d.c
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

#include "cnn/base/ailayer/ailayer_maxpool2d.h"
#include "basic/base/aimath/aimath_basic.h"


AISTRING_STORAGE_WRAPPER(aistring_layer_maxpool2d, "MaxPool2D");

const aicore_layertype_t ailayer_maxpool2d_type_s = {
#ifdef AIDEBUG_PRINT_MODULE_SPECS
    .name = aistring_layer_maxpool2d,
	.print_specs = ailayer_maxpool2d_print_specs
#else
    .name = 0,
    .print_specs = 0
#endif
};
const aicore_layertype_t *ailayer_maxpool2d_type = &ailayer_maxpool2d_type_s;


AISTRING_STORAGE_WRAPPER(aistring_error_maxpool2d_1, "[ailayer_maxpool2d] Padding shape must be smaller than pool_size.\n");
AISTRING_STORAGE_WRAPPER(aistring_error_maxpool2d_2, "[ailayer_maxpool2d] Channel axis must be either 1 (-3) or 3 (-1).\n");

ailayer_t *ailayer_maxpool2d(ailayer_maxpool2d_t *layer, ailayer_t *input_layer)
{
    layer->base.layer_type = ailayer_maxpool2d_type;

    #ifdef AIDEBUG_GENERAL_CHECKS
    if(layer->padding[0] >= layer->pool_size[0] || layer->padding[1] >= layer->pool_size[1])
    {
        AILOG_E(aistring_error_maxpool2d_1);
        return 0;
    }
    if(layer->channel_axis != 1 && layer->channel_axis != -3 && layer->channel_axis != 3 && layer->channel_axis != -1){
        AILOG_E(aistring_error_maxpool2d_2);
    }
    #endif

    layer->base.settings = 0;

    // Set base params of layer
	layer->base.input_layer = input_layer;
    layer->base.output_layer = 0;
	input_layer->output_layer = &(layer->base);

	layer->base.layer_configuration = layer;
	layer->base.result.dim = 4;
	layer->base.result.shape = layer->result_shape;

	layer->base.deltas.dim = 4;
	layer->base.deltas.shape = input_layer->result.shape;
	// Set max locations to NULL, to be able to check, if max_locations has to
	// be populated during forward path
	layer->max_locations = NULL;

	// Set forward and backward function pointers
	layer->base.forward = ailayer_maxpool2d_forward;
	layer->base.backward = ailayer_maxpool2d_backward;

	// Set additional function pointers
	layer->base.calc_result_shape = ailayer_maxpool2d_calc_result_shape;
	layer->base.sizeof_trainmem = ailayer_maxpool2d_sizeof_trainmem;
	layer->base.set_trainmem = ailayer_maxpool2d_set_trainmem;
	layer->base.sizeof_fwdmem = 0;
	layer->base.sizeof_bwdmem = 0;

	layer->base.trainable_params_count = 0;

	ailayer_maxpool2d_calc_result_shape(&layer->base);

	return &layer->base;
}

void ailayer_maxpool2d_forward(ailayer_t *self)
{
	ailayer_maxpool2d_t *layer = (ailayer_maxpool2d_t *)(self->layer_configuration);
	aitensor_t *x_in = &(self->input_layer->result);
	aitensor_t *x_out = &(self->result);

	if(AILAYER_SETTINGS_IS(self->settings, 0b1, AILAYER_SETTINGS_TRAINING_MODE)){
        // Training mode -> Max locations are stored for backward pass
        layer->maxpool2d_fwd(x_in,
                             layer->pool_size,
                             layer->stride,
                             layer->padding,
                             layer->channel_axis,
                             NULL,
                             layer->max_locations,
                             x_out
                             );
    } else {
        // Inference mode -> Max locations are not stored
        layer->maxpool2d_fwd(x_in,
                             layer->pool_size,
                             layer->stride,
                             layer->padding,
                             layer->channel_axis,
                             NULL,
                             NULL,
                             x_out
                             );
    }

	return;
}

void ailayer_maxpool2d_backward(ailayer_t *self)
{
	ailayer_maxpool2d_t *layer = (ailayer_maxpool2d_t *)(self->layer_configuration);
    aitensor_t *delta_in = &(self->deltas); // Ouput of backward function is stored here
	aitensor_t *delta_out = &(self->output_layer->deltas); // Input to backward function

    layer->maxpool2d_bwd(delta_out,
                         layer->pool_size,
                         layer->stride,
                         layer->padding,
                         layer->channel_axis,
                         NULL,
                         layer->max_locations,
                         delta_in
                         );
    return;
}

void ailayer_maxpool2d_calc_result_shape(ailayer_t *self)
{
	ailayer_maxpool2d_t *layer = (ailayer_maxpool2d_t *)(self->layer_configuration);

	uint16_t s_h = layer->stride[0];
	uint16_t s_w = layer->stride[1];
	uint16_t n_h;
	uint16_t n_w;
	uint16_t k_h = layer->pool_size[0];
	uint16_t k_w = layer->pool_size[1];
    int16_t p_h = layer->padding[0];
    int16_t p_w = layer->padding[1];

    uint8_t channel_uaxis = layer->channel_axis < 0 ? 4 + layer->channel_axis : layer->channel_axis; // Negative axis = indexing from the end

	if(channel_uaxis == 1){ // Channels first
        n_h = self->input_layer->result.shape[2];
        n_w = self->input_layer->result.shape[3];

        self->result.shape[0] = self->input_layer->result.shape[0]; // N
        self->result.shape[1] = self->input_layer->result.shape[1]; // C_OUT
        self->result.shape[2] = (n_h + 2 * p_h - k_h) / s_h + 1;    // H
        self->result.shape[3] = (n_w + 2 * p_w - k_w) / s_w + 1;    // W
	} else if(channel_uaxis == 3){ // Channels last
        n_h = self->input_layer->result.shape[1];
        n_w = self->input_layer->result.shape[2];

        self->result.shape[0] = self->input_layer->result.shape[0]; // N
        self->result.shape[1] = (n_h + 2 * p_h - k_h) / s_h + 1;    // H
        self->result.shape[2] = (n_w + 2 * p_w - k_w) / s_w + 1;    // W
        self->result.shape[3] = self->input_layer->result.shape[3]; // C_OUT
	} else {
        // Error
        return;
	}

	return;
}


uint32_t ailayer_maxpool2d_sizeof_trainmem(const ailayer_t *self)
{
	uint32_t memory = 0;

	// Memory for the max_locations.
	memory += sizeof(uint32_t) * self->result.shape[0] * self->result.shape[1]  * self->result.shape[2] * self->result.shape[3];

	return memory;
}

void ailayer_maxpool2d_set_trainmem(ailayer_t *self, void *memory_ptr)
{
	ailayer_maxpool2d_t *layer = (ailayer_maxpool2d_t *) (self->layer_configuration);
	layer->max_locations = memory_ptr;

    return;
}

#ifdef AIDEBUG_PRINT_MODULE_SPECS
AISTRING_STORAGE_WRAPPER(aistring_print_layer_specs_maxpool2d_1, "pool_size: (");
AISTRING_STORAGE_WRAPPER(aistring_print_layer_specs_maxpool2d_2, "); stride: (");
AISTRING_STORAGE_WRAPPER(aistring_print_layer_specs_maxpool2d_3, "); padding: (");
AISTRING_STORAGE_WRAPPER(aistring_print_layer_specs_maxpool2d_4, "); channel_axis: ");
AISTRING_STORAGE_WRAPPER(aistring_print_layer_specs_maxpool2d_5, ", ");

void ailayer_maxpool2d_print_specs(const ailayer_t *self)
{
    ailayer_maxpool2d_t *layer = (ailayer_maxpool2d_t *)(self->layer_configuration);

    AIPRINT(aistring_print_layer_specs_maxpool2d_1);
    AIPRINT_LONG_INT("%ld", (long int) layer->pool_size[0]);
    AIPRINT(aistring_print_layer_specs_maxpool2d_5);
    AIPRINT_LONG_INT("%ld", (long int) layer->pool_size[1]);

    AIPRINT(aistring_print_layer_specs_maxpool2d_2);
    AIPRINT_LONG_INT("%ld", (long int) layer->stride[0]);
    AIPRINT(aistring_print_layer_specs_maxpool2d_5);
    AIPRINT_LONG_INT("%ld", (long int) layer->stride[1]);

    AIPRINT(aistring_print_layer_specs_maxpool2d_3);
    AIPRINT_LONG_INT("%ld", (long int) layer->padding[0]);
    AIPRINT(aistring_print_layer_specs_maxpool2d_5);
    AIPRINT_LONG_INT("%ld", (long int) layer->padding[1]);

    AIPRINT(aistring_print_layer_specs_maxpool2d_4);
    AIPRINT_LONG_INT("%ld", (long int) layer->channel_axis);
}
#endif

