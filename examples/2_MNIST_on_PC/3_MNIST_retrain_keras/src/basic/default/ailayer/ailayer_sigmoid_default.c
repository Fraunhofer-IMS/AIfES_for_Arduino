/**
 * \file basic/default/ailayer/ailayer_sigmoid_default.c
 * \version 2.0alpha
 * \date 12.11.2020
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

#include "basic/default/ailayer/ailayer_sigmoid_default.h"

ailayer_t *ailayer_sigmoid_f32_default(ailayer_sigmoid_f32_t *layer, ailayer_t *input_layer)
{
	layer->dtype = aif32;

	//forward
	layer->sigmoid = aimath_f32_default_sigmoid;

	// backward
	layer->d_sigmoid = aimath_f32_default_d_sigmoid;
	layer->multiply = aimath_f32_default_multiply;

	layer->base.get_result_bound = ailayer_sigmoid_get_result_bound_f32_default;

	return ailayer_sigmoid(layer, input_layer);
}

uint8_t ailayer_sigmoid_get_result_bound_f32_default(const ailayer_t *self, const uint8_t selector, void *result_bound)
{
    float *bound = (float *) result_bound;

    switch(selector){
    case AILAYER_RESULT_LOWER_BOUND:
        *bound = 0.0f;
        return TRUE;
    case AILAYER_RESULT_UPPER_BOUND:
        *bound = 1.0f;
        return TRUE;
    case AILAYER_DELTAS_LOWER_BOUND:
        return FALSE;
    case AILAYER_DELTAS_UPPER_BOUND:
        return FALSE;
    default:
        #ifdef AIDEBUG_PRINT_ERROR_MESSAGES
            printf("\n+++ ERROR: Not defined result bound selector.\n");
        #endif // AIDEBUG_PRINT_ERROR_MESSAGES
        return FALSE;
    }
}
