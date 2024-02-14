/**
 * \file basic/base/ailoss/ailoss_crossentropy.c
 * \version 2.2.0
 * \date 12.01.2021
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

#include "basic/base/ailoss/ailoss_crossentropy.h"
#include "basic/base/aimath/aimath_basic.h"

AISTRING_STORAGE_WRAPPER(aistring_loss_crossentropy, "Cross-Entropy");

const aicore_losstype_t ailoss_crossentropy_type_s = {
#ifdef AIDEBUG_PRINT_MODULE_SPECS
    .name = aistring_loss_crossentropy,
	.print_specs = ailoss_crossentropy_print_specs
#else
    .name = 0,
    .print_specs = 0
#endif
};
const aicore_losstype_t *ailoss_crossentropy_type = &ailoss_crossentropy_type_s;

AISTRING_STORAGE_WRAPPER(aistring_error_loss_crossentropy_1, "[ailoss_crossentropy] No valid input layer. Use either Sigmoid or Softmax as input.\n");

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
            AILOG_E(aistring_error_loss_crossentropy_1);
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

	if(loss->scale_by_batch_size)
    {
        loss->scale_by_batch_size(deltas, deltas);
    }

	return;
}

void ailoss_crossentropy_calc_loss(ailoss_t *self, const aitensor_t *target_data, void *result)
{
	ailoss_crossentropy_t *loss = (ailoss_crossentropy_t *)(self->loss_configuration);

	aitensor_t *predicted_data = &(self->connection_layer.input_layer->result);

    //C = - y * log(a_L) ( * 1/batch_size)
	loss->crossentropy(predicted_data, target_data, result);

	return;
}

void ailoss_crossentropy_dummy_backward(ailayer_t *self)
{
	return;
}


#ifdef AIDEBUG_PRINT_MODULE_SPECS
void ailoss_crossentropy_print_specs(const ailoss_t *self)
{
    return;
}
#endif

