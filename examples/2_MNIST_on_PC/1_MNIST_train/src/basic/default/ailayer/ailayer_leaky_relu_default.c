/**
 * \file basic/default/ailayer/ailayer_leaky_relu_default.c
 * \version 2.0alpha
 * \date 16.03.2021
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

#include "basic/default/ailayer/ailayer_leaky_relu_default.h"

ailayer_t *ailayer_leaky_relu_f32_default(ailayer_leaky_relu_f32_t *layer, ailayer_t *input_layer)
{
	layer->base.dtype = aif32;
	layer->base.alpha = &(layer->alpha);

	//forward
	layer->base.leaky_relu = aimath_f32_default_leaky_relu;

	// backward
	layer->base.d_leaky_relu = aimath_f32_default_d_leaky_relu;
	layer->base.multiply = aimath_f32_default_multiply;

	return ailayer_leaky_relu(&layer->base, input_layer);
}
