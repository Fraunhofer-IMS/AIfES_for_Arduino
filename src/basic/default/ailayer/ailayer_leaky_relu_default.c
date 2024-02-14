/**
 * \file basic/default/ailayer/ailayer_leaky_relu_default.c
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

#include "basic/default/ailayer/ailayer_leaky_relu_default.h"

ailayer_t *ailayer_leaky_relu_f32_default(ailayer_leaky_relu_f32_t *layer, ailayer_t *input_layer)
{
	layer->base.base.result.dtype = aif32;
	layer->base.base.deltas.dtype = aif32;
	layer->base.alpha_dtype = aif32;

	layer->base.alpha = &(layer->alpha);

	layer->base.base.calc_result_tensor_params = 0;
	layer->base.base.init_params = 0;

	//forward
	layer->base.leaky_relu = aimath_f32_default_leaky_relu;

	// backward
	layer->base.d_leaky_relu = aimath_f32_default_d_leaky_relu;
	layer->base.multiply = aimath_f32_default_multiply;

	return ailayer_leaky_relu(&layer->base, input_layer);
}

ailayer_t *ailayer_leaky_relu_q31_default(ailayer_leaky_relu_q31_t *layer, ailayer_t *input_layer)
{
	layer->base.base.result.dtype = aiq31;
	layer->base.base.deltas.dtype = aiq31;
	layer->base.alpha_dtype = aiq31;

	layer->base.alpha = &(layer->alpha);

	layer->base.base.calc_result_tensor_params = ailayer_leaky_relu_calc_result_tensor_params_q31_default;
	layer->base.base.init_params = 0;

	//forward
	layer->base.leaky_relu = aimath_q31_default_leaky_relu;

	// backward
	layer->base.d_leaky_relu = aimath_q31_default_d_leaky_relu;
	layer->base.multiply = aimath_q31_default_multiply;

	return ailayer_leaky_relu(&layer->base, input_layer);
}

ailayer_t *ailayer_leaky_relu_q7_default(ailayer_leaky_relu_q7_t *layer, ailayer_t *input_layer)
{
    ailayer_t *return_layer;

	layer->base.base.result.dtype = aiq7;
	layer->base.base.deltas.dtype = aiq7;
	layer->base.alpha_dtype = aiq7;

	layer->base.alpha = &(layer->alpha);

	layer->base.base.calc_result_tensor_params = ailayer_leaky_relu_calc_result_tensor_params_q7_default;
	layer->base.base.init_params = 0;

	return_layer = ailayer_leaky_relu(&layer->base, input_layer);

	//forward
	layer->base.leaky_relu = aimath_q7_default_leaky_relu;

	// backward
	// No backward supported for q7
	return_layer->backward = 0;
	layer->base.d_leaky_relu = 0;
	layer->base.multiply = 0;

	return return_layer;
}


void ailayer_leaky_relu_calc_result_tensor_params_q31_default(ailayer_t *self)
{
	aimath_q31_params_t *qparams = (aimath_q31_params_t *) (self->result.tensor_params);

	// Values are the same as used in the leaky relu q31 default math function
	qparams->shift = ((aimath_q31_params_t *) (self->input_layer->result.tensor_params))->shift;
	qparams->zero_point = ((aimath_q31_params_t *) (self->input_layer->result.tensor_params))->zero_point;
}

void ailayer_leaky_relu_calc_result_tensor_params_q7_default(ailayer_t *self)
{
	aimath_q7_params_t *qparams = (aimath_q7_params_t *) (self->result.tensor_params);

	// Values are the same as used in the leaky relu q7 default math function
	qparams->shift = ((aimath_q7_params_t *) (self->input_layer->result.tensor_params))->shift;
	qparams->zero_point = ((aimath_q7_params_t *) (self->input_layer->result.tensor_params))->zero_point;
}

