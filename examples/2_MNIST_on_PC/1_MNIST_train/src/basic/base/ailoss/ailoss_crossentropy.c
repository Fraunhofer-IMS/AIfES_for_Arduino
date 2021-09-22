/**
 * \file basic/base/ailoss/ailoss_crossentropy.c
 * \version 2.0alpha
 * \date 12.01.2021
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

#include "basic/base/ailoss/ailoss_crossentropy.h"
#include "basic/base/aimath/aimath_basic.h"

const aicore_losstype_t ailoss_crossentropy_type_s = {
#ifdef AIDEBUG_PRINT_MODULE_SPECS
    .name = "Cross-entropy",
	.print_specs = ailoss_crossentropy_print_specs
#else
    .name = 0,
    .print_specs = 0
#endif
};
const aicore_losstype_t *ailoss_crossentropy_type = &ailoss_crossentropy_type_s;

ailoss_t *ailoss_crossentropy(ailoss_crossentropy_t *loss, ailayer_t *input_layer)
{
    loss->base.loss_type = ailoss_crossentropy_type;

	loss->base.connection_layer.input_layer = input_layer;
	input_layer->output_layer = &(loss->base.connection_layer);

	loss->base.loss_configuration = loss;

	loss->base.connection_layer.deltas.dtype = loss->dtype;

	loss->base.calc_delta = ailoss_crossentropy_calc_delta;
	loss->base.calc_loss = ailoss_crossentropy_calc_loss;

	// Check for valid input and override the backward function (not needed)
	if(input_layer->layer_type == ailayer_softmax_type || input_layer->layer_type == ailayer_sigmoid_type){
        input_layer->backward = ailoss_crossentropy_dummy_backward;
    }
    else{
        #ifdef AIDEBUG_PRINT_ERROR_MESSAGES
            printf("\n!!! ERROR !!! (ailoss_crossentropy): No valid input layer. Use either Sigmoid or Softmax as input.\n");
        #endif
        return 0;
    }

	return &loss->base;
}

void ailoss_crossentropy_calc_delta(ailoss_t *self, const aitensor_t *target_data)
{
	ailoss_crossentropy_t *loss = (ailoss_crossentropy_t *)(self->loss_configuration);
	aitensor_t *predicted_data = &(self->connection_layer.input_layer->result);
	aitensor_t *deltas = &(self->connection_layer.input_layer->deltas);

	deltas->shape = predicted_data->shape; // ToDo: Maybe remove or put it in constructor

	// Calc dC/dz directly instead of dC/da_L and then da_L/dz in the output layer
	// dC/dz = (a_L - y) ( * 1/batch_size)
	loss->tensor_sub(predicted_data, target_data, deltas);

	// ToDo: Scale by batch size. (Divide / Shift)

	return;
}

void ailoss_crossentropy_calc_loss(ailoss_t *self, const aitensor_t *target_data, void *result)
{
	ailoss_crossentropy_t *loss = (ailoss_crossentropy_t *)(self->loss_configuration);

	aitensor_t *predicted_data = &(self->connection_layer.input_layer->result);

    //C = - y * log(a_L) ( * 1/batch_size)
	loss->crossentropy(predicted_data, target_data, result);

	// ToDo: Scale by batch size. (Divide / Shift)

	return;
}

void ailoss_crossentropy_dummy_backward(ailayer_t *self)
{
	return;
}


#ifdef AIDEBUG_PRINT_MODULE_SPECS
void ailoss_crossentropy_print_specs(const ailoss_t *self, int (*print)(const char *format, ...))
{
    return;
}
#endif

