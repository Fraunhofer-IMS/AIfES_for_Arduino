/**
 * \file basic/base/ailayer/ailayer_softsign.c
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
 * \brief A softsign activation function.
 * \details
 */

#include "basic/base/ailayer/ailayer_softsign.h"
#include "basic/base/aimath/aimath_basic.h"

AISTRING_STORAGE_WRAPPER(aistring_layer_softsign, "Softsign");

const aicore_layertype_t ailayer_softsign_type_s = {
#ifdef AIDEBUG_PRINT_MODULE_SPECS
    .name = aistring_layer_softsign,
	.print_specs = ailayer_softsign_print_specs
#else
    .name = 0,
    .print_specs = 0
#endif
};
const aicore_layertype_t *ailayer_softsign_type = &ailayer_softsign_type_s;


ailayer_t *ailayer_softsign(ailayer_softsign_t *layer, ailayer_t *input_layer)
{
    layer->base.layer_type = ailayer_softsign_type;

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

	layer->base.forward = ailayer_softsign_forward;
	layer->base.backward = ailayer_softsign_backward;

	layer->base.calc_result_shape = ailayer_softsign_calc_result_shape;
	layer->base.sizeof_paramem = 0;
	layer->base.set_paramem = 0;
	layer->base.sizeof_trainmem = 0;
	layer->base.set_trainmem = 0;
	layer->base.sizeof_fwdmem = 0;
	layer->base.sizeof_bwdmem = 0;

	layer->base.trainable_params_count = 0;

	ailayer_softsign_calc_result_shape(&layer->base);

	return &(layer->base);
}

void ailayer_softsign_backward(ailayer_t *self)
{
	ailayer_softsign_t *layer = (ailayer_softsign_t *)(self->layer_configuration);
	aitensor_t *delta_in = &(self->deltas);
	aitensor_t *delta_out = &(self->output_layer->deltas);
	aitensor_t *x_in = &(self->input_layer->result);

	int8_t temp_result_params[aimath_sizeof_tensor_params(x_in) + 1]; // +1 to prevent array of size 0
	int8_t temp_result_data[aimath_sizeof_tensor_data(x_in)];
	aitensor_t temp_result = {
		.dim = 2,
		.shape = x_in->shape,
		.data = temp_result_data,
		.dtype = x_in->dtype,
		.tensor_params = temp_result_params
	};

	layer->softsign(x_in, &temp_result);
	layer->d_softsign(&temp_result, &temp_result);
	layer->multiply(&temp_result, delta_out, delta_in);

	return;
}

void ailayer_softsign_forward(ailayer_t *self)
{
	ailayer_softsign_t *layer = (ailayer_softsign_t *)(self->layer_configuration);
	aitensor_t *x_in = &(self->input_layer->result);
	aitensor_t *x_out = &(self->result);

	layer->softsign(x_in, x_out);
	return;
}

void ailayer_softsign_calc_result_shape(ailayer_t *self)
{
	// Unused: Shape is already defined (Pointer)
	return;
}

#ifdef AIDEBUG_PRINT_MODULE_SPECS
void ailayer_softsign_print_specs(const ailayer_t *self)
{
    return;
}
#endif
