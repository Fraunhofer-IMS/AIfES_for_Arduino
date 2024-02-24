/**
 * \file basic/base/ailayer/ailayer_input.c
 * \version 2.0alpha
 * \date 20.10.2020
 * \copyright  Copyright (C) 2020-2021  Fraunhofer Institute for Microelectronic Circuits and Systems.
    All rights reserved.

    AIfES is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * \brief
 * \details
 */

#include "basic/base/ailayer/ailayer_input.h"
#include "basic/base/aimath/aimath_basic.h"

const aicore_layertype_t ailayer_input_type_s = {
#ifdef AIDEBUG_PRINT_MODULE_SPECS
    .name = "Input",
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

	layer->base.layer_configuration = layer;
	layer->base.result.dtype = layer->dtype;
	layer->base.result.shape = layer->input_shape;
	layer->base.result.dim = layer->input_dim;

	layer->base.deltas.dtype = 0;

	layer->base.forward = ailayer_input_forward;
	layer->base.backward = ailayer_input_backward;

	layer->base.calc_result_shape = ailayer_input_calc_result_shape;
	layer->base.sizeof_paramem = 0;
	layer->base.set_paramem = 0;
	layer->base.sizeof_trainmem = 0;
	layer->base.set_trainmem = 0;

	layer->base.get_result_bound = 0;

	layer->base.trainable_params_count = 0;

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
	//ailayer_input_t *layer = (ailayer_input_t *)(self->layer_configuration);

	//self->result.shape[0] = configuration->input_shape[0];
	//self->result.shape[1] = configuration->input_shape[1];
	return;
}

#ifdef AIDEBUG_PRINT_MODULE_SPECS
void ailayer_input_print_specs(const ailayer_t *self, int (*print)(const char *format, ...))
{
    ailayer_input_t *self_casted = (ailayer_input_t *) self;
    uint8_t i = 0;

    print("Dim: %d; Shape: [%d", self_casted->input_dim, self_casted->input_shape[i]);
    for(i = 1; i < self_casted->input_dim; i++){
        print(", %d", self_casted->input_shape[i]);
    }
    print("]");
    return;
}
#endif
