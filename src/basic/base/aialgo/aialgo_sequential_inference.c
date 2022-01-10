/**
 * \file basic/base/aialgo/aialgo_sequential_inference.c
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

#include "basic/base/aialgo/aialgo_sequential_inference.h"

#include "basic/default/aimath/aimath_f32_default.h"
#include "basic/default/aimath/aimath_q7_default.h"

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

		memory = 0;

		// Memory for the quantization parameter of the intermediate results
		if(layer_ptr->calc_result_tensor_params != 0){
            // This memory is only needed if the parameters are predefined by the layer (e.g. for sigmoid layers)
            // The other result tensor parameters are in the parameter memory
            memory += layer_ptr->result.dtype->tensor_params_size;
        }

		memory += aimath_sizeof_tensor_data(&(layer_ptr->result));
		if(memory > max_memory) max_memory = memory;

		layer_ptr = layer_ptr->output_layer;
	}

	AIFES_ALIGN_INTEGER(max_memory, AIFES_MEMORY_ALIGNMENT);

	return 2 * max_memory; // Input and output buffer
}

uint32_t aialgo_sizeof_parameter_memory(aimodel_t *model)
{
	int i;
	ailayer_t *layer_ptr;
	uint32_t memory = 0;

	// Calculate memory in 2 separate loops to avoid wasting memory for alignment

	// 1. Calculate memory for tensor parameters of the layer results (Q7-shift, Q7-ZeroPoint, ...)
	layer_ptr = model->input_layer;
	for(i = 0; i < model->layer_count; i++)
	{
		layer_ptr->calc_result_shape(layer_ptr);

		// Memory for the quantization parameter of the intermediate results
		if(layer_ptr->calc_result_tensor_params == 0){
            // This memory is only needed if the parameters are NOT predefined by the layer (e.g. for dense layers)
            // The other result tensor parameters are in the inference of training memory
            memory += layer_ptr->result.dtype->tensor_params_size;
            AIFES_ALIGN_INTEGER(memory, AIFES_MEMORY_ALIGNMENT);
        }

		layer_ptr = layer_ptr->output_layer;
	}

	// 2. Calculate memory for trainable parameters (weights, ...)
	layer_ptr = model->input_layer;
	for(i = 0; i < model->layer_count; i++)
	{
        // Memory for trainable parameter
		if(layer_ptr->sizeof_paramem != 0)
		{
			memory += layer_ptr->sizeof_paramem(layer_ptr);
            AIFES_ALIGN_INTEGER(memory, AIFES_MEMORY_ALIGNMENT);
		}

		layer_ptr = layer_ptr->output_layer;
	}
	return memory;
}

void aialgo_distribute_parameter_memory(aimodel_t *model, void *memory_ptr, uint32_t memory_size)
{
	int i;
	ailayer_t *layer_ptr;
	uint32_t address_counter = 0;

	// Set memory in 2 separate loops to avoid wasting memory for alignment

	// 1. Distribute memory for tensor parameters (Q7-shift, Q7-ZeroPoint, ...)
	layer_ptr = model->input_layer;
	for(i = 0; i < model->layer_count; i++)
	{
		layer_ptr->calc_result_shape(layer_ptr);

		// Memory for the quantization parameter of the intermediate results
		if(layer_ptr->result.dtype->tensor_params_size != 0){
            if(layer_ptr->calc_result_tensor_params == 0){
                // This memory is only needed if the parameters are NOT predefined by the layer (e.g. for dense layers)
                // The other result tensor parameters are in the inference of training memory
                layer_ptr->result.tensor_params = memory_ptr + address_counter;
                address_counter += layer_ptr->result.dtype->tensor_params_size;
                AIFES_ALIGN_INTEGER(address_counter, AIFES_MEMORY_ALIGNMENT);
            }
		}
		layer_ptr = layer_ptr->output_layer;
	}

	// 2. Distribute memory for trainable parameters (weights, ...)
	// The memory address might be unaligned afterwards.
	layer_ptr = model->input_layer;
	for(i = 0; i < model->layer_count; i++)
	{
        // Memory for trainable parameter
		if(layer_ptr->sizeof_paramem != 0)
		{
			layer_ptr->set_paramem(layer_ptr, memory_ptr + address_counter);
			address_counter += layer_ptr->sizeof_paramem(layer_ptr);
            AIFES_ALIGN_INTEGER(address_counter, AIFES_MEMORY_ALIGNMENT);
		}

		layer_ptr = layer_ptr->output_layer;
	}
	return;
}


uint8_t aialgo_schedule_inference_memory(aimodel_t *model, void *memory_ptr, uint32_t memory_size)
{
	uint16_t i;
	void *memory_block;
	ailayer_t *layer_ptr = model->input_layer;

	// Init result tensor with shape and memory
	for(i = 0; i < model->layer_count; i++)
	{
	    memory_block = memory_ptr + (i % 2) * memory_size / 2;

		layer_ptr->calc_result_shape(layer_ptr);

		if(layer_ptr->result.dtype->tensor_params_size != 0){
            if(layer_ptr->calc_result_tensor_params != 0){
                // This memory is only needed if the parameters are predefined by the layer (e.g. for sigmoid layers)
                // The other result tensor parameters are in the parameter memory
                layer_ptr->result.tensor_params = memory_block;
            }
		}

		layer_ptr->result.data = memory_block + layer_ptr->result.dtype->tensor_params_size;

		layer_ptr = layer_ptr->output_layer;
	}

	return 0;
}

aitensor_t *aialgo_forward_model(aimodel_t *model, aitensor_t *input_data)
{
	uint16_t i;
	ailayer_t *layer_ptr = model->input_layer;

	model->input_layer->result.data = input_data->data;
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

	model->input_layer->result.shape[0] = 1;

	uint32_t output_multiplier = 1;
	for(i = output_data->dim - 1; i > 0; i--)
	{
		output_multiplier *= output_data->shape[i];
	}

	for(i = 0; i < input_data->shape[0]; i++)
	{
		input_batch.data = input_data->data + i * input_multiplier * input_data->dtype->size;

		output_batch = aialgo_forward_model(model, &input_batch);

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

void aialgo_quantize_model_f32_to_q7(aimodel_t *model_f32, aimodel_t *model_q7, aitensor_t *representative_dataset)
{
    uint32_t i, j;
    float min_value, max_value;
    float mi_ma_values[2 * model_f32->layer_count];

    ailayer_t *f32_layer_ptr, *q7_layer_ptr;

	aitensor_t input_batch;
	uint16_t input_batch_shape[representative_dataset->dim];
	input_batch.shape = input_batch_shape;

	uint32_t input_multiplier = 1;
	for(i = representative_dataset->dim - 1; i > 0; i--)
	{
		input_multiplier *= representative_dataset->shape[i];
		input_batch_shape[i] = representative_dataset->shape[i];
	}
	input_batch_shape[0] = 1;

	// Calculate min and max values of results with representative dataset
	// Init mima values
	for(i = 0; i < model_f32->layer_count; i++)
    {
        mi_ma_values[2 * i + 0] = FLT_MAX;
        mi_ma_values[2 * i + 1] = -FLT_MAX;
    }
	for(i = 0; i < representative_dataset->shape[0]; i++)
	{
		input_batch.data = (float *) representative_dataset->data + i * input_multiplier;
        model_f32->input_layer->result.data = input_batch.data;

        f32_layer_ptr = model_f32->input_layer;
        q7_layer_ptr = model_q7->input_layer;
        for(j = 0; j < model_f32->layer_count; j++)
        {
            f32_layer_ptr->forward(f32_layer_ptr);
            aimath_f32_default_min(&f32_layer_ptr->result, &min_value);
            aimath_f32_default_max(&f32_layer_ptr->result, &max_value);
            if(mi_ma_values[2 * j + 0] > min_value){
                mi_ma_values[2 * j + 0] = min_value;
            }
            if(mi_ma_values[2 * j + 1] < max_value){
                mi_ma_values[2 * j + 1] = max_value;
            }

            f32_layer_ptr = f32_layer_ptr->output_layer;
            q7_layer_ptr = q7_layer_ptr->output_layer;
        }
	}

	// Quantize results with calculated min and max values
    f32_layer_ptr = model_f32->input_layer;
    q7_layer_ptr = model_q7->input_layer;
	for(i = 0; i < model_f32->layer_count; i++)
    {
		if(q7_layer_ptr->calc_result_tensor_params == 0){
			aimath_q7_calc_q_params_from_f32(1.1f * mi_ma_values[2 * i + 0], 1.1f * mi_ma_values[2 * i + 1], q7_layer_ptr->result.tensor_params);
		} else {
		    // Tensor params are not dependent on the input data
			q7_layer_ptr->calc_result_tensor_params(q7_layer_ptr);
		}

        f32_layer_ptr = f32_layer_ptr->output_layer;
        q7_layer_ptr = q7_layer_ptr->output_layer;
    }

    // Quantize params
    f32_layer_ptr = model_f32->input_layer;
    q7_layer_ptr = model_q7->input_layer;
    for(i = 0; i < model_f32->layer_count; i++)
    {
        if(q7_layer_ptr->calc_result_tensor_params != 0) q7_layer_ptr->calc_result_tensor_params(q7_layer_ptr);

        if(f32_layer_ptr->layer_type == ailayer_dense_type){
            // quantize weights to q7 and bias to q31
            ailayer_dense_quantize_q7_from_f32((ailayer_dense_t *) f32_layer_ptr->layer_configuration, (ailayer_dense_t *) q7_layer_ptr->layer_configuration);
        }
        else{
            // Default quantization: scale: max-miin; zero_point: 0
            for(j = 0; j < f32_layer_ptr->trainable_params_count; j++){
                aimath_f32_default_min(f32_layer_ptr->trainable_params[j], &min_value);
                aimath_f32_default_max(f32_layer_ptr->trainable_params[j], &max_value);
                if(max_value < -min_value){
                    max_value = -min_value;
                }
                aimath_q7_calc_q_params_from_f32(-max_value, max_value, q7_layer_ptr->trainable_params[j]->tensor_params);
                aimath_q7_quantize_tensor_from_f32(f32_layer_ptr->trainable_params[j], q7_layer_ptr->trainable_params[j]);
            }
        }


        f32_layer_ptr = f32_layer_ptr->output_layer;
        q7_layer_ptr = q7_layer_ptr->output_layer;
    }
	return;
}

void aialgo_set_model_result_precision_q31(aimodel_t *model, uint16_t shift)
{
	int i;
	ailayer_t *layer_ptr = model->input_layer;

	for(i = 0; i < model->layer_count; i++){
	    if(layer_ptr->calc_result_tensor_params == 0){
            ((aimath_q31_params_t *) layer_ptr->result.tensor_params)->shift = shift;
            ((aimath_q31_params_t *) layer_ptr->result.tensor_params)->zero_point = 0;
	    }

		layer_ptr = layer_ptr->output_layer;
	}
	return;
}

AISTRING_STORAGE_WRAPPER(aistring_error_set_model_delta_precision_q31_1) = "[aialgo_set_model_delta_precision_q31] No loss defined in the model.\n";

void aialgo_set_model_delta_precision_q31(aimodel_t *model, uint16_t shift)
{
	int i;
	ailayer_t *layer_ptr = model->input_layer->output_layer;

	if(model->output_layer->output_layer != 0){
        for(i = 0; i < model->layer_count; i++){
            ((aimath_q31_params_t *) layer_ptr->deltas.tensor_params)->shift = shift;
            ((aimath_q31_params_t *) layer_ptr->deltas.tensor_params)->zero_point = 0;

            layer_ptr = layer_ptr->output_layer;
        }
	}
    else {
        // No loss defined in the model
        #ifdef AIDEBUG_PRINT_ERROR_MESSAGES
            AILOG_E(aistring_error_set_model_delta_precision_q31_1);
        #endif // AIDEBUG_PRINT_ERROR_MESSAGES
    }
	return;
}

void aialgo_set_model_gradient_precision_q31(aimodel_t *model, uint16_t shift)
{
	int i, j;
	ailayer_t *layer_ptr = model->input_layer;

	for(i = 0; i < model->layer_count; i++){
        for(j = 0; j < layer_ptr->trainable_params_count; j++){
            ((aimath_q31_params_t *) layer_ptr->gradients[j]->tensor_params)->shift = shift;
            ((aimath_q31_params_t *) layer_ptr->gradients[j]->tensor_params)->zero_point = 0;
        }

		layer_ptr = layer_ptr->output_layer;
	}
	return;
}

AISTRING_STORAGE_WRAPPER(aistring_print_model_structure_1) = ": ";
AISTRING_STORAGE_WRAPPER(aistring_print_model_structure_2) = " (";
AISTRING_STORAGE_WRAPPER(aistring_print_model_structure_3) = ") <";
AISTRING_STORAGE_WRAPPER(aistring_print_model_structure_4) = ">\n";
AISTRING_STORAGE_WRAPPER(aistring_print_model_structure_5) = ": No specification found for this layer.\n";

void aialgo_print_model_structure(aimodel_t *model)
{
    int i;
	ailayer_t *layer_ptr = model->input_layer;

	for(i = 0; i < model->layer_count; i++){
        if(layer_ptr->layer_type->print_specs != 0){
            AIPRINT_INT("%4d", i + 1);
            AIPRINT(aistring_print_model_structure_1);
            AIPRINT(layer_ptr->layer_type->name);
            AIPRINT(aistring_print_model_structure_2);
            AIPRINT(layer_ptr->result.dtype->name);
            AIPRINT(aistring_print_model_structure_3);
            layer_ptr->layer_type->print_specs(layer_ptr);
            AIPRINT(aistring_print_model_structure_4);
        } else {
            AIPRINT_INT("%4d", i + 1);
            AIPRINT(aistring_print_model_structure_5);
        }
        layer_ptr = layer_ptr->output_layer;
	}
	return;
}

