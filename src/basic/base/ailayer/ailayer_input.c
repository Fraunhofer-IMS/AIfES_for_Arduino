/**
 * \file basic/base/ailayer/ailayer_input.c
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

#include "basic/base/ailayer/ailayer_input.h"
#include "basic/base/aimath/aimath_basic.h"

AISTRING_STORAGE_WRAPPER(aistring_layer_input, "Input");

const aicore_layertype_t ailayer_input_type_s = {
#ifdef AIDEBUG_PRINT_MODULE_SPECS
    .name = aistring_layer_input,
	.print_specs = ailayer_input_print_specs
#else
    .name = 0,
    .print_specs = 0
#endif
};
const aicore_layertype_t *ailayer_input_type = &ailayer_input_type_s;


ailayer_t *ailayer_input(ailayer_input_t *layer)
{
    layer->base.layer_type = ailayer_input_type;

    layer->base.output_layer = 0;

    layer->base.settings = 0;
    AILAYER_SETTINGS_SET(layer->base.settings, 0b1, AILAYER_SETTINGS_TRAINABLE, FALSE);
    AILAYER_SETTINGS_SET(layer->base.settings, 0b1, AILAYER_SETTINGS_NO_INPUT_GRADIENT, TRUE);

	layer->base.layer_configuration = layer;
	layer->base.result.shape = layer->input_shape;
	layer->base.result.dim = layer->input_dim;

	layer->base.forward = ailayer_input_forward;
	layer->base.backward = ailayer_input_backward;

	layer->base.calc_result_shape = ailayer_input_calc_result_shape;
	layer->base.sizeof_paramem = 0;
	layer->base.set_paramem = 0;
	layer->base.sizeof_trainmem = 0;
	layer->base.set_trainmem = 0;
	layer->base.sizeof_fwdmem = 0;
	layer->base.sizeof_bwdmem = 0;

	layer->base.trainable_params_count = 0;

	ailayer_input_calc_result_shape(&layer->base);

	return &layer->base;
}

void ailayer_input_forward(ailayer_t *self)
{
	return;
}

void ailayer_input_backward(ailayer_t *self)
{
	return;
}

void ailayer_input_calc_result_shape(ailayer_t *self)
{
	return;
}

#ifdef AIDEBUG_PRINT_MODULE_SPECS
AISTRING_STORAGE_WRAPPER(aistring_print_layer_specs_input_1, "Dim: ");
AISTRING_STORAGE_WRAPPER(aistring_print_layer_specs_input_2, "; Shape: [");
AISTRING_STORAGE_WRAPPER(aistring_print_layer_specs_input_3, ", ");
AISTRING_STORAGE_WRAPPER(aistring_print_layer_specs_input_4, "]");

void ailayer_input_print_specs(const ailayer_t *self)
{
    ailayer_input_t *self_casted = (ailayer_input_t *) self;
    uint8_t i = 0;

    AIPRINT(aistring_print_layer_specs_input_1);
    AIPRINT_UINT("%u", (unsigned int) self_casted->input_dim);
    AIPRINT(aistring_print_layer_specs_input_2);
    AIPRINT_LONG_INT("%ld", (long int) self_casted->input_shape[i]);
    for(i = 1; i < self_casted->input_dim; i++){
        AIPRINT(aistring_print_layer_specs_input_3);
        AIPRINT_LONG_INT("%ld", (long int) self_casted->input_shape[i]);
    }
    AIPRINT(aistring_print_layer_specs_input_4);
    return;
}
#endif
