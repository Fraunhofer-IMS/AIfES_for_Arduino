/**
 * \file basic/base/ailayer/ailayer_elu.c
 * \version 2.2.0
 * \date 16.03.2021
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

#include "basic/base/ailayer/ailayer_elu.h"
#include "basic/base/aimath/aimath_basic.h"

AISTRING_STORAGE_WRAPPER(aistring_layer_elu, "ELU");

const aicore_layertype_t ailayer_elu_type_s = {
#ifdef AIDEBUG_PRINT_MODULE_SPECS
    .name = aistring_layer_elu,
	.print_specs = ailayer_elu_print_specs
#else
    .name = 0,
    .print_specs = 0
#endif
};
const aicore_layertype_t *ailayer_elu_type = &ailayer_elu_type_s;

ailayer_t *ailayer_elu(ailayer_elu_t *layer,  ailayer_t *input_layer) ////const void *beta1,
{
    layer->base.layer_type = ailayer_elu_type;

    layer->base.settings = 0;
    AILAYER_SETTINGS_SET(layer->base.settings, 0b1, AILAYER_SETTINGS_TRAINABLE, FALSE);
    AILAYER_SETTINGS_SET(layer->base.settings, 0b1, AILAYER_SETTINGS_NO_INPUT_GRADIENT, FALSE);

	layer->base.input_layer = input_layer;
    layer->base.output_layer = 0;
	input_layer->output_layer = &(layer->base);

	layer->base.layer_configuration = layer;
	layer->base.result.shape = input_layer->result.shape;
	layer->base.result.dim = input_layer->result.dim;

	layer->base.deltas.dim = 2;
	layer->base.deltas.shape = layer->base.result.shape;

	layer->base.forward = ailayer_elu_forward;
	layer->base.backward = ailayer_elu_backward;

	layer->base.calc_result_shape = ailayer_elu_calc_result_shape;
	layer->base.sizeof_paramem = 0;
	layer->base.set_paramem = 0;
	layer->base.sizeof_trainmem = 0;
	layer->base.set_trainmem = 0;
	layer->base.sizeof_fwdmem = 0;
	layer->base.sizeof_bwdmem = 0;

	layer->base.trainable_params_count = 0;

	ailayer_elu_calc_result_shape(&layer->base);

	return &(layer->base);
}

void ailayer_elu_forward(ailayer_t *self)
{
	ailayer_elu_t *layer = (ailayer_elu_t *)(self->layer_configuration);
	aitensor_t *x_in = &(self->input_layer->result);
	aitensor_t *x_out = &(self->result);

	layer->elu(x_in, layer->alpha, x_out);
	return;
}


void ailayer_elu_backward(ailayer_t *self)
{
	ailayer_elu_t *layer = (ailayer_elu_t *)(self->layer_configuration);
	aitensor_t *delta_in = &(self->deltas);
	aitensor_t *delta_out = &(self->output_layer->deltas);
	aitensor_t *x_in = &(self->input_layer->result);

	// delta_in = delta_out .* elu'(x_in)
	layer->d_elu(x_in, layer->alpha, delta_in);
	layer->multiply(delta_in, delta_out, delta_in);
	return;
}

void ailayer_elu_calc_result_shape(ailayer_t *self)
{
	// Unused: Shape is already defined (Pointer)
	return;
}

#ifdef AIDEBUG_PRINT_MODULE_SPECS
AISTRING_STORAGE_WRAPPER(aistring_print_layer_specs_elu_1, "alpha: ");

void ailayer_elu_print_specs(const ailayer_t *self)
{
    ailayer_elu_t *layer = (ailayer_elu_t *) self->layer_configuration;

    AIPRINT(aistring_print_layer_specs_elu_1);
    layer->alpha_dtype->print_aiscalar(layer->alpha);
    return;
}
#endif

