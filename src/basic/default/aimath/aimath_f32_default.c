/**
 * \file basic/default/aimath/aimath_f32_default.c
 * \version 2.0alpha
 * \date 25.10.2020
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

#include "basic/default/aimath/aimath_f32_default.h"
#include <float.h>


AISTRING_STORAGE_WRAPPER(aistring_error_f32_linear_1) = "[aimath_f32_default_linear] MatMul input shapes doesn't match.\n";
AISTRING_STORAGE_WRAPPER(aistring_error_f32_linear_2) = "[aimath_f32_default_linear] MatMul output shape doesn't match.\n";

void aimath_f32_default_linear(const aitensor_t *a, const aitensor_t *b, const aitensor_t *c, aitensor_t *result)
{
	uint16_t i, j, k;
	float sum;

	float *a_data = (float *) a->data;
	float *b_data = (float *) b->data;
	float *c_data = c != 0 ? (float *) c->data : 0;
	float *result_data = (float *) result->data;

#ifdef AIDEBUG_SHAPE_CHECKS
	if(a->shape[1] != b->shape[0])
	{
		AILOG_E(aistring_error_f32_linear_1);
		return;
	}
	if(a->shape[0] != result->shape[0] || b->shape[1] != result->shape[1])
	{
		AILOG_E(aistring_error_f32_linear_2);
		return;
	}
#endif

	for(i = 0; i < a->shape[0]; i++)
	{
		for(j = 0; j < b->shape[1]; j++)
		{
			sum = 0.0f;
			for(k = 0; k < a->shape[1]; k++)
			{
				sum += a_data[i*a->shape[1] + k] * b_data[k*b->shape[1] + j];
			}
			if(c != 0){
				// Bias add
				sum += c_data[j];
			}
			result_data[i*b->shape[1] + j] = sum;
		}
	}
	return;
}

void aimath_f32_default_linear_bt(const aitensor_t *a, const aitensor_t *b, const aitensor_t *c, aitensor_t *result)
{
	uint16_t i, j, k;
	float sum;

	float *a_data = (float *) a->data;
	float *b_data = (float *) b->data;
	float *c_data = c != 0 ? (float *) c->data : 0;
	float *result_data = (float *) result->data;

#ifdef AIDEBUG_SHAPE_CHECKS
	if(a->shape[1] != b->shape[1])
	{
		AILOG_E(aistring_error_f32_linear_1);
		return;
	}
	if(a->shape[0] != result->shape[0] || b->shape[0] != result->shape[1])
	{
		AILOG_E(aistring_error_f32_linear_2);
		return;
	}
#endif

	for(i = 0; i < a->shape[0]; i++)
	{
		for(j = 0; j < b->shape[1]; j++)
		{
			sum = 0.0f;
			for(k = 0; k < a->shape[1]; k++)
			{
				sum += a_data[i*a->shape[1] + k] * b_data[j*b->shape[1] + k];
			}
			if(c != 0){
				// Bias add
				sum += c_data[j];
			}
			result_data[i*b->shape[0] + j] = sum;
		}
	}
	return;
}

void aimath_f32_default_mat_mul(const aitensor_t *a, const aitensor_t *b, aitensor_t *result){
	aimath_f32_default_linear(a, b, 0, result);
}

void aimath_f32_default_mat_mul_bt(const aitensor_t *a, const aitensor_t *b, aitensor_t *result){
	aimath_f32_default_linear_bt(a, b, 0, result);
}

void aimath_f32_default_multiply(const aitensor_t *a, const aitensor_t *b, aitensor_t *result)
{
	uint32_t i;
	for(i = 0; i < aimath_tensor_elements(a); i++)
	{
		((float *) result->data)[i] = ((float *) a->data)[i] * ((float *) b->data)[i];
	}
	return;
}

void aimath_f32_default_divide(const aitensor_t *a, const aitensor_t *b, aitensor_t *result)
{
	uint32_t i;
	for(i = 0; i < aimath_tensor_elements(a); i++)
	{
		((float *) result->data)[i] = ((float *) a->data)[i] / ((float *) b->data)[i];
	}
	return;
}

void aimath_f32_default_scalar_mul(const void *scalar, const aitensor_t *a, aitensor_t *result)
{
	uint32_t i;
	for(i = 0; i < aimath_tensor_elements(a); i++)
	{
		((float *) result->data)[i] = *((float *) scalar) * ((float *) a->data)[i];
	}
	return;
}

void aimath_f32_default_scalar_add(const void *scalar, const aitensor_t *a, aitensor_t *result)
{
	uint32_t i;
	for(i = 0; i < aimath_tensor_elements(a); i++)
	{
		((float *) result->data)[i] = *((float *) scalar) + ((float *) a->data)[i];
	}
	return;
}

/**
* Add 2D tensors
*/
void aimath_f32_default_tensor_add(const aitensor_t *a, const aitensor_t *b, aitensor_t *result)
{
	uint32_t i;
	for(i = 0; i < aimath_tensor_elements(a); i++)
	{
		((float *) result->data)[i] = ((float *) a->data)[i] + ((float *) b->data)[i];
	}
	return;
}

void aimath_f32_default_tensor_sub(const aitensor_t *a, const aitensor_t *b, aitensor_t *result)
{
	uint32_t i;
	for(i = 0; i < aimath_tensor_elements(a); i++)
	{
		((float *) result->data)[i] = ((float *) a->data)[i] - ((float *) b->data)[i];
	}
	return;
}

// only for 2D tensors
// a: f32
// b: u8
// result: f32
void aimath_f32_default_tensor_sub_sparse8(const aitensor_t *a, const aitensor_t *b, aitensor_t *result)
{
	uint32_t i, index;
	for(i = 0; i < a->shape[0]; i++)
	{
	    index = a->shape[1] * i + ((uint8_t *) b->data)[i];
		((float *) result->data)[index] = ((float *) a->data)[index] - 1.0f;
	}
	return;
}

void aimath_f32_default_copy_tensor(const aitensor_t *from, aitensor_t *to)
{
	uint32_t i;
	for(i = 0; i < aimath_tensor_elements(from); i++)
	{
		((float *) to->data)[i] = ((float *) from->data)[i];
	}
	return;
}

void aimath_f32_default_transpose_vector(aitensor_t *vector)
{
	uint16_t temp;
	temp = vector->shape[0];
	vector->shape[0] = vector->shape[1];
	vector->shape[1] = temp;
	return;
}

void aimath_f32_default_transpose_matrix(aitensor_t *x)
{
    uint16_t i, j;
    float temp_data[x->shape[0] * x->shape[1]], temp;
    aitensor_t temp_tensor = AITENSOR_2D_F32(x->shape, temp_data);

    aimath_f32_default_copy_tensor(x, &temp_tensor);

    for(i = 0; i < x->shape[0]; i++){
        for(j = 0; j < x->shape[1]; j++){
            ((float *) x->data)[j*x->shape[0] + i] = temp_data[i*x->shape[1] + j];
        }
    }

    temp = x->shape[0];
    x->shape[0] = x->shape[1];
    x->shape[1] = temp;
    return;
}

void aimath_f32_default_norm_squared(const aitensor_t *x, void *result)
{
	uint32_t i;

	*((float *) result) = 0.0f;

	for(i = 0; i < x->shape[0] * x->shape[1]; i++)
	{
		*((float *) result) += ((float *) x->data)[i] * ((float *) x->data)[i];
	}
	return;
}

void aimath_f32_default_sum(const aitensor_t *x, void *result)
{
	uint32_t i;

	*((float *) result) = 0.0f;

	for(i = 0; i < x->shape[0] * x->shape[1]; i++)
	{
		*((float *) result) += ((float *) x->data)[i];
	}
	return;
}

void aimath_f32_default_min(const aitensor_t *x, void *result)
{
    uint32_t i;
    float min_value = FLT_MAX;

	for(i = 0; i < aimath_tensor_elements(x); i++)
	{
		if(((float *) x->data)[i] < min_value){
            min_value = ((float *) x->data)[i];
		}
	}
	*((float *) result) = min_value;
	return;
}


void aimath_f32_default_max(const aitensor_t *x, void *result)
{
    uint32_t i;
    float max_value = -FLT_MAX;

	for(i = 0; i < aimath_tensor_elements(x); i++)
	{
		if(((float *) x->data)[i] > max_value){
            max_value = ((float *) x->data)[i];
		}
	}
	*((float *) result) = max_value;
	return;
}


void aimath_f32_default_sigmoid(const aitensor_t *x, aitensor_t *result)
{
	uint32_t i;

	for(i = 0; i < aimath_tensor_elements(x); i++)
	{
		((float *) result->data)[i] = 1.0f / (1.0f + expf(- ((float *) x->data)[i]));
	}
	return;
}

void aimath_f32_default_d_sigmoid(const aitensor_t *sigmoid_x, aitensor_t *result)
{
	uint32_t i;
	for(i = 0; i < aimath_tensor_elements(sigmoid_x); i++)
	{
		// sigmoid'(x) = sigmoid(x) * (1 - sigmoid(x))
		((float *) result->data)[i] = ((float *) sigmoid_x->data)[i] * (1.0f - ((float *) sigmoid_x->data)[i]);
	}
	return;
}

void aimath_f32_default_tanh(const aitensor_t *x, aitensor_t *result)
{
	uint32_t i;
	float temp;
	for(i = 0; i < aimath_tensor_elements(x); i++)
	{
	    temp = expf(((float *) x->data)[i]);
		((float *) result->data)[i] = (temp - (1.0f/temp)) / (temp + (1.0f/temp));
	}
	return;
}

void aimath_f32_default_d_tanh(const aitensor_t *tanh_x, aitensor_t *result)
{
	uint32_t i;
	for(i = 0; i < aimath_tensor_elements(tanh_x); i++)
	{
		// tanh'(x) = 1 - (tanh(x))^2
		((float *) result->data)[i] = 1.0f - (((float *) tanh_x->data)[i] * ((float *) tanh_x->data)[i]);
	}
	return;
}

void aimath_f32_default_relu(const aitensor_t *x, aitensor_t *result)
{
	uint32_t i;

	for(i = 0; i < aimath_tensor_elements(x); i++)
	{
		((float *) result->data)[i] = ((float *) x->data)[i] > 0.0f ? ((float *) x->data)[i] : 0.0f;
	}
	return;
}

void aimath_f32_default_d_relu(const aitensor_t *x, aitensor_t *result)
{
	uint32_t i;

	for(i = 0; i < aimath_tensor_elements(x); i++)
	{
		((float *) result->data)[i] = ((float *) x->data)[i] >= 0.0f ? 1.0f : 0.0f;
	}
	return;
}

void aimath_f32_default_leaky_relu(const aitensor_t *x, const void *alpha, aitensor_t *result)
{
	uint32_t i;

	for(i = 0; i < aimath_tensor_elements(x); i++)
	{
		((float *) result->data)[i] = ((float *) x->data)[i] >= 0.0f ? ((float *) x->data)[i] : ((float *) x->data)[i] * *((float *) alpha);
	}
	return;
}

void aimath_f32_default_d_leaky_relu(const aitensor_t *x, const void *alpha, aitensor_t *result)
{
	uint32_t i;

	for(i = 0; i < aimath_tensor_elements(x); i++)
	{
		((float *) result->data)[i] = ((float *) x->data)[i] >= 0.0f ? 1.0f : *((float *) alpha);
	}
	return;
}

void aimath_f32_default_elu(const aitensor_t *x, const void *alpha, aitensor_t *result)
{
	uint32_t i;

	for(i = 0; i < aimath_tensor_elements(x); i++)
	{
		((float *) result->data)[i] = ((float *) x->data)[i] > 0.0f ? ((float *) x->data)[i] : (*((float *) alpha) * (exp(((float *) x->data)[i]) - 1.0f));
	}
	return;
}

void aimath_f32_default_d_elu(const aitensor_t *x, const void *alpha, aitensor_t *result)
{
	uint32_t i;

	for(i = 0; i < aimath_tensor_elements(x); i++)
	{
		((float *) result->data)[i] = ((float *) x->data)[i] > 0.0f ? 1.0f : (*((float *) alpha) * expf(((float *) x->data)[i]));
	}
	return;
}

void aimath_f32_default_softmax(const aitensor_t *x, aitensor_t *result)
{
    uint32_t i, j;
 	float max;
 	float exp_sum;

 	float *x_data = (float *) x->data;
 	float *result_data = (float *) result->data;

 	// Multiplier for array index calculation
 	uint16_t multiplier = 1;
 	for(i = x->dim - 1; i >= 1; i--){
        multiplier *= x->shape[i];
 	}

 	// Do for every dataset. (0 is batch dimension)
 	for(i = 0; i < x->shape[0]; i++){
        // calc max value for numeric stability
        max = x_data[0];
        for(j = 0; j < multiplier; j++)
        {
            if(x_data[i * multiplier + j] > max) max = x_data[i * multiplier + j];
        }
        // calc exp functions
        exp_sum = 0.0f;
        for(j = 0; j < multiplier; j++)
        {
            result_data[i * multiplier + j] = aimath_f32_default_expf_fast(x_data[i * multiplier + j] - max);
            exp_sum += result_data[i * multiplier + j];
        }
        //calc softmax
        for(j = 0; j < multiplier; j++)
        {
            result_data[i * multiplier + j] = result_data[i * multiplier + j] / exp_sum;
        }
 	}
	return;
}

void aimath_f32_default_softsign(const aitensor_t *x, aitensor_t *result)
{
	uint32_t i;

	for(i = 0; i < aimath_tensor_elements(x); i++)
	{
		((float *) result->data)[i] = ((float *) x->data)[i] / (1.0f + fabs(((float *) x->data)[i]));
	}
	return;
}

void aimath_f32_default_d_softsign(const aitensor_t *x, aitensor_t *result)
{
	uint32_t i;

	for(i = 0; i < aimath_tensor_elements(x); i++)
	{
		((float *) result->data)[i] = 1.0f / pow((1.0f + abs(((float *) x->data)[i])), 2);
	}
	return;
}

// predicted data: f32
// target_data: f32
void aimath_f32_default_binary_crossentropy(const aitensor_t *predicted_data, const aitensor_t *target_data, void *result)
{
	uint32_t i;

	*((float *) result) = 0.0f;
	for(i = 0; i < aimath_tensor_elements(predicted_data); i++)
	{
        *((float *) result) -= ((float *) target_data->data)[i] * logf(((float *) predicted_data->data)[i])
                + (1.0f - ((float *) target_data->data)[i]) * logf(1.0f - ((float *) predicted_data->data)[i]);
	}
	return;
}

// predicted data: f32
// target_data: f32
void aimath_f32_default_categorical_crossentropy(const aitensor_t *predicted_data, const aitensor_t *target_data, void *result)
{
	uint32_t i;

	*((float *) result) = 0.0f;
	for(i = 0; i < aimath_tensor_elements(predicted_data); i++)
	{
        if(((float *) target_data->data)[i] != 0){
            *((float *) result) -= ((float *) target_data->data)[i] * logf(((float *) predicted_data->data)[i]);
        }
	}
	return;
}

// 2D tensors only
// predicted data: f32
// target_data: u8
void aimath_f32_default_categorical_crossentropy_sparse8(const aitensor_t *predicted_data, const aitensor_t *target_data, void *result)
{
	uint32_t i, index;

	*((float *) result) = 0.0f;
	for(i = 0; i < target_data->shape[0]; i++)
	{
	    index = i * predicted_data->shape[1] + ((uint8_t *) target_data->data)[i];
        *((float *) result) -= logf(((float *) predicted_data->data)[index]);
	}
	return;
}

void aimath_f32_default_sqrt(const aitensor_t *x, aitensor_t *result)
{
	uint32_t i;
	for(i = 0; i < aimath_tensor_elements(x); i++)
	{
		((float *) result->data)[i] = sqrt(((float *) x->data)[i]);
	}
	return;
}

void aimath_f32_default_zero_tensor(aitensor_t *tensor)
{
	uint32_t i;
	for(i = 0; i < aimath_tensor_elements(tensor); i++)
	{
		((float *) tensor->data)[i] = 0.0f;
	}
	return;
}

void aimath_f32_default_init_zeros(aitensor_t *tensor)
{
	aimath_f32_default_zero_tensor(tensor);
	return;
}

void aimath_f32_default_tensor_init_uniform(aitensor_t *tensor, float from, float to)
{
	uint32_t i;
	for(i = 0; i < aimath_tensor_elements(tensor); i++)
	{
		((float *) tensor->data)[i] = ((float) rand() / (float) RAND_MAX) * (to - from) + from;
	}
	return;
}

/* Glorot Uniform weight Initialization
 *
 * Glorot uniform initializer, also called Xavier uniform initializer.
 *
 * Glorot et al., 2010
 *
 * \ref http://proceedings.mlr.press/v9/glorot10a/glorot10a.pdf
 * \ref https://prateekvishnu.medium.com/xavier-and-he-normal-he-et-al-initialization-8e3d7a087528
 */
void aimath_f32_default_init_glorot_uniform(aitensor_t *tensor)
{
	float fan_in, fan_out, fan_avg;
	if(tensor->dim == 2)
	{
		fan_in = tensor->shape[0];
		fan_out = tensor->shape[1];
	}
	else if(tensor->dim == 4)
	{
		fan_in = tensor->shape[1] * tensor->shape[2] * tensor->shape[3]; // In channel * kernel_elems
		fan_out = tensor->shape[0] * tensor->shape[2] * tensor->shape[3]; // Out channel * kernel_elems
	}

	fan_avg = (fan_in + fan_out) / 2.0f;
	float r = sqrt(3.0f / fan_avg);
	aimath_f32_default_tensor_init_uniform(tensor, -r, r);
}

/* He Uniform weight Initialization
 *
 * He et al, 2015
 *
 * \ref http://arxiv.org/abs/1502.01852
 * \ref https://prateekvishnu.medium.com/xavier-and-he-normal-he-et-al-initialization-8e3d7a087528
 */
void aimath_f32_default_init_he_uniform(aitensor_t *tensor)
{
	float fan_in, fan_avg;
	if(tensor->dim == 2)
	{
		fan_in = tensor->shape[0];
	}
	else if(tensor->dim == 4)
	{
		fan_in = tensor->shape[1] * tensor->shape[2] * tensor->shape[3]; // In channel * kernel_elems
	}

	fan_avg = fan_in  / 2.0f;
	float r = sqrt(3.0f / fan_avg);
	aimath_f32_default_tensor_init_uniform(tensor, -r, r);
}

//Info(?): http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.9.4508&rep=rep1&type=pdf
//ToDo: Mit dem in der Quelle erwhnten Parameter c rumspielen
float aimath_f32_default_expf_fast(float x)
{
    if(x > 80.0f){
		x = 80.0f;
	}
	else if(x < -80.0f){
		x = -80.0f;
	}

	union { float f; int32_t x; } u;
	//max-values for a ca. [-88 , 88]
	// a = 2^23 / ln(2) ;  b = 127 * 2^23
	// x_int32 = a * x_float + b
	u.x = (int32_t) (12102203.0f * x + 1064631197.0f);
	return u.f;

}
