/**
 * \file basic/default/ailayer/ailayer_softmax_default.c
 * \version 2.0alpha
 * \date 07.12.2020
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

#include "basic/default/ailayer/ailayer_softmax_default.h"

ailayer_t *ailayer_softmax_f32_default(ailayer_softmax_f32_t *layer, ailayer_t *input_layer)
{
	layer->dtype = aif32;

	layer->base.calc_result_tensor_params = 0;

	//forward
	layer->softmax = aimath_f32_default_softmax;

	return ailayer_softmax(layer, input_layer);
}

ailayer_t *ailayer_softmax_q31_default(ailayer_softmax_q31_t *layer, ailayer_t *input_layer)
{
	layer->dtype = aiq31;

	layer->base.calc_result_tensor_params = ailayer_softmax_calc_result_tensor_params_q31_default;

	//forward
	layer->softmax = aimath_q31_default_softmax;

	return ailayer_softmax(layer, input_layer);
}

ailayer_t *ailayer_softmax_q7_default(ailayer_softmax_q7_t *layer, ailayer_t *input_layer)
{
	layer->dtype = aiq7;

	layer->base.calc_result_tensor_params = ailayer_softmax_calc_result_tensor_params_q7_default;

	//forward
	layer->softmax = aimath_q7_default_softmax;

	return ailayer_softmax(layer, input_layer);
}

void ailayer_softmax_calc_result_tensor_params_q31_default(ailayer_t *self)
{
	aimath_q31_params_t *qparams = (aimath_q31_params_t *) (self->result.tensor_params);

	// Values are the same as used in the sigmoid q31 default math function
	qparams->shift = 32;
	qparams->zero_point = -2147483647;
}

void ailayer_softmax_calc_result_tensor_params_q7_default(ailayer_t *self)
{
	aimath_q7_params_t *qparams = (aimath_q7_params_t *) (self->result.tensor_params);

	// Values are the same as used in the sigmoid q7 default math function
	qparams->shift = 8;
	qparams->zero_point = -128;
}
