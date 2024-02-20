/**
 * \file cnn/default/ailayer/ailayer_maxpool2d_default.c
 * \version 2.2.0
 * \date 18.10.2021
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
 * \brief See ailayer_maxpool2D_default.h for documentation.
 * \details
 */

#include "cnn/default/ailayer/ailayer_maxpool2d_default.h"

ailayer_t *ailayer_maxpool2d_f32_default(ailayer_maxpool2d_f32_t *layer, ailayer_t *input_layer){
    layer->base.result.dtype = aif32;
    layer->base.deltas.dtype = aif32;

	layer->base.calc_result_tensor_params = 0;
	layer->base.init_params = 0;

    layer->maxpool2d_fwd = aimath_f32_default_maxpool2d_fwd;
    layer->maxpool2d_bwd = aimath_f32_default_maxpool2d_bwd;

    return ailayer_maxpool2d(layer, input_layer);
}

ailayer_t *ailayer_maxpool2d_cfirst_f32_default(ailayer_maxpool2d_f32_t *layer, ailayer_t *input_layer)
{
	layer->channel_axis = AIFES_CHANNELS_FIRST;
	return ailayer_maxpool2d_f32_default(layer, input_layer);
}

ailayer_t *ailayer_maxpool2d_chw_f32_default(ailayer_maxpool2d_f32_t *layer, ailayer_t *input_layer)
{
	layer->channel_axis = AIFES_CHANNELS_FIRST;
	return ailayer_maxpool2d_f32_default(layer, input_layer);
}

ailayer_t *ailayer_maxpool2d_clast_f32_default(ailayer_maxpool2d_f32_t *layer, ailayer_t *input_layer)
{
	layer->channel_axis = AIFES_CHANNELS_LAST;
	return ailayer_maxpool2d_f32_default(layer, input_layer);
}

ailayer_t *ailayer_maxpool2d_hwc_f32_default(ailayer_maxpool2d_f32_t *layer, ailayer_t *input_layer)
{
	layer->channel_axis = AIFES_CHANNELS_LAST;
	return ailayer_maxpool2d_f32_default(layer, input_layer);
}
