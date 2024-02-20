/**
 * \file cnn/default/ailayer/ailayer_reshape_default.c
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
 * \brief See ailayer_reshape_default.h for documentation.
 * \details
 */

#include "cnn/default/ailayer/ailayer_reshape_default.h"

ailayer_t *ailayer_reshape_f32_default(ailayer_reshape_f32_t *layer, ailayer_t *input_layer){
    layer->base.result.dtype = aif32;
    layer->base.deltas.dtype = aif32;

	layer->base.calc_result_tensor_params = 0;
	layer->base.init_params = 0;

	// No operation is needed. Comparable to "view" layer in PyTorch.
	layer->reshape = 0;

    return ailayer_reshape(layer, input_layer);
}

ailayer_t *ailayer_reshape_q31_default(ailayer_reshape_q31_t *layer, ailayer_t *input_layer){
    layer->base.result.dtype = aiq31;
    layer->base.deltas.dtype = aiq31;

	layer->base.calc_result_tensor_params = 0;
	layer->base.init_params = 0;

	// No operation is needed. Comparable to "view" layer in PyTorch.
	layer->reshape = 0;

    return ailayer_reshape(layer, input_layer);
}

ailayer_t *ailayer_reshape_q7_default(ailayer_reshape_f32_t *layer, ailayer_t *input_layer){
    layer->base.result.dtype = aiq7;
    layer->base.deltas.dtype = aiq7;

	layer->base.calc_result_tensor_params = 0;
	layer->base.init_params = 0;

	// No operation is needed. Comparable to "view" layer in PyTorch.
	layer->reshape = 0;

    return ailayer_reshape(layer, input_layer);
}

ailayer_t *ailayer_flatten_f32_default(ailayer_flatten_f32_t *layer, ailayer_t *input_layer){
    layer->base.base.result.dtype = aif32;
    layer->base.base.deltas.dtype = aif32;

	layer->base.base.calc_result_tensor_params = 0;
	layer->base.base.init_params = 0;

	layer->base.output_dim = 2;
	layer->base.infer_axis = 1;
	layer->base.output_shape = layer->output_shape;

	// No operation is needed. Comparable to "view" layer in PyTorch.
	layer->base.reshape = 0;

    return ailayer_reshape(&layer->base, input_layer);
}

ailayer_t *ailayer_flatten_q31_default(ailayer_flatten_f32_t *layer, ailayer_t *input_layer){
    layer->base.base.result.dtype = aiq31;
    layer->base.base.deltas.dtype = aiq31;

	layer->base.base.calc_result_tensor_params = 0;
	layer->base.base.init_params = 0;

	layer->base.output_dim = 2;
	layer->base.infer_axis = 1;
	layer->base.output_shape = layer->output_shape;

	// No operation is needed. Comparable to "view" layer in PyTorch.
	layer->base.reshape = 0;

    return ailayer_reshape(&layer->base, input_layer);
}

ailayer_t *ailayer_flatten_q7_default(ailayer_flatten_f32_t *layer, ailayer_t *input_layer){
    layer->base.base.result.dtype = aiq7;
    layer->base.base.deltas.dtype = aiq7;

	layer->base.base.calc_result_tensor_params = 0;
	layer->base.base.init_params = 0;

	layer->base.output_dim = 2;
	layer->base.infer_axis = 1;
	layer->base.output_shape = layer->output_shape;

	// No operation is needed. Comparable to "view" layer in PyTorch.
	layer->base.reshape = 0;

    return ailayer_reshape(&layer->base, input_layer);
}
