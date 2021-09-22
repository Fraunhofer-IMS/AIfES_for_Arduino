/**
 * \file basic/base/ailoss/ailoss_mse.c
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

#include "basic/base/ailoss/ailoss_mse.h"
#include "basic/base/aimath/aimath_basic.h"

const aicore_losstype_t ailoss_mse_type_s = {
#ifdef AIDEBUG_PRINT_MODULE_SPECS
    .name = "Mean Squared Error",
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
	loss->base.connection_layer.get_result_bound = 0;

	loss->base.calc_delta = ailoss_mse_calc_delta;
	loss->base.calc_loss = ailoss_mse_calc_loss;

	return &loss->base;
}

void ailoss_mse_calc_delta(ailoss_t *self, const aitensor_t *target_data)
{
	ailoss_mse_t *loss = (ailoss_mse_t *)(self->loss_configuration);
	aitensor_t *predicted_data = &(self->connection_layer.input_layer->result);

	loss->tensor_sub(predicted_data, target_data, &(self->connection_layer.deltas));

	// ToDo: Scale by batch size. (Divide / Shift)

	return;
}

void ailoss_mse_calc_loss(ailoss_t *self, const aitensor_t *target_data, void *result)
{
	ailoss_mse_t *loss = (ailoss_mse_t *)(self->loss_configuration);

	aitensor_t *predicted_data = &(self->connection_layer.input_layer->result);

	uint8_t temp_tensor_data[aimath_sizeof_tensor_data(target_data)];
	aitensor_t temp_tensor = {
		.dim = target_data->dim,
		.shape = target_data->shape,
		.data = temp_tensor_data,
		.dtype = target_data->dtype,
		.tensor_params = self->connection_layer.deltas.tensor_params
	};

	loss->tensor_sub(predicted_data, target_data, &temp_tensor);
	loss->norm_squared(&temp_tensor, result);

	// ToDo: Scale by batch size. (Divide / Shift)

	return;
}

#ifdef AIDEBUG_PRINT_MODULE_SPECS
void ailoss_mse_print_specs(const ailoss_t *self, int (*print)(const char *format, ...))
{
    return;
}
#endif
