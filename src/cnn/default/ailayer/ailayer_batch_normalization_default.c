/**
 * \file cnn/default/ailayer/ailayer_batch_normalization_default.c
 * \version 2.2.0
 * \date 31.01.2022
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
 * \brief See ailayer_batch_normalization_default.h for documentation.
 * \details
 */

#include "cnn/default/ailayer/ailayer_batch_normalization_default.h"
#include "cnn/default/aimath/aimath_cnn_f32_default.h"

ailayer_t *ailayer_batch_norm_f32_default(ailayer_batch_norm_f32_t *layer, ailayer_t *input_layer)
{
	layer->base.base.result.dtype = aif32;
	layer->base.base.deltas.dtype = aif32;
	layer->base.betas.dtype = aif32;
	layer->base.gammas.dtype = aif32;
	layer->base.moving_means.dtype = aif32;
	layer->base.moving_variances.dtype = aif32;

	layer->base.momentum = &layer->momentum;
	layer->base.eps = &layer->eps;

	layer->base.base.calc_result_tensor_params = 0;
	layer->base.base.init_params = ailayer_batch_norm_init_params_f32_default;

	layer->base.empirical_mean_channelwise = aimath_f32_default_mean_channelwise;
	layer->base.empirical_variance_channelwise = aimath_f32_default_variance_channelwise;
	layer->base.batch_norm = aimath_f32_default_batch_norm;
	layer->base.d_batch_norm = aimath_f32_default_d_batch_norm;
	layer->base.exponential_moving_average = aimath_f32_default_exponential_moving_average;

	return ailayer_batch_norm(&layer->base, input_layer);
}

ailayer_t *ailayer_batch_norm_cfirst_f32_default(ailayer_batch_norm_f32_t *layer, ailayer_t *input_layer)
{
	layer->base.channel_axis = AIFES_CHANNELS_FIRST;
	return ailayer_batch_norm_f32_default(layer, input_layer);
}

ailayer_t *ailayer_batch_norm_chw_f32_default(ailayer_batch_norm_f32_t *layer, ailayer_t *input_layer)
{
	layer->base.channel_axis = AIFES_CHANNELS_FIRST;
	return ailayer_batch_norm_f32_default(layer, input_layer);
}

ailayer_t *ailayer_batch_norm_cl_f32_default(ailayer_batch_norm_f32_t *layer, ailayer_t *input_layer)
{
	layer->base.channel_axis = AIFES_CHANNELS_FIRST;
	return ailayer_batch_norm_f32_default(layer, input_layer);
}

ailayer_t *ailayer_batch_norm_clast_f32_default(ailayer_batch_norm_f32_t *layer, ailayer_t *input_layer)
{
	layer->base.channel_axis = AIFES_CHANNELS_LAST;
	return ailayer_batch_norm_f32_default(layer, input_layer);
}

ailayer_t *ailayer_batch_norm_hwc_f32_default(ailayer_batch_norm_f32_t *layer, ailayer_t *input_layer)
{
	layer->base.channel_axis = AIFES_CHANNELS_LAST;
	return ailayer_batch_norm_f32_default(layer, input_layer);
}

ailayer_t *ailayer_batch_norm_lc_f32_default(ailayer_batch_norm_f32_t *layer, ailayer_t *input_layer)
{
	layer->base.channel_axis = AIFES_CHANNELS_LAST;
	return ailayer_batch_norm_f32_default(layer, input_layer);
}

void ailayer_batch_norm_init_params_f32_default(ailayer_t *self)
{
	ailayer_batch_norm_t *layer = (ailayer_batch_norm_t *) (self->layer_configuration);

	aimath_f32_default_init_zeros(&layer->moving_means);
	aimath_f32_default_init_ones(&layer->moving_variances);
	aimath_f32_default_init_zeros(&layer->betas);
	aimath_f32_default_init_ones(&layer->gammas);

	return;
}
