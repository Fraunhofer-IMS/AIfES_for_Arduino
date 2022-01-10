/**
 * \file basic/default/ailayer/ailayer_elu_default.c
 * \version 2.0alpha
 * \date 16.03.2021
 * \copyright  Copyright (C) 2020-2021  Fraunhofer Institute for Microelectronic Circuits and Systems.
    All rights reserved.

    AIfES is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * \brief
 * \details
 */

#include "basic/default/ailayer/ailayer_elu_default.h"

ailayer_t *ailayer_elu_f32_default(ailayer_elu_f32_t *layer, ailayer_t *input_layer)
{
	layer->base.dtype = aif32;
	layer->base.alpha = &(layer->alpha);

	layer->base.base.calc_result_tensor_params = 0;

	//forward
	layer->base.elu = aimath_f32_default_elu;

	// backward
	layer->base.d_elu = aimath_f32_default_d_elu;
	layer->base.multiply = aimath_f32_default_multiply;

	return ailayer_elu(&layer->base, input_layer);
}

ailayer_t *ailayer_elu_q31_default(ailayer_elu_q31_t *layer, ailayer_t *input_layer)
{
	layer->base.dtype = aiq31;
	layer->base.alpha = &(layer->alpha);

	layer->base.base.calc_result_tensor_params = ailayer_elu_calc_result_tensor_params_q31_default;

	//forward
	layer->base.elu = aimath_q31_default_elu;

	// backward
	layer->base.d_elu = aimath_q31_default_d_elu;
	layer->base.multiply = aimath_q31_default_multiply;

	return ailayer_elu(&layer->base, input_layer);
}

ailayer_t *ailayer_elu_q7_default(ailayer_elu_q7_t *layer, ailayer_t *input_layer)
{
    ailayer_t *return_layer;

	layer->base.dtype = aiq7;
	layer->base.alpha = &(layer->alpha);

	layer->base.base.calc_result_tensor_params = ailayer_elu_calc_result_tensor_params_q7_default;

	return_layer = ailayer_elu(&layer->base, input_layer);

	//forward
	layer->base.elu = aimath_q7_default_elu;

	// backward
	// No backward supported for q7
	return_layer->backward = 0;
	layer->base.d_elu = 0;
	layer->base.multiply = 0;

	return return_layer;
}


void ailayer_elu_calc_result_tensor_params_q31_default(ailayer_t *self)
{
	aimath_q31_params_t *qparams = (aimath_q31_params_t *) (self->result.tensor_params);

	// Values are the same as used in the elu q31 default math function
	qparams->shift = ((aimath_q31_params_t *) (self->input_layer->result.tensor_params))->shift;
	qparams->zero_point = ((aimath_q31_params_t *) (self->input_layer->result.tensor_params))->zero_point;
}

void ailayer_elu_calc_result_tensor_params_q7_default(ailayer_t *self)
{
	aimath_q7_params_t *qparams = (aimath_q7_params_t *) (self->result.tensor_params);

	// Values are the same as used in the elu q7 default math function
	qparams->shift = ((aimath_q7_params_t *) (self->input_layer->result.tensor_params))->shift;
	qparams->zero_point = ((aimath_q7_params_t *) (self->input_layer->result.tensor_params))->zero_point;
}

