/**
 * \file basic/default/ailayer/ailayer_input_default.c
 * \version 2.0alpha
 * \date 12.11.2020
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

#include "basic/default/ailayer/ailayer_input_default.h"

ailayer_t *ailayer_input_f32_default(ailayer_input_f32_t *layer)
{
	layer->dtype = aif32;

	layer->base.calc_result_tensor_params = 0;

	return ailayer_input(layer);
}

ailayer_t *ailayer_input_q31_default(ailayer_input_q31_t *layer)
{
	layer->dtype = aiq31;

	layer->base.calc_result_tensor_params = 0;

	return ailayer_input(layer);
}

ailayer_t *ailayer_input_q7_default(ailayer_input_q7_t *layer)
{
	layer->dtype = aiq7;

	layer->base.calc_result_tensor_params = 0;

	return ailayer_input(layer);
}
