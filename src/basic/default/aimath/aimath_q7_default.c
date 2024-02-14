/**
 * \file basic/default/aimath/aimath_q7_default.c
 * \version 2.2.0
 * \date 28.10.2020
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

#include "basic/default/aimath/aimath_q7_default.h"


AISTRING_STORAGE_WRAPPER(aistring_error_q7_linear32_1, "[aimath_q7_default_linear32] MatMul input shapes doesn't match.\n");
AISTRING_STORAGE_WRAPPER(aistring_error_q7_linear32_2, "[aimath_q7_default_linear32] MatMul output shape doesn't match.\n");
AISTRING_STORAGE_WRAPPER(aistring_error_q7_linear32_3, "[aimath_q7_default_linear32] Third operand shift does not match.\n");

void aimath_q7_default_linear32(const aitensor_t *a, const aitensor_t *b, const aitensor_t *c, aitensor_t *result)
{
	uint16_t i, j, k;
	int32_t sum, acc; // 16-bit accumulator
	uint16_t a_shift = ((aimath_q7_params_t *) a->tensor_params)->shift;
	uint16_t b_shift = ((aimath_q7_params_t *) b->tensor_params)->shift;
	uint16_t c_shift = ((aimath_q31_params_t *) c->tensor_params)->shift;
	uint16_t result_shift = ((aimath_q7_params_t *) result->tensor_params)->shift;

	int8_t z_a = ((aimath_q7_params_t *) a->tensor_params)->zero_point;
	int8_t z_b = ((aimath_q7_params_t *) b->tensor_params)->zero_point;
	int8_t z_result = ((aimath_q7_params_t *) result->tensor_params)->zero_point;

	// Output scaling factor M = (S_1 * S_2) / S_3
	uint16_t output_shift = a_shift + b_shift - result_shift;

	int8_t *a_data = (int8_t *) a->data;
	int8_t *b_data = (int8_t *) b->data;
	int32_t *c_data;
	if(c != 0) c_data = (int32_t *) c->data;
	int8_t *result_data = (int8_t *) result->data;


#ifdef AIDEBUG_SHAPE_CHECKS
	if(a->shape[1] != b->shape[0])
	{
		AILOG_E(aistring_error_q7_linear32_1);
		return;
	}
	if(a->shape[0] != result->shape[0] || b->shape[1] != result->shape[1])
	{
		AILOG_E(aistring_error_q7_linear32_2);
		return;
	}
#endif
#ifdef AIDEBUG_GENERAL_CHECKS
	if(c_shift != a_shift + b_shift)
	{
		AILOG_E(aistring_error_q7_linear32_3);
		return;
	}
#endif // AIDEBUG_GENERAL_CHECKS

	for(i = 0; i < a->shape[0]; i++)
	{
		for(j = 0; j < b->shape[1]; j++)
		{
			sum = 0;
			for(k = 0; k < a->shape[1]; k++)
			{
				// uint32 += uint8 * uint8
				sum += (int32_t) a_data[i*a->shape[1] + k] * (int32_t) b_data[k*b->shape[1] + j];
			}
			// Add the zero point correction terms if zero point != 0
			if(z_a != 0){
				// a_2 = sum(q_{2,jk}) from j=1 to N
				acc = 0;
				for(k = 0; k < a->shape[1]; k++){
					acc += (int32_t) b_data[k*b->shape[1] + j];
				}
				sum -= z_a * acc;
			}
			if(z_b != 0){
				// a_1 = sum(q_{1,ij}) from j=1 to N
				acc = 0;
				for(k = 0; k < a->shape[1]; k++){
					acc += (int32_t) a_data[i*a->shape[1] + k];
				}
				sum -= z_b * acc;
			}
			if(z_a != 0 && z_b != 0){
				// N * Z_1 * Z_2
				sum += a->shape[1] * z_a * z_b;
			}
			if(c != 0){
				// Bias add
				sum += c_data[j];
			}

			result_data[i*b->shape[1] + j] = (int8_t)((sum >> output_shift) + (int16_t) z_result);
		}
	}
	return;
}

void aimath_q7_default_linear32_bt(const aitensor_t *a, const aitensor_t *b, const aitensor_t *c, aitensor_t *result)
{
	uint16_t i, j, k;
	int32_t sum, acc; // 16-bit accumulator
	uint16_t a_shift = ((aimath_q7_params_t *) a->tensor_params)->shift;
	uint16_t b_shift = ((aimath_q7_params_t *) b->tensor_params)->shift;
	uint16_t c_shift = ((aimath_q31_params_t *) c->tensor_params)->shift;
	uint16_t result_shift = ((aimath_q7_params_t *) result->tensor_params)->shift;

	int8_t z_a = ((aimath_q7_params_t *) a->tensor_params)->zero_point;
	int8_t z_b = ((aimath_q7_params_t *) b->tensor_params)->zero_point;
	int8_t z_result = ((aimath_q7_params_t *) result->tensor_params)->zero_point;

	// Output scaling factor M = (S_1 * S_2) / S_3
	uint16_t output_shift = a_shift + b_shift - result_shift;

	int8_t *a_data = (int8_t *) a->data;
	int8_t *b_data = (int8_t *) b->data;
	int32_t *c_data;
	if(c != 0) c_data = (int32_t *) c->data;
	int8_t *result_data = (int8_t *) result->data;


#ifdef AIDEBUG_SHAPE_CHECKS
	if(a->shape[1] != b->shape[1])
	{
		AILOG_E(aistring_error_q7_linear32_1);
		return;
	}
	if(a->shape[0] != result->shape[0] || b->shape[0] != result->shape[1])
	{
		AILOG_E(aistring_error_q7_linear32_2);
		return;
	}
#endif
#ifdef AIDEBUG_GENERAL_CHECKS
	if(c_shift != a_shift + b_shift)
	{
		AILOG_E(aistring_error_q7_linear32_3);
		return;
	}
#endif // AIDEBUG_GENERAL_CHECKS

	for(i = 0; i < a->shape[0]; i++)
	{
		for(j = 0; j < b->shape[0]; j++)
		{
			sum = 0;
			for(k = 0; k < a->shape[1]; k++)
			{
				// uint32 += uint8 * uint8
				sum += (int32_t) a_data[i*a->shape[1] + k] * (int32_t) b_data[j*b->shape[1] + k];
			}
			// Add the zero point correction terms if zero point != 0
			if(z_a != 0){
				// a_2 = sum(q_{2,jk}) from j=1 to N
				acc = 0;
				for(k = 0; k < a->shape[1]; k++){
					acc += (int32_t) b_data[j*b->shape[1] + k];
				}
				sum -= z_a * acc;
			}
			if(z_b != 0){
				// a_1 = sum(q_{1,ij}) from j=1 to N
				acc = 0;
				for(k = 0; k < a->shape[1]; k++){
					acc += (int32_t) a_data[i*a->shape[1] + k];
				}
				sum -= z_b * acc;
			}
			if(z_a != 0 && z_b != 0){
				// N * Z_1 * Z_2
				sum += a->shape[1] * z_a * z_b;
			}
			if(c != 0){
				// Bias add
				sum += c_data[j];
			}

			result_data[i*b->shape[0] + j] = (int8_t)((sum >> output_shift) + (int16_t) z_result);
		}
	}
	return;
}

void aimath_q7_default_mat_mul(const aitensor_t *a, const aitensor_t *b, aitensor_t *result){
	aimath_q7_default_linear32(a, b, 0, result);
}

void aimath_q7_default_multiply(const aitensor_t *a, const aitensor_t *b, aitensor_t *result)
{
	uint32_t i;
	int32_t acc;

	int32_t z_a = (int32_t) ((aimath_q7_params_t *) a->tensor_params)->zero_point;
	int32_t z_b = (int32_t) ((aimath_q7_params_t *) b->tensor_params)->zero_point;
	int32_t z_result = (int32_t) ((aimath_q7_params_t *) result->tensor_params)->zero_point;

	uint16_t output_shift = ((aimath_q7_params_t *) a->tensor_params)->shift + ((aimath_q7_params_t *) b->tensor_params)->shift - ((aimath_q7_params_t *) result->tensor_params)->shift;

	// Check if zero point correction is needed
	if(z_a != 0 && z_b != 0){
        for(i = 0; i < aimath_tensor_elements(a); i++){
            // q_a * q_b
            acc = (int32_t) ((int8_t *) a->data)[i] * (int32_t) ((int8_t *) b->data)[i];

            // - q_b * z_a
            acc -= (int32_t) ((int8_t *) b->data)[i] * z_a;

            // - q_a * z_b
            acc -= (int32_t) ((int8_t *) a->data)[i] * z_b;

            // + z_a * z_b
            acc += z_a * z_b;

            ((int8_t *) result->data)[i] = (int8_t) ((acc >> output_shift) + z_result);
        }
	}
	else if(z_a != 0){
        for(i = 0; i < aimath_tensor_elements(a); i++){
            // q_a * q_b
            acc = (int32_t) ((int8_t *) a->data)[i] * (int32_t) ((int8_t *) b->data)[i];

            // - q_b * z_a
            acc -= (int32_t) ((int8_t *) b->data)[i] * z_a;

            ((int8_t *) result->data)[i] = (int8_t) ((acc >> output_shift) + z_result);
        }
    }
	else if(z_b != 0){
        for(i = 0; i < aimath_tensor_elements(a); i++){
            // q_a * q_b
            acc = (int32_t) ((int8_t *) a->data)[i] * (int32_t) ((int8_t *) b->data)[i];

            // - q_a * z_b
            acc -= (int32_t) ((int8_t *) a->data)[i] * z_b;

            ((int8_t *) result->data)[i] = (int8_t) ((acc >> output_shift) + z_result);
        }
	}
	else{
        for(i = 0; i < aimath_tensor_elements(a); i++){
            // q_a * q_b
            acc = (int32_t) ((int8_t *) a->data)[i] * (int32_t) ((int8_t *) b->data)[i];

            ((int8_t *) result->data)[i] = (int8_t) ((acc >> output_shift) + z_result);
        }
	}
	return;
}

// Without zero point correction -> TODO
void aimath_q7_default_scalar_mul(const void *scalar, const aitensor_t *a, aitensor_t *result)
{
	uint32_t i;
	int32_t acc;

	int32_t z_scalar = (int32_t) ((aiscalar_q7_t *) scalar)->zero_point;
	int32_t z_a = (int32_t) ((aimath_q7_params_t *) a->tensor_params)->zero_point;
	int32_t z_result = (int32_t) ((aimath_q7_params_t *) result->tensor_params)->zero_point;

	uint16_t output_shift = ((aiscalar_q7_t *) scalar)->shift + ((aimath_q7_params_t *) a->tensor_params)->shift - ((aimath_q7_params_t *) result->tensor_params)->shift;

	// Check if zero point correction is needed
	if(z_a != 0 && z_scalar != 0){
        for(i = 0; i < aimath_tensor_elements(a); i++){
            // q_scalar * q_a
            acc = (int32_t) ((aiscalar_q7_t *) scalar)->value * (int32_t) ((int8_t *) a->data)[i];

            // - q_a * z_scalar
            acc -= (int32_t) ((int8_t *) a->data)[i] * z_scalar;

            // - q_scalar * z_a
            acc -= (int32_t) ((aiscalar_q7_t *) scalar)->value * z_a;

            // + z_scalar * z_a
            acc += z_scalar * z_a;

            ((int8_t *) result->data)[i] = (int8_t) ((acc >> output_shift) + z_result);
        }
	}
	else if(z_a != 0){
        for(i = 0; i < aimath_tensor_elements(a); i++){
            // q_scalar * q_a
            acc = (int32_t) ((aiscalar_q7_t *) scalar)->value * (int32_t) ((int8_t *) a->data)[i];

            // - q_scalar * z_a
            acc -= (int32_t) ((aiscalar_q7_t *) scalar)->value * z_a;

            ((int8_t *) result->data)[i] = (int8_t) ((acc >> output_shift) + z_result);
        }
    }
	else if(z_scalar != 0){
        for(i = 0; i < aimath_tensor_elements(a); i++){
            // q_scalar * q_a
            acc = (int32_t) ((aiscalar_q7_t *) scalar)->value * (int32_t) ((int8_t *) a->data)[i];

            // - q_a * z_scalar
            acc -= (int32_t) ((int8_t *) a->data)[i] * z_scalar;

            ((int8_t *) result->data)[i] = (int8_t) ((acc >> output_shift) + z_result);
        }
	}
	else{
        for(i = 0; i < aimath_tensor_elements(a); i++){
            // q_scalar * q_a
            acc = (int32_t) ((aiscalar_q7_t *) scalar)->value * (int32_t) ((int8_t *) a->data)[i];

            ((int8_t *) result->data)[i] = (int8_t) ((acc >> output_shift) + z_result);
        }
	}
	return;
}

/**
* Add 2D tensors
*/
void aimath_q7_default_tensor_add_different_shift(const aitensor_t *a, const aitensor_t *b, aitensor_t *result)
{
	uint32_t i;

	int16_t z_a = (int16_t) ((aimath_q7_params_t *) a->tensor_params)->zero_point;
	int16_t z_b = (int16_t) ((aimath_q7_params_t *) b->tensor_params)->zero_point;
	int16_t z_result = (int16_t) ((aimath_q7_params_t *) result->tensor_params)->zero_point;

	int16_t z_correct;

	uint16_t s_a = ((aimath_q7_params_t *) a->tensor_params)->shift;
	uint16_t s_b = ((aimath_q7_params_t *) b->tensor_params)->shift;
	uint16_t s_result = ((aimath_q7_params_t *) result->tensor_params)->shift;

	// Shift the operands to the result scale/shift
	if(s_a < s_b){
		if(s_b < s_result){
            z_correct = z_b + (z_a << (s_b - s_a));
			for(i = 0; i < aimath_tensor_elements(a); i++)
			{
				((int8_t *) result->data)[i] = (int8_t) (((((int16_t) ((int8_t *) a->data)[i] << (s_b - s_a))
                                                    + (int16_t) ((int8_t *) b->data)[i] - z_correct) << (s_result - s_b))
                                                    + z_result);
			}
		} else {
            z_correct = z_b + (z_a << (s_b - s_a));
			for(i = 0; i < aimath_tensor_elements(a); i++)
			{
				((int8_t *) result->data)[i] = (int8_t) (((((int16_t) ((int8_t *) a->data)[i] << (s_b - s_a))
                                                    + (int16_t) ((int8_t *) b->data)[i] - z_correct) >> (s_b - s_result))
                                                    + z_result);
			}
		}
	}
	else{
		if(s_a < s_result){
            z_correct = (z_b << (s_a - s_b)) + z_a;
			for(i = 0; i < aimath_tensor_elements(a); i++)
			{
				((int8_t *) result->data)[i] = (int8_t) ((((int16_t) ((int8_t *) a->data)[i]
                                                    + ((int16_t) ((int8_t *) b->data)[i] << (s_a - s_b)) - z_correct) << (s_result - s_a))
                                                    + z_result);
			}
		} else {
            z_correct = (z_b << (s_a - s_b)) + z_a;
			for(i = 0; i < aimath_tensor_elements(a); i++)
			{
				((int8_t *) result->data)[i] = (int8_t) ((((int16_t) ((int8_t *) a->data)[i]
                                                    + ((int16_t) ((int8_t *) b->data)[i] << (s_a - s_b)) - z_correct) >> (s_a - s_result))
                                                    + z_result);
			}
		}
	}
	return;
}

void aimath_q7_default_tensor_add_same_shift(const aitensor_t *a, const aitensor_t *b, aitensor_t *result)
{
    uint32_t i;

	int16_t z_a = (int16_t) ((aimath_q7_params_t *) a->tensor_params)->zero_point;
	int16_t z_b = (int16_t) ((aimath_q7_params_t *) b->tensor_params)->zero_point;
	int16_t z_result = (int16_t) ((aimath_q7_params_t *) result->tensor_params)->zero_point;

	int16_t z_correct = - z_a - z_b + z_result;

    for(i = 0; i < aimath_tensor_elements(a); i++)
	{
		((int8_t *) result->data)[i] = (int8_t) ((int16_t) ((int8_t *) a->data)[i] + (int16_t) ((int8_t *) b->data)[i] + z_correct);
	}
	return;
}

void aimath_q7_default_tensor_sub_different_shift(const aitensor_t *a, const aitensor_t *b, aitensor_t *result)
{
	uint32_t i;

	int16_t z_a = (int16_t) ((aimath_q7_params_t *) a->tensor_params)->zero_point;
	int16_t z_b = (int16_t) ((aimath_q7_params_t *) b->tensor_params)->zero_point;
	int16_t z_result = (int16_t) ((aimath_q7_params_t *) result->tensor_params)->zero_point;

	int16_t z_correct;

	uint16_t s_a = ((aimath_q7_params_t *) a->tensor_params)->shift;
	uint16_t s_b = ((aimath_q7_params_t *) b->tensor_params)->shift;
	uint16_t s_result = ((aimath_q7_params_t *) result->tensor_params)->shift;

	// Shift the operands to the result scale/shift
	if(s_a < s_b){
		if(s_b < s_result){
            z_correct = z_b - (z_a << (s_b - s_a));
			for(i = 0; i < aimath_tensor_elements(a); i++)
			{
				((int8_t *) result->data)[i] = (int8_t) (((((int16_t) ((int8_t *) a->data)[i] << (s_b - s_a))
                                                    - (int16_t) ((int8_t *) b->data)[i] + z_correct) << (s_result - s_b))
                                                    + z_result);
			}
		} else {
            z_correct = z_b - (z_a << (s_b - s_a));
			for(i = 0; i < aimath_tensor_elements(a); i++)
			{
				((int8_t *) result->data)[i] = (int8_t) (((((int16_t) ((int8_t *) a->data)[i] << (s_b - s_a))
                                                    - (int16_t) ((int8_t *) b->data)[i] + z_correct) >> (s_b - s_result))
                                                    + z_result);
			}
		}
	}
	else{
		if(s_a < s_result){
            z_correct = (z_b << (s_a - s_b)) - z_a;
			for(i = 0; i < aimath_tensor_elements(a); i++)
			{
				((int8_t *) result->data)[i] = (int8_t) ((((int16_t) ((int8_t *) a->data)[i]
                                                    - ((int16_t) ((int8_t *) b->data)[i] << (s_a - s_b)) + z_correct) << (s_result - s_a))
                                                    + z_result);
			}
		} else {
            z_correct = (z_b << (s_a - s_b)) - z_a;
			for(i = 0; i < aimath_tensor_elements(a); i++)
			{
				((int8_t *) result->data)[i] = (int8_t) ((((int16_t) ((int8_t *) a->data)[i]
                                                    - ((int16_t) ((int8_t *) b->data)[i] << (s_a - s_b)) + z_correct) >> (s_a - s_result))
                                                    + z_result);
			}
		}
	}
	return;
}

void aimath_q7_default_tensor_sub_same_shift(const aitensor_t *a, const aitensor_t *b, aitensor_t *result)
{
    uint32_t i;

	int16_t z_a = (int16_t) ((aimath_q7_params_t *) a->tensor_params)->zero_point;
	int16_t z_b = (int16_t) ((aimath_q7_params_t *) b->tensor_params)->zero_point;
	int16_t z_result = (int16_t) ((aimath_q7_params_t *) result->tensor_params)->zero_point;

	int16_t z_correct = - z_a + z_b + z_result;

    for(i = 0; i < aimath_tensor_elements(a); i++)
	{
		((int8_t *) result->data)[i] = (int8_t) ((int16_t) ((int8_t *) a->data)[i] - (int16_t) ((int8_t *) b->data)[i] + z_correct);
	}
	return;
}

void aimath_q7_default_copy_tensor(const aitensor_t *from, aitensor_t *to)
{
	uint32_t i;
	for(i = 0; i < aimath_tensor_elements(from); i++)
	{
		((int8_t *) to->data)[i] = ((int8_t *) from->data)[i];
	}
	return;
}

void aimath_q7_default_transpose_vector(aitensor_t *vector)
{
	uint16_t temp;
	temp = vector->shape[0];
	vector->shape[0] = vector->shape[1];
	vector->shape[1] = temp;
	return;
}

void aimath_q7_default_transpose_matrix(aitensor_t *x)
{
    uint16_t i, j;
    int8_t temp_data[x->shape[0] * x->shape[1]], temp;
    aitensor_t temp_tensor = AITENSOR_2D_Q7(x->shape, x->tensor_params, temp_data);

    aimath_q7_default_copy_tensor(x, &temp_tensor);

    for(i = 0; i < x->shape[0]; i++){
        for(j = 0; j < x->shape[1]; j++){
            ((int8_t *) x->data)[j*x->shape[0] + i] = temp_data[i*x->shape[1] + j];
        }
    }

    temp = x->shape[0];
    x->shape[0] = x->shape[1];
    x->shape[1] = temp;
    return;
}

void aimath_q7_default_sigmoid(const aitensor_t *x, aitensor_t *result)
{
	uint32_t i;
	int16_t borders[3] = {16, 38, 80}; //  {1.0, 2.375, 5.0}
	uint16_t border_shift = 4;

	uint16_t x_shift = ((aimath_q7_params_t *) x->tensor_params)->shift;
	int8_t x_zero_point = ((aimath_q7_params_t *) x->tensor_params)->zero_point;

	int16_t res;
	int16_t x_abs;
	int16_t x_data_point;

	if(x_shift > border_shift){
		borders[0] <<= (x_shift - border_shift);
		borders[1] <<= (x_shift - border_shift);
		borders[2] <<= (x_shift - border_shift);
		border_shift = 2 * border_shift - x_shift;
	}

	for(i = 0; i < aimath_tensor_elements(x); i++)
	{
		x_data_point = (int16_t) ((int8_t *) x->data)[i] - (int16_t) x_zero_point;
		x_abs = x_data_point < 0 ? -x_data_point : x_data_point;

		if(x_shift < border_shift){
			x_abs <<= (border_shift - x_shift);
		}

		if(x_abs < borders[0]){
            // 0,25 � |X| + 0,5
			if(border_shift < 2){
				res = (x_abs >> (2 - border_shift)) + 0;
			}
			else{
				res = (x_abs << (border_shift - 2)) + 0;
			}
		}
		else if(x_abs < borders[1]){
		    // 0,0125 � |X| + 0,625
			if(border_shift < 3){
				res = (x_abs >> (3 - border_shift)) + 31;
			}
			else{
				res = (x_abs << (border_shift - 3)) + 31;
			}
		}
		else if(x_abs < borders[2]){
		    // 0,03125 � |X| + 0,84375
			if(border_shift < 5){
				res = (x_abs >> (5 - border_shift)) + 87;
			}
			else{
				res = (x_abs << (border_shift - 5)) + 87;
			}
		}
		else{
		    // 1
			res = 127;
		}

		if(x_data_point < 0){
            res = 0 - res;
		}

		((int8_t *) result->data)[i] = (int8_t) res;
	}

	((aimath_q7_params_t *) result->tensor_params)->shift = 8;
	((aimath_q7_params_t *) result->tensor_params)->zero_point = -128;

	return;
}

void aimath_q7_default_elu(const aitensor_t *x, const void *alpha, aitensor_t *result)
{
	uint32_t i;
	int16_t borders[4] = {-32, -64, -96, -127};
	int16_t b[4] = {12, 22, 28, 32};
	uint16_t border_shift = 5; //default value range (-4, 4)

	uint16_t x_shift = ((aimath_q7_params_t *) x->tensor_params)->shift;
	int16_t x_zero_point = (int16_t) ((aimath_q7_params_t *) x->tensor_params)->zero_point;
	uint16_t alpha_shift = ((aiscalar_q7_t *) alpha)->shift;
	int16_t alpha_zero_point = (int16_t) ((aiscalar_q7_t *) alpha)->zero_point;
    int16_t alpha_data = (int16_t) ((aiscalar_q7_t *) alpha)->value;

	int16_t res;
	int16_t x_data_point;

	if(x_shift > border_shift){
		borders[0] <<= (x_shift - border_shift);
		borders[1] <<= (x_shift - border_shift);
		borders[2] <<= (x_shift - border_shift);
		borders[3] <<= (x_shift - border_shift);
		b[0] <<= (x_shift - border_shift);
		b[1] <<= (x_shift - border_shift);
		b[2] <<= (x_shift - border_shift);
		b[3] <<= (x_shift - border_shift);
		//border_shift = 2 * border_shift - x_shift;
	}
	else{
        borders[0] >>= (border_shift - x_shift);
		borders[1] >>= (border_shift - x_shift);
		borders[2] >>= (border_shift - x_shift);
		borders[3] >>= (border_shift - x_shift);
        b[0] >>= (border_shift - x_shift);
		b[1] >>= (border_shift - x_shift);
		b[2] >>= (border_shift - x_shift);
        b[3] >>= (border_shift - x_shift);
        //border_shift = 2* x_shift - border_shift;
	}

	for(i = 0; i < aimath_tensor_elements(x); i++)
	{
		x_data_point = (int16_t) ((int8_t *) x->data)[i] - x_zero_point;

        if(x_data_point > 0){
            res = x_data_point;
        }
        else{
            if(x_data_point > borders[0]){
                res = ((20 * x_data_point) >> 5);
            }
            else if(x_data_point > borders[1]){
                res = ((8 * x_data_point) >> 5) - b[0];
            }
            else if(x_data_point > borders[2]){
                 res = ((3 * x_data_point) >> 5) - b[1];
            }
            else if(x_data_point > borders[3]){
                res = ((x_data_point) >> 5) - b[2];
            }
            else{
                res = -b[3];
            }

            res = res * (alpha_data - alpha_zero_point);
            res >>= alpha_shift;
        }
		((int8_t *) result->data)[i] = (int8_t) (res + x_zero_point);
	}

	((aimath_q7_params_t *) result->tensor_params)->shift = x_shift;
	((aimath_q7_params_t *) result->tensor_params)->zero_point = x_zero_point;

	return;
}

void aimath_q7_default_relu(const aitensor_t *x, aitensor_t *result) {
    uint32_t i;

    uint16_t shift = ((aimath_q7_params_t *) x->tensor_params)->shift;
    int8_t zero_point = ((aimath_q7_params_t *) x->tensor_params)->zero_point;

    for(i = 0; i < aimath_tensor_elements(x); i++)
	{
        ((int8_t *) result->data)[i] = ((int8_t *) x->data)[i] > zero_point ? ((int8_t *) x->data)[i] : zero_point;
	}

	((aimath_q7_params_t *) result->tensor_params)->shift = shift;
	((aimath_q7_params_t *) result->tensor_params)->zero_point = zero_point;

	return;
}

void aimath_q7_default_d_relu(const aitensor_t *x, aitensor_t *result)
{
	uint32_t i;
    int8_t zero_point = ((aimath_q7_params_t *) x->tensor_params)->zero_point;

	for(i = 0; i < aimath_tensor_elements(x); i++)
	{
		((int8_t *) result->data)[i] = ((int8_t *) x->data)[i] >= zero_point ? 1 : 0;
	}

	((aimath_q7_params_t *) result->tensor_params)->shift = 0;
	((aimath_q7_params_t *) result->tensor_params)->zero_point = 0;

	return;
}

void aimath_q7_default_leaky_relu(const aitensor_t *x, const void *alpha, aitensor_t *result)
{
    uint32_t i;
    int32_t acc;

    int8_t z_alpha = ((aiscalar_q7_t *) alpha)->zero_point;
	int8_t z_x = ((aimath_q7_params_t *) x->tensor_params)->zero_point;
	int8_t z_result = z_x;

	uint16_t s_x = ((aimath_q7_params_t *) x->tensor_params)->shift;
	uint16_t s_alpha = ((aiscalar_q7_t *) alpha)->shift;

	uint16_t output_shift = s_alpha; // Output has same shift as input

	for(i = 0; i < aimath_tensor_elements(x); i++)
	{
        if (((int8_t *) x->data)[i] > z_x) {
            ((int8_t *) result->data)[i] = ((int8_t *) x->data)[i];
        } else {
            // Check if zero point correction is needed
            if(z_x != 0 && z_alpha != 0){
                // q_alpha * q_x
                acc = (int32_t) ((aiscalar_q7_t *) alpha)->value * (int32_t) ((int8_t *) x->data)[i];

                // - q_x * z_alpha
                acc -= (int32_t) ((int8_t *) x->data)[i] * z_alpha;

                // - q_alpha * z_x
                acc -= (int32_t) ((aiscalar_q7_t *) alpha)->value * z_x;

                // + z_alpha * z_x
                acc += z_alpha * z_x;

                ((int8_t *) result->data)[i] = (int8_t) ((acc >> output_shift) + z_result);
            }
            else if(z_x != 0){
                // q_alpha * q_x
                acc = (int32_t) ((aiscalar_q7_t *) alpha)->value * (int32_t) ((int8_t *) x->data)[i];

                // - q_alpha * z_x
                acc -= (int32_t) ((aiscalar_q7_t *) alpha)->value * z_x;

                ((int8_t *) result->data)[i] = (int8_t) ((acc >> output_shift) + z_result);
            }
            else if(z_alpha != 0){
                // q_alpha * q_x
                acc = (int32_t) ((aiscalar_q7_t *) alpha)->value * (int32_t) ((int8_t *) x->data)[i];

                // - q_x * z_alpha
                acc -= (int32_t) ((int8_t *) x->data)[i] * z_alpha;

                ((int8_t *) result->data)[i] = (int8_t) ((acc >> output_shift) + z_result);
            }
            else{
                // q_alpha * q_x
                acc = (int32_t) ((aiscalar_q7_t *) alpha)->value * (int32_t) ((int8_t *) x->data)[i];

                ((int8_t *) result->data)[i] = (int8_t) ((acc >> output_shift) + z_result);
            }
        }
	}

	((aimath_q7_params_t *) result->tensor_params)->zero_point = z_x;
	((aimath_q7_params_t *) result->tensor_params)->shift = s_x;
	return;
}

void aimath_q7_default_tanh(const aitensor_t *x, aitensor_t *result)
{
	uint32_t i;
	int16_t borders[3] = {16, 38, 80}; //  {1.0, 2.375, 5.0}
	uint16_t border_shift = 4;

	uint16_t x_shift = ((aimath_q7_params_t *) x->tensor_params)->shift - 1; // Reduce shift (multiply by 2)
	int8_t x_zero_point = ((aimath_q7_params_t *) x->tensor_params)->zero_point;

	int16_t res;
	int16_t x_abs;
	int16_t x_data_point;

	if(x_shift > border_shift){
		borders[0] <<= (x_shift - border_shift);
		borders[1] <<= (x_shift - border_shift);
		borders[2] <<= (x_shift - border_shift);
		border_shift = 2 * border_shift - x_shift;
	}

	for(i = 0; i < aimath_tensor_elements(x); i++)
	{
		x_data_point = (int16_t) ((int8_t *) x->data)[i] - (int16_t) x_zero_point;
		x_abs = x_data_point < 0 ? -x_data_point : x_data_point;

		if(x_shift < border_shift){
			x_abs <<= (border_shift - x_shift);
		}

		if(x_abs < borders[0]){
            // 0,25 � |X| + 0,5
			if(border_shift < 2){
				res = (x_abs >> (2 - border_shift)) + 0;
			}
			else{
				res = (x_abs << (border_shift - 2)) + 0;
			}
		}
		else if(x_abs < borders[1]){
            // 0,0125 � |X| + 0,625
			if(border_shift < 3){
				res = (x_abs >> (3 - border_shift)) + 31;
			}
			else{
				res = (x_abs << (border_shift - 3)) + 31;
			}
		}
		else if(x_abs < borders[2]){
            // 0,03125 � |X| + 0,84375
			if(border_shift < 5){
				res = (x_abs >> (5 - border_shift)) + 87;
			}
			else{
				res = (x_abs << (border_shift - 5)) + 87;
			}
		}
		else{
            // 1
			res = 127;
		}

		if(x_data_point < 0){
            res = 0 - res;
		}

		((int8_t *) result->data)[i] = (int8_t) res;
	}

	((aimath_q7_params_t *) result->tensor_params)->shift = 7; // Multiply sigmoid result by 2
	((aimath_q7_params_t *) result->tensor_params)->zero_point = 0;

	return;
}

void aimath_q7_default_softsign(const aitensor_t *x, aitensor_t *result)
{
    uint32_t i;
    int32_t acc;
    int32_t acc_denom;

    uint16_t shift = ((aimath_q7_params_t *) x->tensor_params)->shift;
    int8_t zero_point = ((aimath_q7_params_t *) x->tensor_params)->zero_point;

    for(i = 0; i < aimath_tensor_elements(x); i++)
	{
        if (((int8_t *) x->data)[i] > zero_point) {
            acc_denom = ((int8_t *) x->data)[i] - zero_point + (1 << shift);
            acc = ((int8_t *) x->data)[i] - zero_point;
            acc = (acc << 7) / acc_denom;
            ((int8_t *) result->data)[i] = (int8_t) (acc);
        }
        else if (((int8_t *) x->data)[i] < zero_point){
            acc_denom = zero_point - ((int8_t *) x->data)[i] + (1 << shift);
            acc = ((int8_t *) x->data)[i] - zero_point;
            acc = (acc << 7) / acc_denom;
            ((int8_t *) result->data)[i] = (int8_t) (acc);
        }
        else {
            ((int8_t *) result->data)[i] = (int8_t) (zero_point);
        }

	}

	((aimath_q7_params_t *) result->tensor_params)->shift = 7;
	((aimath_q7_params_t *) result->tensor_params)->zero_point = 0;

	return;
}

void aimath_q7_default_softmax(const aitensor_t *x, aitensor_t *result) {
    int32_t i, j;
	int16_t borders[4] = {-16, -32, -48, -80}; //  {-1.0, -2.0, -3.0, -5.0}
	uint16_t border_shift = 4;
	int8_t max;

	uint16_t x_shift = ((aimath_q7_params_t *) x->tensor_params)->shift;
	int8_t x_zero_point = ((aimath_q7_params_t *) x->tensor_params)->zero_point;

	int32_t res;
	int32_t acc = 0;
	int8_t e_x[aimath_tensor_elements(x)];
	int16_t x_data_point;

	// Multiplier for array index calculation
 	uint16_t multiplier = 1;
 	for(i = x->dim - 1; i >= 1; i--){
        multiplier *= x->shape[i];
 	}

 	if(x_shift > border_shift){
		borders[0] <<= (x_shift - border_shift);
		borders[1] <<= (x_shift - border_shift);
		borders[2] <<= (x_shift - border_shift);
		borders[3] <<= (x_shift - border_shift);
	}
	else{
        borders[0] >>= (border_shift - x_shift);
		borders[1] >>= (border_shift - x_shift);
		borders[2] >>= (border_shift - x_shift);
		borders[3] >>= (border_shift - x_shift);
	}

    for(i = 0; i < x->shape[0]; i++){
        // calc max value for numeric stability
        max = ((int8_t *) x->data)[i * multiplier];
        for (j = 1; j < multiplier; j++) {
            if (((int8_t *) x->data)[i * multiplier + j] > max) {
                max = ((int8_t *) x->data)[i * multiplier + j];
            }
        }
        x_zero_point = max;

        // calc exp functions
        acc = 0;
        for(j = 0; j < multiplier; j++)
        {
            x_data_point = (int16_t) ((int8_t *) x->data)[i * multiplier + j] - (int16_t) x_zero_point;

            if (x_data_point > borders[0]) {
                res = ((161 * x_data_point) >> x_shift) + 127;
            }
            else if (x_data_point > borders[1]) {
                res = ((59 * x_data_point) >> x_shift) + 26;
            }
            else if (x_data_point > borders[2]) {
                res = ((23 * x_data_point) >> x_shift) - 46;
            }
            else if (x_data_point > borders[3]) {
                res = ((6 * x_data_point) >> x_shift) - 96;
            }
            else {
                res = -128;
            }

            acc += (res + 128);
            e_x[i * multiplier + j] = (int8_t) res;
        }

        //calc softmax
        for(j = 0; j < multiplier; j++)
        {
            res = ((((int32_t) e_x[i * multiplier + j] + 128) << 8) / (acc + 1)) - 128;
            ((int8_t *) result->data)[i * multiplier + j] = (int8_t) res;
        }
 	}

 	((aimath_q7_params_t *) result->tensor_params)->shift = 8;
	((aimath_q7_params_t *) result->tensor_params)->zero_point = -128;

	return;
}

// Set values to zero_point
void aimath_q7_default_zero_tensor(aitensor_t *tensor)
{
	uint32_t i;
	int8_t zero_point = ((aimath_q7_params_t *) tensor->tensor_params)->zero_point;

	for(i = 0; i < aimath_tensor_elements(tensor); i++)
	{
		((int8_t *) tensor->data)[i] = zero_point;
	}
	return;
}

// Init shift, zero_point and values to zero
void aimath_q7_default_init_zeros(aitensor_t *tensor)
{
	uint32_t i;
	((aimath_q7_params_t *) tensor->tensor_params)->shift = 0;
	((aimath_q7_params_t *) tensor->tensor_params)->zero_point = 0;

	for(i = 0; i < aimath_tensor_elements(tensor); i++)
	{
		((int8_t *) tensor->data)[i] = 0;
	}
	return;
}
