/**
 * \file basic/base/ailoss/ailoss_mse.c
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

#include "basic/base/ailoss/ailoss_mse.h"
#include "basic/base/aimath/aimath_basic.h"
#include "basic/base/aimath/aimath_q31.h"

AISTRING_STORAGE_WRAPPER(aistring_loss_mse, "Mean Squared Error");

const aicore_losstype_t ailoss_mse_type_s = {
#ifdef AIDEBUG_PRINT_MODULE_SPECS
    .name = aistring_loss_mse,
	.print_specs = ailoss_mse_print_specs
#else
    .name = 0,
    .print_specs = 0
#endif
};
const aicore_losstype_t *ailoss_mse_type = &ailoss_mse_type_s;

ailoss_t *ailoss_mse(ailoss_mse_t *loss, ailayer_t *input_layer)
{
    loss->base.loss_type = ailoss_mse_type;

	loss->base.connection_layer.input_layer = input_layer;
	input_layer->output_layer = &(loss->base.connection_layer);

	loss->base.loss_configuration = loss;

	loss->base.connection_layer.deltas.dtype = loss->dtype;
	loss->base.connection_layer.deltas.dim = 2;
	loss->base.connection_layer.deltas.shape = input_layer->result.shape;

	loss->base.calc_delta = ailoss_mse_calc_delta;
	loss->base.calc_loss = ailoss_mse_calc_loss;

	return &loss->base;
}

void ailoss_mse_calc_delta(ailoss_t *self, const aitensor_t *target_data)
{
	ailoss_mse_t *loss = (ailoss_mse_t *)(self->loss_configuration);
	aitensor_t *predicted_data = &(self->connection_layer.input_layer->result);

	aitensor_t *deltas = &(self->connection_layer.deltas);

    loss->mse_gradients(predicted_data, target_data, deltas);

	return;
}

void ailoss_mse_calc_loss(ailoss_t *self, const aitensor_t *target_data, void *result)
{
	ailoss_mse_t *loss = (ailoss_mse_t *)(self->loss_configuration);

	aitensor_t *predicted_data = &(self->connection_layer.input_layer->result);

	loss->mse_loss(predicted_data, target_data, result);

	return;
}

#ifdef AIDEBUG_PRINT_MODULE_SPECS
void ailoss_mse_print_specs(const ailoss_t *self)
{
    return;
}
#endif
