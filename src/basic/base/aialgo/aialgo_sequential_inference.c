/**
 * \file basic/base/aialgo/aialgo_sequential_inference.c
 * \version 2.0alpha
 * \date 20.10.2020
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

#include "basic/base/aialgo/aialgo_sequential_inference.h"

#include "basic/default/aimath/aimath_f32_default.h"

#include <stdio.h>
#include <float.h>
#include <string.h>

uint32_t aialgo_sizeof_inference_memory(aimodel_t *model)
{
	uint16_t i;
	uint32_t memory = 0, max_memory = 0;
	ailayer_t *layer_ptr = model->input_layer;

	// Calculate result shapes and max amount of memory
	for(i = 0; i < model->layer_count; i++)
	{
		layer_ptr->calc_result_shape(layer_ptr);
		memory = aimath_sizeof_tensor_data(&(layer_ptr->result));
		if(memory > max_memory) max_memory = memory;

		layer_ptr = layer_ptr->output_layer;
	}

	return 2 * max_memory; // Input and output buffer
}

uint32_t aialgo_sizeof_parameter_memory(aimodel_t *model)
{
	int i;
	ailayer_t *layer_ptr = model->input_layer;
	uint32_t memory = 0;

	for(i = 0; i < model->layer_count; i++)
	{
		layer_ptr->calc_result_shape(layer_ptr);

		// Memory for the quantization parameter of the intermediate results
		memory += layer_ptr->result.dtype->tensor_params_size;

        // Memory for trainable parameter
		if(layer_ptr->sizeof_paramem != 0)
		{
			memory += layer_ptr->sizeof_paramem(layer_ptr);
		}

		layer_ptr = layer_ptr->output_layer;
	}
	return memory;
}

void aialgo_distribute_parameter_memory(aimodel_t *model, void *memory_ptr, uint32_t memory_size)
{
	int i;
	ailayer_t *layer_ptr = model->input_layer;
	uint32_t address_counter = 0;

	for(i = 0; i < model->layer_count; i++)
	{
		layer_ptr->calc_result_shape(layer_ptr);

		// Memory for the quantization parameter of the intermediate results
		if(layer_ptr->result.dtype->tensor_params_size != 0){
            layer_ptr->result.tensor_params = memory_ptr + address_counter;
            address_counter += layer_ptr->result.dtype->tensor_params_size;
		}

        // Memory for trainable parameter
		if(layer_ptr->sizeof_paramem != 0)
		{
			layer_ptr->set_paramem(layer_ptr, memory_ptr + address_counter);
			address_counter += layer_ptr->sizeof_paramem(layer_ptr);
		}

		layer_ptr = layer_ptr->output_layer;
	}
	return;
}


uint8_t aialgo_schedule_inference_memory(aimodel_t *model, void *memory_ptr, uint32_t memory_size)
{
	uint16_t i;
	ailayer_t *layer_ptr = model->input_layer;

	// Init result tensor with shape and memory
	for(i = 0; i < model->layer_count; i++)
	{
		layer_ptr->calc_result_shape(layer_ptr);
		layer_ptr->result.data = memory_ptr + (i % 2) * memory_size / 2;

		layer_ptr = layer_ptr->output_layer;
	}

	return 0;
}

aitensor_t *aialgo_forward_model(aimodel_t *model, aitensor_t *input_data)
{
	uint16_t i;
	ailayer_t *layer_ptr = model->input_layer;

	model->input_layer->result.data = input_data->data;
	model->input_layer->result.tensor_params = input_data->tensor_params;
	for(i = 0; i < model->layer_count; i++)
	{
		layer_ptr->forward(layer_ptr);

		// Print intermediate results
		//print_aitensor(&layer_ptr->result);

		layer_ptr = layer_ptr->output_layer;
	}
	return &(model->output_layer->result);
}

aitensor_t *aialgo_inference_model(aimodel_t *model, aitensor_t *input_data, aitensor_t *output_data)
{
	uint32_t i, j;

	uint16_t input_batch_shape[input_data->dim];
	aitensor_t input_batch = {
	    .dtype = input_data->dtype,
        .shape = input_batch_shape,
        .dim = input_data->dim,
        .tensor_params = input_data->tensor_params
	};
	aitensor_t *output_batch;

	uint32_t input_multiplier = 1;
	for(i = input_data->dim - 1; i > 0; i--)
	{
		input_multiplier *= input_data->shape[i];
		input_batch_shape[i] = input_data->shape[i];
	}
	input_batch_shape[0] = 1;

	uint32_t output_multiplier = 1;
	for(i = output_data->dim - 1; i > 0; i--)
	{
		output_multiplier *= output_data->shape[i];
	}

	for(i = 0; i < input_data->shape[0]; i++)
	{
		input_batch.data = input_data->data + i * input_multiplier * input_data->dtype->size;

		output_batch = aialgo_forward_model(model, &input_batch);

		// ToDo: Copy tensor
		for(j = 0; j < aimath_tensor_elements(output_batch); j++)
		{
		    memcpy(output_data->data + i * output_multiplier * input_data->dtype->size,
              output_batch->data,
              aimath_tensor_elements(output_batch) * input_data->dtype->size);
		}
	}
	memcpy(output_data->tensor_params, output_batch->tensor_params, output_batch->dtype->tensor_params_size);
	return output_data;
}

uint8_t aialgo_compile_model(aimodel_t *model)
{
	ailayer_t *layer_ptr = model->input_layer;
	uint16_t layer_counter = 1;
	const uint16_t MAX_LAYER_COUNT = 128; // May be an other value

	model->trainable_params_count = 0;
	while(layer_ptr != model->output_layer && layer_counter < MAX_LAYER_COUNT)
	{
		layer_counter++;
		model->trainable_params_count += layer_ptr->trainable_params_count;

		layer_ptr = layer_ptr->output_layer;
	}
	model->layer_count = layer_counter;

	return 0;
}

void aialgo_print_model_structure(aimodel_t *model)
{
    int i;
	ailayer_t *layer_ptr = model->input_layer;

	printf("Layers:\n");
	for(i = 0; i < model->layer_count; i++){
        if(layer_ptr->layer_type->print_specs != 0){
            printf("%4d: %s (%s) <", i + 1, layer_ptr->layer_type->name, layer_ptr->result.dtype->name);
            layer_ptr->layer_type->print_specs(layer_ptr, printf);
            printf(">\n");
        } else {
            printf("%4d: No specification found for this layer.\n", i + 1);
        }
        layer_ptr = layer_ptr->output_layer;
	}
	return;
}

