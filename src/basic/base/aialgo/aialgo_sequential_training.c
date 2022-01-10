/**
 * \file basic/base/aialgo/aialgo_sequential_training.c
 * \version 2.0alpha
 * \date 20.10.2020
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

#include "basic/base/aialgo/aialgo_sequential_training.h"
#include "basic/base/aialgo/aialgo_sequential_inference.h"

// ToDo: Remove dependency
#include "basic/default/aimath/aimath_f32_default.h"

uint32_t aialgo_sizeof_training_memory(aimodel_t *model, aiopti_t *optimizer)
{
	uint16_t i, j;
	ailayer_t *layer_ptr = model->input_layer;
	uint32_t memory = 0;

	for(i = 0; i < model->layer_count; i++)
	{
		layer_ptr->calc_result_shape(layer_ptr);

		// Memory for the quantization parameter of the intermediate results
		if(layer_ptr->calc_result_tensor_params != 0){
            // This memory is only needed if the parameters are predefined by the layer (e.g. for sigmoid layers)
            // The other result tensor parameters are in the parameter memory
            memory += layer_ptr->result.dtype->tensor_params_size;
            AIFES_ALIGN_INTEGER(memory, AIFES_MEMORY_ALIGNMENT);
        }

		// Intermediate result memory
		memory += aimath_sizeof_tensor_data(&(layer_ptr->result));
        AIFES_ALIGN_INTEGER(memory, AIFES_MEMORY_ALIGNMENT);

		// Memory for the qantization parameter of the deltas
		if(layer_ptr->output_layer->deltas.dtype != 0){
            memory += layer_ptr->output_layer->deltas.dtype->tensor_params_size;
            AIFES_ALIGN_INTEGER(memory, AIFES_MEMORY_ALIGNMENT);
		}

		// Trainingmemory e.g. for gradients
		if(layer_ptr->sizeof_trainmem != 0){
			memory += layer_ptr->sizeof_trainmem(layer_ptr);
            AIFES_ALIGN_INTEGER(memory, AIFES_MEMORY_ALIGNMENT);
		}

		// optimization memory (e.g. first or second momentum)
		if(optimizer->sizeof_optimem != 0){
			for(j = 0; j < layer_ptr->trainable_params_count; j++){
				memory += optimizer->sizeof_optimem(optimizer, layer_ptr->trainable_params[j]);
                AIFES_ALIGN_INTEGER(memory, AIFES_MEMORY_ALIGNMENT);
			}
		}

		layer_ptr = layer_ptr->output_layer;
	}
	return memory;
}

uint8_t aialgo_schedule_training_memory(aimodel_t *model, aiopti_t *optimizer, void *memory_ptr, uint32_t memory_size)
{
	uint16_t i, j;
	uint32_t address_counter = 0;
	ailayer_t *layer_ptr = model->input_layer;

	for(i = 0; i < model->layer_count; i++)
	{
		layer_ptr->calc_result_shape(layer_ptr);

		// Memory for the quantization parameter of the intermediate results
		if(layer_ptr->calc_result_tensor_params != 0){
            // This memory is only needed if the parameters are predefined by the layer (e.g. for sigmoid layers)
            // The other result tensor parameters are in the parameter memory
            layer_ptr->result.tensor_params = memory_ptr + address_counter;
            address_counter += layer_ptr->result.dtype->tensor_params_size;
            AIFES_ALIGN_INTEGER(address_counter, AIFES_MEMORY_ALIGNMENT);
        }

		// Result memory = deltas memory
		layer_ptr->result.data = memory_ptr + address_counter;
		layer_ptr->output_layer->deltas.data = memory_ptr + address_counter;
		address_counter += aimath_sizeof_tensor_data(&(layer_ptr->result));
        AIFES_ALIGN_INTEGER(address_counter, AIFES_MEMORY_ALIGNMENT);

		layer_ptr->output_layer->deltas.dtype = layer_ptr->result.dtype;
		layer_ptr->output_layer->deltas.dim = layer_ptr->result.dim;
		layer_ptr->output_layer->deltas.shape = layer_ptr->result.shape;

		// Memory for the qantization parameter of the deltas
		if(layer_ptr->output_layer->deltas.dtype != 0){
            layer_ptr->output_layer->deltas.tensor_params = memory_ptr + address_counter;
            address_counter += layer_ptr->output_layer->deltas.dtype->tensor_params_size;
            AIFES_ALIGN_INTEGER(address_counter, AIFES_MEMORY_ALIGNMENT);
		}

		// Training memory e.g. for gradients
		if(layer_ptr->sizeof_trainmem != 0){
			layer_ptr->set_trainmem(layer_ptr, memory_ptr + address_counter);
			address_counter += layer_ptr->sizeof_trainmem(layer_ptr);
            AIFES_ALIGN_INTEGER(address_counter, AIFES_MEMORY_ALIGNMENT);
		}

		// optimization memory (e.g. first or second momentum)
		if(optimizer->sizeof_optimem != 0){
			for(j = 0; j < layer_ptr->trainable_params_count; j++){
				layer_ptr->optimem[j] = memory_ptr + address_counter;
				address_counter += optimizer->sizeof_optimem(optimizer, layer_ptr->trainable_params[j]);
                AIFES_ALIGN_INTEGER(address_counter, AIFES_MEMORY_ALIGNMENT);
			}
		}

		layer_ptr = layer_ptr->output_layer;
	}

	return 0;
}

void aialgo_init_model_for_training(aimodel_t *model, aiopti_t *optimizer)
{
	uint16_t i, j;
	ailayer_t *layer_ptr = model->input_layer;

	for(i = 0; i < model->layer_count; i++)
	{
		// Init the optimization memory (e.g. setting the momentums to zero)
		for(j = 0; j < layer_ptr->trainable_params_count; j++){
			if(optimizer->init_optimem != 0){
				optimizer->init_optimem(optimizer, layer_ptr->trainable_params[j], layer_ptr->gradients[j], layer_ptr->optimem[j]);
			}
		}
		layer_ptr = layer_ptr->output_layer;
	}
	return;
}

AISTRING_STORAGE_WRAPPER(aistring_error_backward_1) = "[aialgo_backward_model] No backward function implementation in the layer\n";

void aialgo_backward_model(aimodel_t *model, aitensor_t *target_data)
{
	uint16_t i;
	ailayer_t *layer_ptr = model->output_layer;

	model->loss->calc_delta(model->loss, target_data);
	for(i = 0; i < model->layer_count; i++)
	{
#ifdef AIDEBUG_GENERAL_CHECKS
	    if(layer_ptr->backward == 0){
            AILOG_E(aistring_error_backward_1);
            return;
	    }
#endif
		layer_ptr->backward(layer_ptr);
		layer_ptr = layer_ptr->input_layer;
	}
	return;
}

void aialgo_train_model(aimodel_t *model, aitensor_t *input_tensor, aitensor_t *target_tensor, aiopti_t *optimizer, uint32_t batch_size)
{
	uint32_t i;

	aitensor_t input_batch;
	uint16_t input_batch_shape[input_tensor->dim];
	input_batch.dtype = input_tensor->dtype;
	input_batch.dim = input_tensor->dim;
	input_batch.shape = input_batch_shape;
	input_batch.tensor_params = input_tensor->tensor_params;
	aitensor_t target_batch;
	uint16_t target_batch_shape[target_tensor->dim];
	target_batch.dtype = target_tensor->dtype;
	target_batch.dim = target_tensor->dim;
	target_batch.shape = target_batch_shape;
	target_batch.tensor_params = target_tensor->tensor_params;

	uint32_t input_multiplier = 1;
	for(i = input_tensor->dim - 1; i > 0; i--)
	{
		input_multiplier *= input_tensor->shape[i];
		input_batch_shape[i] = input_tensor->shape[i];
	}
	input_multiplier *= input_tensor->dtype->size;
	input_batch_shape[0] = 1;
	uint32_t target_multiplier = 1;
	for(i = target_tensor->dim - 1; i > 0; i--)
	{
		target_multiplier *= target_tensor->shape[i];
		target_batch_shape[i] = target_tensor->shape[i];
	}
	target_multiplier *= target_tensor->dtype->size;
	target_batch_shape[0] = 1;

	model->input_layer->result.shape[0] = 1;

	uint32_t batch_count = (uint32_t) (input_tensor->shape[0] / batch_size);
	uint32_t batch;

	for(batch = 0; batch < batch_count; batch++)
	{
		aialgo_zero_gradients_model(model, optimizer);
		for(i = 0; i < batch_size; i++)
		{
			input_batch.data = input_tensor->data + batch * input_multiplier * batch_size + i * input_multiplier;
			target_batch.data = target_tensor->data + batch * target_multiplier * batch_size + i * target_multiplier;

			aialgo_forward_model(model, &input_batch);
			aialgo_backward_model(model, &target_batch);
		}

		aialgo_update_params_model(model, optimizer);
	}
	return;
}

void aialgo_calc_loss_model_f32(aimodel_t *model, aitensor_t *input_tensor, aitensor_t *target_tensor, float *result)
{
	uint32_t i;
	float loss;

	aitensor_t input_batch;
	uint16_t input_batch_shape[input_tensor->dim];
	input_batch.dtype = input_tensor->dtype;
	input_batch.dim = input_tensor->dim;
	input_batch.shape = input_batch_shape;
	input_batch.tensor_params = input_tensor->tensor_params;
	aitensor_t target_batch;
	uint16_t target_batch_shape[target_tensor->dim];
	target_batch.dtype = target_tensor->dtype;
	target_batch.dim = target_tensor->dim;
	target_batch.shape = target_batch_shape;
	target_batch.tensor_params = target_tensor->tensor_params;

	uint32_t input_multiplier = 1;
	for(i = input_tensor->dim - 1; i > 0; i--)
	{
		input_multiplier *= input_tensor->shape[i];
		input_batch_shape[i] = input_tensor->shape[i];
	}
	input_multiplier *= input_tensor->dtype->size;
	input_batch_shape[0] = 1;
	uint32_t target_multiplier = 1;
	for(i = target_tensor->dim - 1; i > 0; i--)
	{
		target_multiplier *= target_tensor->shape[i];
		target_batch_shape[i] = target_tensor->shape[i];
	}
	target_multiplier *= target_tensor->dtype->size;
	target_batch_shape[0] = 1;

	*result = 0;
	for(i = 0; i < input_tensor->shape[0]; i++)
	{
		input_batch.data = input_tensor->data + i * input_multiplier;
		target_batch.data = target_tensor->data + i * target_multiplier;

		aialgo_forward_model(model, &input_batch);
		model->loss->calc_loss(model->loss, &target_batch, &loss);
		*result += loss;
	}
	return;
}

void aialgo_calc_loss_model_q31(aimodel_t *model, aitensor_t *input_tensor, aitensor_t *target_tensor, aiscalar_q31_t *result)
{
	uint32_t i;
	aiscalar_q31_t loss = { .shift = result->shift, .zero_point = result->zero_point};

	aitensor_t input_batch;
	uint16_t input_batch_shape[input_tensor->dim];
	input_batch.dtype = input_tensor->dtype;
	input_batch.dim = input_tensor->dim;
	input_batch.shape = input_batch_shape;
	input_batch.tensor_params = input_tensor->tensor_params;
	aitensor_t target_batch;
	uint16_t target_batch_shape[target_tensor->dim];
	target_batch.dtype = target_tensor->dtype;
	target_batch.dim = target_tensor->dim;
	target_batch.shape = target_batch_shape;
	target_batch.tensor_params = target_tensor->tensor_params;

	uint32_t input_multiplier = 1;
	for(i = input_tensor->dim - 1; i > 0; i--)
	{
		input_multiplier *= input_tensor->shape[i];
		input_batch_shape[i] = input_tensor->shape[i];
	}
	input_multiplier *= input_tensor->dtype->size;
	input_batch_shape[0] = 1;
	uint32_t target_multiplier = 1;
	for(i = target_tensor->dim - 1; i > 0; i--)
	{
		target_multiplier *= target_tensor->shape[i];
		target_batch_shape[i] = target_tensor->shape[i];
	}
	target_multiplier *= target_tensor->dtype->size;
	target_batch_shape[0] = 1;

	result->value = result->zero_point;
	for(i = 0; i < input_tensor->shape[0]; i++)
	{
		input_batch.data = input_tensor->data + i * input_multiplier;
		target_batch.data = target_tensor->data + i * target_multiplier;

		aialgo_forward_model(model, &input_batch);
		model->loss->calc_loss(model->loss, &target_batch, &loss);
		result->value += loss.value - loss.zero_point;
	}
	return;
}

void aialgo_zero_gradients_model(aimodel_t *model, aiopti_t *optimizer)
{
	uint16_t i, j;
	ailayer_t *layer_ptr = model->input_layer;

	for(i = 0; i < model->layer_count; i++)
	{
		for(j = 0; j < layer_ptr->trainable_params_count; j++){
			optimizer->zero_gradients(optimizer, layer_ptr->gradients[j]);
		}
		layer_ptr = layer_ptr->output_layer;
	}
	return;
}

void aialgo_update_params_model(aimodel_t *model, aiopti_t *optimizer)
{
	uint16_t i, j;
	ailayer_t *layer_ptr = model->input_layer;

	if(optimizer->begin_step != 0){
		optimizer->begin_step(optimizer);
	}
	for(i = 0; i < model->layer_count; i++)
	{
		for(j = 0; j < layer_ptr->trainable_params_count; j++){
			optimizer->update_params(optimizer, layer_ptr->trainable_params[j], layer_ptr->gradients[j], layer_ptr->optimem[j]);
            //print_aitensor(layer_ptr->gradients[j]);
		}
		layer_ptr = layer_ptr->output_layer;
	}
	if(optimizer->end_step != 0){
		optimizer->end_step(optimizer);
	}
	return;
}

AISTRING_STORAGE_WRAPPER(aistring_print_loss_specs_1) = " (";
AISTRING_STORAGE_WRAPPER(aistring_print_loss_specs_2) = ") <";
AISTRING_STORAGE_WRAPPER(aistring_print_loss_specs_3) = ">";

void aialgo_print_loss_specs(ailoss_t *loss)
{
    AIPRINT(loss->loss_type->name);
    AIPRINT(aistring_print_loss_specs_1);
    AIPRINT(loss->connection_layer.deltas.dtype->name);
    AIPRINT(aistring_print_loss_specs_2);
	loss->loss_type->print_specs(loss);
	AIPRINT(aistring_print_loss_specs_3);
	return;
}

AISTRING_STORAGE_WRAPPER(aistring_print_optimizer_specs_1) = " (";
AISTRING_STORAGE_WRAPPER(aistring_print_optimizer_specs_2) = ") <";
AISTRING_STORAGE_WRAPPER(aistring_print_optimizer_specs_3) = ">";

void aialgo_print_optimizer_specs(aiopti_t *opti)
{
    AIPRINT(opti->optimizer_type->name);
    AIPRINT(aistring_print_optimizer_specs_1);
    AIPRINT(opti->dtype->name);
    AIPRINT(aistring_print_optimizer_specs_2);
	opti->optimizer_type->print_specs(opti);
	AIPRINT(aistring_print_optimizer_specs_3);
	return;
}
