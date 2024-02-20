/**
 * \file cnn/base/ailayer/ailayer_batch_normalization.c
 * \version 2.2.0
 * \date 17.01.2022
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

#include "cnn/base/ailayer/ailayer_batch_normalization.h"
#include "basic/base/aimath/aimath_basic.h"

AISTRING_STORAGE_WRAPPER(aistring_layer_batch_norm, "Batch Normalization");

const aicore_layertype_t ailayer_batch_norm_type_s = {
#ifdef AIDEBUG_PRINT_MODULE_SPECS
    .name = aistring_layer_batch_norm,
	.print_specs = ailayer_batch_norm_print_specs
#else
    .name = 0,
    .print_specs = 0
#endif
};
const aicore_layertype_t *ailayer_batch_norm_type = &ailayer_batch_norm_type_s;

ailayer_t *ailayer_batch_norm(ailayer_batch_norm_t *layer, ailayer_t *input_layer)
{
    uint8_t channel_uaxis;
    if(layer->channel_axis < 0){
        channel_uaxis = (uint8_t) input_layer->result.dim + layer->channel_axis;
    } else {
        channel_uaxis = (uint8_t)layer->channel_axis;
    }

    layer->base.layer_type = ailayer_batch_norm_type;

    layer->base.settings = 0;
    AILAYER_SETTINGS_SET(layer->base.settings, 0b1, AILAYER_SETTINGS_TRAINABLE, TRUE);
    AILAYER_SETTINGS_SET(layer->base.settings, 0b1, AILAYER_SETTINGS_NO_INPUT_GRADIENT, FALSE);

	layer->base.input_layer = input_layer;
    layer->base.output_layer = 0;
	input_layer->output_layer = &(layer->base);

	layer->base.layer_configuration = layer;
	layer->base.result.dim = input_layer->result.dim;
	layer->base.result.shape = input_layer->result.shape;

	layer->base.deltas.dim = input_layer->result.dim;
	layer->base.deltas.shape = layer->base.result.shape;

	layer->betas.dim = 1;
	layer->betas.shape = &layer->base.result.shape[channel_uaxis];

	layer->gammas.dim = 1;
	layer->gammas.shape = &layer->base.result.shape[channel_uaxis];

	layer->moving_means.dim = 1;
	layer->moving_means.shape = &layer->base.result.shape[channel_uaxis];

	layer->moving_variances.dim = 1;
	layer->moving_variances.shape = &layer->base.result.shape[channel_uaxis];

	layer->base.forward = ailayer_batch_norm_forward;
	layer->base.backward = ailayer_batch_norm_backward;

	layer->base.calc_result_shape = ailayer_batch_norm_calc_result_shape;
	layer->base.sizeof_paramem = ailayer_batch_norm_sizeof_paramem;
	layer->base.set_paramem = ailayer_batch_norm_set_paramem;
	layer->base.sizeof_trainmem = ailayer_batch_norm_sizeof_trainmem;
	layer->base.set_trainmem = ailayer_batch_norm_set_trainmem;
	layer->base.sizeof_fwdmem = 0;
	layer->base.sizeof_bwdmem = 0;

	layer->base.trainable_params_count = 2;
	layer->base.trainable_params = layer->trainable_params;
	layer->base.gradients = layer->gradients;
	layer->base.optimem = layer->optimem;

	layer->trainable_params[0] = &layer->betas;
	layer->trainable_params[1] = &layer->gammas;

	ailayer_batch_norm_calc_result_shape(&layer->base);

	return &layer->base;
}

void ailayer_batch_norm_forward(ailayer_t *self)
{
	aitensor_t *x_in = &(self->input_layer->result);
	aitensor_t *x_out = &(self->result);
	ailayer_batch_norm_t *layer = (ailayer_batch_norm_t *)(self->layer_configuration);
	aitensor_t *moving_means = &(layer->moving_means);
	aitensor_t *moving_variances = &(layer->moving_variances);
	aitensor_t *means = layer->means;
	aitensor_t *variances = layer->variances;
	aitensor_t *betas = &(layer->betas);
	aitensor_t *gammas = &(layer->gammas);
	void *momentum = layer->momentum;
	void *eps = layer->eps;

	if(AILAYER_SETTINGS_IS(self->settings, 0b1, AILAYER_SETTINGS_TRAINING_MODE)){
        // In training mode
        if(AILAYER_SETTINGS_IS(self->settings, 0b1, AILAYER_SETTINGS_BATCH_MODE)){
            // For batch mode (a whole batch is processed at once) with sufficiently large batches.
            layer->empirical_mean_channelwise(x_in, layer->channel_axis, means);
            layer->exponential_moving_average(means, momentum, moving_means);

            layer->empirical_variance_channelwise(x_in, layer->channel_axis, means, variances);
            layer->exponential_moving_average(variances, momentum, moving_variances);

            layer->batch_norm(x_in, layer->channel_axis, means, variances, betas, gammas, eps, x_out);
        } else {
            // For single-sample-mode (a batch is processed in multiple forward passes)
            // or small batches (e.g. batch size = 1).
            layer->empirical_mean_channelwise(x_in, layer->channel_axis, means);
            layer->exponential_moving_average(means, momentum, moving_means);

            layer->empirical_variance_channelwise(x_in, layer->channel_axis, moving_means, variances); // <- Moving variances here
            layer->exponential_moving_average(variances, momentum, moving_variances);

            layer->batch_norm(x_in, layer->channel_axis, moving_means, moving_variances, betas, gammas, eps, x_out);
        }
	} else {
	    // In inference mode
        layer->batch_norm(x_in, layer->channel_axis, moving_means, moving_variances, betas, gammas, eps, x_out);
	}

	return;
}


void ailayer_batch_norm_backward(ailayer_t *self)
{
	aitensor_t *delta_in = &(self->deltas);
	aitensor_t *delta_out = &(self->output_layer->deltas);
	aitensor_t *x_in = &(self->input_layer->result);
	ailayer_batch_norm_t *layer = (ailayer_batch_norm_t *)(self->layer_configuration);
	aitensor_t *means;
	aitensor_t *variances;
	aitensor_t *betas = &(layer->betas);
	aitensor_t *gammas = &(layer->gammas);
	aitensor_t *d_betas = layer->gradients[0];
	aitensor_t *d_gammas = layer->gradients[1];

    if(AILAYER_SETTINGS_IS(self->settings, 0b1, AILAYER_SETTINGS_BATCH_MODE)){
        means = layer->means;
        variances = layer->variances;
    } else {
        // A batch is split into single sample forward passes -> Use moving mean/variance
        means = &(layer->moving_means);
        variances = &(layer->moving_variances);
    }

	if(AILAYER_SETTINGS_IS(self->settings, 0b1, AILAYER_SETTINGS_TRAINABLE)){
        layer->d_batch_norm(x_in,
                            layer->channel_axis,
                            means,
                            variances,
                            betas,
                            gammas,
                            delta_out,
                            layer->eps,
                            delta_in,
                            d_betas,
                            d_gammas);
    } else {
        layer->d_batch_norm(x_in,
                            layer->channel_axis,
                            means,
                            variances,
                            betas,
                            gammas,
                            delta_out,
                            layer->eps,
                            delta_in,
                            0,
                            0);
    }

	return;
}

void ailayer_batch_norm_calc_result_shape(ailayer_t *self)
{
	// Unused: Shape is already defined (Pointer)
	return;
}

uint32_t ailayer_batch_norm_sizeof_paramem(const ailayer_t *self)
{
	uint32_t memory = 0;
	ailayer_batch_norm_t *layer = (ailayer_batch_norm_t *)(self->layer_configuration);

	// Betas
	memory += layer->betas.dtype->tensor_params_size;
    AIFES_ALIGN_INTEGER(memory, AIFES_MEMORY_ALIGNMENT);
	memory += layer->betas.shape[0] * aimath_sizeof_dtype(layer->betas.dtype); // data
    AIFES_ALIGN_INTEGER(memory, AIFES_MEMORY_ALIGNMENT);

	// Gammas
	memory += layer->gammas.dtype->tensor_params_size;
    AIFES_ALIGN_INTEGER(memory, AIFES_MEMORY_ALIGNMENT);
	memory += layer->gammas.shape[0] * aimath_sizeof_dtype(layer->gammas.dtype); // data
    AIFES_ALIGN_INTEGER(memory, AIFES_MEMORY_ALIGNMENT);

	// Moving means
	memory += layer->moving_means.dtype->tensor_params_size;
    AIFES_ALIGN_INTEGER(memory, AIFES_MEMORY_ALIGNMENT);
	memory += layer->moving_means.shape[0] * aimath_sizeof_dtype(layer->moving_means.dtype); // data
    AIFES_ALIGN_INTEGER(memory, AIFES_MEMORY_ALIGNMENT);

	// Moving variances
	memory += layer->moving_variances.dtype->tensor_params_size;
    AIFES_ALIGN_INTEGER(memory, AIFES_MEMORY_ALIGNMENT);
	memory += layer->moving_variances.shape[0] * aimath_sizeof_dtype(layer->moving_variances.dtype); // data
    AIFES_ALIGN_INTEGER(memory, AIFES_MEMORY_ALIGNMENT);

	return memory;
}

void ailayer_batch_norm_set_paramem(ailayer_t *self, void *memory_ptr)
{
	uint32_t address_counter = 0;
	ailayer_batch_norm_t *layer = (ailayer_batch_norm_t *) (self->layer_configuration);

	// Betas
	layer->betas.tensor_params = memory_ptr + address_counter;
	address_counter += layer->betas.dtype->tensor_params_size;
    AIFES_ALIGN_INTEGER(address_counter, AIFES_MEMORY_ALIGNMENT);

	layer->betas.data = memory_ptr + address_counter;
	address_counter += aimath_sizeof_tensor_data(&(layer->betas));
    AIFES_ALIGN_INTEGER(address_counter, AIFES_MEMORY_ALIGNMENT);

	// Gammas
	layer->gammas.tensor_params = memory_ptr + address_counter;
	address_counter += layer->gammas.dtype->tensor_params_size;
    AIFES_ALIGN_INTEGER(address_counter, AIFES_MEMORY_ALIGNMENT);

	layer->gammas.data = memory_ptr + address_counter;
	address_counter += aimath_sizeof_tensor_data(&(layer->gammas));
    AIFES_ALIGN_INTEGER(address_counter, AIFES_MEMORY_ALIGNMENT);

	// Moving means
	layer->moving_means.tensor_params = memory_ptr + address_counter;
	address_counter += layer->moving_means.dtype->tensor_params_size;
    AIFES_ALIGN_INTEGER(address_counter, AIFES_MEMORY_ALIGNMENT);

	layer->moving_means.data = memory_ptr + address_counter;
	address_counter += aimath_sizeof_tensor_data(&(layer->moving_means));
    AIFES_ALIGN_INTEGER(address_counter, AIFES_MEMORY_ALIGNMENT);

	// Moving variances
	layer->moving_variances.tensor_params = memory_ptr + address_counter;
	address_counter += layer->moving_variances.dtype->tensor_params_size;
    AIFES_ALIGN_INTEGER(address_counter, AIFES_MEMORY_ALIGNMENT);

	layer->moving_variances.data = memory_ptr + address_counter;
	address_counter += aimath_sizeof_tensor_data(&(layer->moving_variances));
    AIFES_ALIGN_INTEGER(address_counter, AIFES_MEMORY_ALIGNMENT);

    // Only betas and gammas are trainable
	layer->trainable_params[0] = &(layer->betas);
	layer->trainable_params[1] = &(layer->gammas);

	return;
}

uint32_t ailayer_batch_norm_sizeof_trainmem(const ailayer_t *self)
{
	uint32_t memory = 0;
	ailayer_batch_norm_t *layer = (ailayer_batch_norm_t *)(self->layer_configuration);

	// Betas gradients
	memory += sizeof(aitensor_t);
	memory += aimath_sizeof_tensor_data(&layer->betas);
    AIFES_ALIGN_INTEGER(memory, AIFES_MEMORY_ALIGNMENT);
	memory += aimath_sizeof_tensor_params(&layer->betas);
    AIFES_ALIGN_INTEGER(memory, AIFES_MEMORY_ALIGNMENT);

	// Gammas gradients
	memory += sizeof(aitensor_t);
	memory += aimath_sizeof_tensor_data(&layer->gammas);
    AIFES_ALIGN_INTEGER(memory, AIFES_MEMORY_ALIGNMENT);
	memory += aimath_sizeof_tensor_params(&layer->gammas);

	// Means
	memory += sizeof(aitensor_t);
	memory += aimath_sizeof_tensor_data(&layer->moving_means);
    AIFES_ALIGN_INTEGER(memory, AIFES_MEMORY_ALIGNMENT);

	// Variances
	memory += sizeof(aitensor_t);
	memory += aimath_sizeof_tensor_data(&layer->moving_variances);
    AIFES_ALIGN_INTEGER(memory, AIFES_MEMORY_ALIGNMENT);

	return memory;
}

void ailayer_batch_norm_set_trainmem(ailayer_t *self, void *memory_ptr)
{
	uint32_t address_counter = 0;
	ailayer_batch_norm_t *layer = (ailayer_batch_norm_t *) (self->layer_configuration);

    uint8_t channel_axis;
    if(layer->channel_axis < 0){
        channel_axis = (uint8_t) self->result.dim + layer->channel_axis;
    } else {
        channel_axis = (uint8_t) layer->channel_axis;
    }

	// Betas gradients in gradients[0]
	self->gradients[0] = memory_ptr + address_counter;
	address_counter += sizeof(aitensor_t);
    AIFES_ALIGN_INTEGER(address_counter, AIFES_MEMORY_ALIGNMENT);
	self->gradients[0]->data = memory_ptr + address_counter;
	self->gradients[0]->dtype = layer->betas.dtype;
	self->gradients[0]->dim = 1;
	self->gradients[0]->shape = &self->result.shape[channel_axis];
	address_counter += aimath_sizeof_tensor_data(layer->gradients[0]);
    AIFES_ALIGN_INTEGER(address_counter, AIFES_MEMORY_ALIGNMENT);
	self->gradients[0]->tensor_params = memory_ptr + address_counter;
	address_counter += aimath_sizeof_tensor_params(layer->gradients[0]);
    AIFES_ALIGN_INTEGER(address_counter, AIFES_MEMORY_ALIGNMENT);

	// Gammas gradients in gradients[1]
	self->gradients[1] = memory_ptr + address_counter;
	address_counter += sizeof(aitensor_t);
    AIFES_ALIGN_INTEGER(address_counter, AIFES_MEMORY_ALIGNMENT);
	self->gradients[1]->data = memory_ptr + address_counter;
	self->gradients[1]->dtype = layer->gammas.dtype;
	self->gradients[1]->dim = 1;
	self->gradients[1]->shape = &self->result.shape[channel_axis];
	address_counter += aimath_sizeof_tensor_data(layer->gradients[1]);
    AIFES_ALIGN_INTEGER(address_counter, AIFES_MEMORY_ALIGNMENT);
	self->gradients[1]->tensor_params = memory_ptr + address_counter;
	address_counter += aimath_sizeof_tensor_params(layer->gradients[1]);
    AIFES_ALIGN_INTEGER(address_counter, AIFES_MEMORY_ALIGNMENT);

	// Means
	layer->means = memory_ptr + address_counter;
	address_counter += sizeof(aitensor_t);
    AIFES_ALIGN_INTEGER(address_counter, AIFES_MEMORY_ALIGNMENT);
	layer->means->data = memory_ptr + address_counter;
	layer->means->dtype = layer->moving_means.dtype;
	layer->means->dim = 1;
	layer->means->shape = &self->result.shape[channel_axis];
	layer->means->tensor_params = layer->moving_means.tensor_params;
	address_counter += aimath_sizeof_tensor_data(layer->means);
    AIFES_ALIGN_INTEGER(address_counter, AIFES_MEMORY_ALIGNMENT);

	// Variances
	layer->variances = memory_ptr + address_counter;
	address_counter += sizeof(aitensor_t);
    AIFES_ALIGN_INTEGER(address_counter, AIFES_MEMORY_ALIGNMENT);
	layer->variances->data = memory_ptr + address_counter;
	layer->variances->dtype = layer->moving_variances.dtype;
	layer->variances->dim = 1;
	layer->variances->shape = &self->result.shape[channel_axis];
	layer->variances->tensor_params = layer->moving_variances.tensor_params;
	address_counter += aimath_sizeof_tensor_data(layer->variances);
    AIFES_ALIGN_INTEGER(address_counter, AIFES_MEMORY_ALIGNMENT);

	return;
}

#ifdef AIDEBUG_PRINT_MODULE_SPECS
AISTRING_STORAGE_WRAPPER(aistring_print_layer_specs_batch_norm_1, "momentum: ");
AISTRING_STORAGE_WRAPPER(aistring_print_layer_specs_batch_norm_2, ", eps: ");
AISTRING_STORAGE_WRAPPER(aistring_print_layer_specs_batch_norm_3, ", channel_axis: ");

void ailayer_batch_norm_print_specs(const ailayer_t *self)
{
    ailayer_batch_norm_t *layer = (ailayer_batch_norm_t *)(self->layer_configuration);

    AIPRINT(aistring_print_layer_specs_batch_norm_1);
    print_aiscalar(layer->momentum, layer->base.result.dtype);
    AIPRINT(aistring_print_layer_specs_batch_norm_2);
    print_aiscalar(layer->eps, layer->base.result.dtype);
    AIPRINT(aistring_print_layer_specs_batch_norm_3);
    AIPRINT_LONG_INT("%ld", (long int) layer->channel_axis);
}
#endif
