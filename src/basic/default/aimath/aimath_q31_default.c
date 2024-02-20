/**
 * \file basic/default/aimath/aimath_q31_default.c
 * \version 2.2.0
 * \date 03.02.2021
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

#include "basic/default/aimath/aimath_q31_default.h"


AISTRING_STORAGE_WRAPPER(aistring_error_q31_linear32_1, "[aimath_q31_default_linear32] MatMul input shapes doesn't match.\n");
AISTRING_STORAGE_WRAPPER(aistring_error_q31_linear32_2, "[aimath_q31_default_linear32] MatMul output shape doesn't match.\n");

void aimath_q31_default_linear32(const aitensor_t *a, const aitensor_t *b, const aitensor_t *c, aitensor_t *result)
{
	uint16_t i, j, k;
	int64_t sum, acc;
	uint16_t a_shift = ((aimath_q31_params_t *) a->tensor_params)->shift;
	uint16_t b_shift = ((aimath_q31_params_t *) b->tensor_params)->shift;
	uint16_t result_shift = ((aimath_q31_params_t *) result->tensor_params)->shift;

	int64_t z_a = (int64_t) ((aimath_q31_params_t *) a->tensor_params)->zero_point;
	int64_t z_b = (int64_t) ((aimath_q31_params_t *) b->tensor_params)->zero_point;
	int64_t z_result = (int64_t) ((aimath_q31_params_t *) result->tensor_params)->zero_point;

	// Output scaling factor M = (S_1 * S_2) / S_3
	uint16_t output_shift, final_shift;
	if(result_shift > a_shift + b_shift){
        output_shift = 0;
        final_shift = result_shift - a_shift - b_shift;
	} else {
        output_shift = a_shift + b_shift - result_shift;
        final_shift = 0;
	}

	int32_t *a_data = (int32_t *) a->data;
	int32_t *b_data = (int32_t *) b->data;
	int32_t *c_data;
	if(c != 0) c_data = (int32_t *) c->data;
	else c_data = 0;
	int32_t *result_data = (int32_t *) result->data;


#ifdef AIDEBUG_SHAPE_CHECKS
	if(a->shape[1] != b->shape[0])
	{
		AILOG_E(aistring_error_q31_linear32_1);
		return;
	}
	if(a->shape[0] != result->shape[0] || b->shape[1] != result->shape[1])
	{
		AILOG_E(aistring_error_q31_linear32_2);
		return;
	}
#endif

	for(i = 0; i < a->shape[0]; i++)
	{
		for(j = 0; j < b->shape[1]; j++)
		{
			sum = 0;
			for(k = 0; k < a->shape[1]; k++)
			{
				// uint32 += uint8 * uint8
				sum += (((int64_t) a_data[i*a->shape[1] + k] * (int64_t) b_data[k*b->shape[1] + j]) >> output_shift) << final_shift;
			}
			// Add the zero point correction terms if zero point != 0
			if(z_a != 0){
				// a_2 = sum(q_{2,kj}) from k=1 to N
				acc = 0;
				for(k = 0; k < a->shape[1]; k++){
					acc += (int64_t) b_data[k*b->shape[1] + j];
				}
				sum -= ((z_a * acc) >> output_shift) << final_shift;
			}
			if(z_b != 0){
				// a_1 = sum(q_{1,ik}) from k=1 to N
				acc = 0;
				for(k = 0; k < a->shape[1]; k++){
					acc += (int64_t) a_data[i*a->shape[1] + k];
				}
				sum -= ((z_b * acc) >> output_shift) << final_shift;
			}
			if(z_a != 0 && z_b != 0){
				// N * Z_1 * Z_2
				sum += ((a->shape[1] * z_a * z_b) >> output_shift) << final_shift;
			}
			if(c != 0){
				// Bias add
				sum += (c_data[j] >> output_shift) << final_shift;
			}

			result_data[i*b->shape[1] + j] = (int32_t)((sum) + (int64_t) z_result);
		}
	}

	return;
}

void aimath_q31_default_mat_mul(const aitensor_t *a, const aitensor_t *b, aitensor_t *result){
	aimath_q31_default_linear32(a, b, 0, result);
}

// ToDo: Check zeropoint correction
void aimath_q31_default_multiply(const aitensor_t *a, const aitensor_t *b, aitensor_t *result)
{
	uint32_t i;
	int64_t acc;

	int64_t z_a = (int64_t) ((aimath_q31_params_t *) a->tensor_params)->zero_point;
	int64_t z_b = (int64_t) ((aimath_q31_params_t *) b->tensor_params)->zero_point;
	int64_t z_result = (int64_t) ((aimath_q31_params_t *) result->tensor_params)->zero_point;

	uint16_t output_shift = ((aimath_q31_params_t *) a->tensor_params)->shift + ((aimath_q31_params_t *) b->tensor_params)->shift - ((aimath_q31_params_t *) result->tensor_params)->shift;

	// Check if zero point correction is needed
	if(z_a != 0 && z_b != 0){
        for(i = 0; i < aimath_tensor_elements(a); i++){
            // q_a * q_b
            acc = (int64_t) ((int32_t *) a->data)[i] * (int64_t) ((int32_t *) b->data)[i];

            // - q_b * z_a
            acc -= (int64_t) ((int32_t *) b->data)[i] * z_a;

            // - q_a * z_b
            acc -= (int64_t) ((int32_t *) a->data)[i] * z_b;

            // + z_a * z_b
            acc += z_a * z_b;

            ((int32_t *) result->data)[i] = (int32_t) ((acc >> output_shift) + z_result);
        }
	}
	else if(z_a != 0){
        for(i = 0; i < aimath_tensor_elements(a); i++){
            // q_a * q_b
            acc = (int64_t) ((int32_t *) a->data)[i] * (int64_t) ((int32_t *) b->data)[i];

            // - q_b * z_a
            acc -= (int64_t) ((int32_t *) b->data)[i] * z_a;

            ((int32_t *) result->data)[i] = (int32_t) ((acc >> output_shift) + z_result);
        }
    }
	else if(z_b != 0){
        for(i = 0; i < aimath_tensor_elements(a); i++){
            // q_a * q_b
            acc = (int64_t) ((int32_t *) a->data)[i] * (int64_t) ((int32_t *) b->data)[i];

            // - q_a * z_b
            acc -= (int64_t) ((int32_t *) a->data)[i] * z_b;

            ((int32_t *) result->data)[i] = (int32_t) ((acc >> output_shift) + z_result);
        }
	}
	else{
        for(i = 0; i < aimath_tensor_elements(a); i++){
            // q_a * q_b
            acc = (int64_t) ((int32_t *) a->data)[i] * (int64_t) ((int32_t *) b->data)[i];

            ((int32_t *) result->data)[i] = (int32_t) ((acc >> output_shift) + z_result);
        }
	}
	return;
}

// ToDo: Check zeropoint correction
void aimath_q31_default_scalar_mul(const void *scalar, const aitensor_t *a, aitensor_t *result)
{
	uint32_t i;
	int64_t acc;

	int64_t z_scalar = (int64_t) ((aiscalar_q31_t *) scalar)->zero_point;
	int64_t z_a = (int64_t) ((aimath_q31_params_t *) a->tensor_params)->zero_point;
	int64_t z_result = (int64_t) ((aimath_q31_params_t *) result->tensor_params)->zero_point;

	uint16_t output_shift = ((aiscalar_q31_t *) scalar)->shift + ((aimath_q31_params_t *) a->tensor_params)->shift - ((aimath_q31_params_t *) result->tensor_params)->shift;

	// Check if zero point correction is needed
	if(z_a != 0 && z_scalar != 0){
        for(i = 0; i < aimath_tensor_elements(a); i++){
            // q_scalar * q_a
            acc = (int64_t) ((aiscalar_q31_t *) scalar)->value * (int64_t) ((int32_t *) a->data)[i];

            // - q_a * z_scalar
            acc -= (int64_t) ((int32_t *) a->data)[i] * z_scalar;

            // - q_scalar * z_a
            acc -= (int64_t) ((aiscalar_q31_t *) scalar)->value * z_a;

            // + z_scalar * z_a
            acc += z_scalar * z_a;

            ((int32_t *) result->data)[i] = (int32_t) ((acc >> output_shift) + z_result);
        }
	}
	else if(z_a != 0){
        for(i = 0; i < aimath_tensor_elements(a); i++){
            // q_scalar * q_a
            acc = (int64_t) ((aiscalar_q31_t *) scalar)->value * (int64_t) ((int32_t *) a->data)[i];

            // - q_scalar * z_a
            acc -= (int64_t) ((aiscalar_q31_t *) scalar)->value * z_a;

            ((int32_t *) result->data)[i] = (int32_t) ((acc >> output_shift) + z_result);
        }
    }
	else if(z_scalar != 0){
        for(i = 0; i < aimath_tensor_elements(a); i++){
            // q_scalar * q_a
            acc = (int64_t) ((aiscalar_q31_t *) scalar)->value * (int64_t) ((int32_t *) a->data)[i];

            // - q_a * z_scalar
            acc -= (int64_t) ((int32_t *) a->data)[i] * z_scalar;

            ((int32_t *) result->data)[i] = (int32_t) ((acc >> output_shift) + z_result);
        }
	}
	else{
        for(i = 0; i < aimath_tensor_elements(a); i++){
            // q_scalar * q_a
            acc = (int64_t) ((aiscalar_q31_t *) scalar)->value * (int64_t) ((int32_t *) a->data)[i];

            ((int32_t *) result->data)[i] = (int32_t) ((acc >> output_shift) + z_result);
        }
	}
	return;
}

/**
* Add 2D tensors
*/
void aimath_q31_default_tensor_add_different_shift(const aitensor_t *a, const aitensor_t *b, aitensor_t *result)
{
	uint32_t i;

	int64_t z_a = (int64_t) ((aimath_q31_params_t *) a->tensor_params)->zero_point;
	int64_t z_b = (int64_t) ((aimath_q31_params_t *) b->tensor_params)->zero_point;
	int64_t z_result = (int64_t) ((aimath_q31_params_t *) result->tensor_params)->zero_point;

	int64_t z_correct;

	uint16_t s_a = ((aimath_q31_params_t *) a->tensor_params)->shift;
	uint16_t s_b = ((aimath_q31_params_t *) b->tensor_params)->shift;
	uint16_t s_result = ((aimath_q31_params_t *) result->tensor_params)->shift;

	// Shift the operands to the result scale/shift
	if(s_a < s_b){
		if(s_b < s_result){
            z_correct = z_b + (z_a << (s_b - s_a));
			for(i = 0; i < aimath_tensor_elements(a); i++)
			{
				((int32_t *) result->data)[i] = (int32_t) (((((int64_t) ((int32_t *) a->data)[i] << (s_b - s_a))
                                                    + (int64_t) ((int32_t *) b->data)[i] - z_correct) << (s_result - s_b))
                                                    + z_result);
			}
		} else {
            z_correct = z_b + (z_a << (s_b - s_a));
			for(i = 0; i < aimath_tensor_elements(a); i++)
			{
				((int32_t *) result->data)[i] = (int32_t) (((((int64_t) ((int32_t *) a->data)[i] << (s_b - s_a))
                                                    + (int64_t) ((int32_t *) b->data)[i] - z_correct) >> (s_b - s_result))
                                                    + z_result);
			}
		}
	}
	else{
		if(s_a < s_result){
            z_correct = (z_b << (s_a - s_b)) + z_a;
			for(i = 0; i < aimath_tensor_elements(a); i++)
			{
				((int32_t *) result->data)[i] = (int32_t) ((((int64_t) ((int32_t *) a->data)[i]
                                                    + ((int64_t) ((int32_t *) b->data)[i] << (s_a - s_b)) - z_correct) << (s_result - s_a))
                                                    + z_result);
			}
		} else {
            z_correct = (z_b << (s_a - s_b)) + z_a;
			for(i = 0; i < aimath_tensor_elements(a); i++)
			{
				((int32_t *) result->data)[i] = (int32_t) ((((int64_t) ((int32_t *) a->data)[i]
                                                    + ((int64_t) ((int32_t *) b->data)[i] << (s_a - s_b)) - z_correct) >> (s_a - s_result))
                                                    + z_result);
			}
		}
	}
	return;
}

void aimath_q31_default_tensor_add_same_shift(const aitensor_t *a, const aitensor_t *b, aitensor_t *result)
{
    uint32_t i;

	int64_t z_a = (int64_t) ((aimath_q31_params_t *) a->tensor_params)->zero_point;
	int64_t z_b = (int64_t) ((aimath_q31_params_t *) b->tensor_params)->zero_point;
	int64_t z_result = (int64_t) ((aimath_q31_params_t *) result->tensor_params)->zero_point;

	int64_t z_correct = - z_a - z_b + z_result;

    for(i = 0; i < aimath_tensor_elements(a); i++)
	{
		((int32_t *) result->data)[i] = (int32_t) ((int64_t) ((int32_t *) a->data)[i] + (int64_t) ((int32_t *) b->data)[i] + z_correct);
	}
	return;
}

void aimath_q31_default_tensor_sub_different_shift(const aitensor_t *a, const aitensor_t *b, aitensor_t *result)
{
	uint32_t i;

	int64_t z_a = (int64_t) ((aimath_q31_params_t *) a->tensor_params)->zero_point;
	int64_t z_b = (int64_t) ((aimath_q31_params_t *) b->tensor_params)->zero_point;
	int64_t z_result = (int64_t) ((aimath_q31_params_t *) result->tensor_params)->zero_point;

	int64_t z_correct;

	uint16_t s_a = ((aimath_q31_params_t *) a->tensor_params)->shift;
	uint16_t s_b = ((aimath_q31_params_t *) b->tensor_params)->shift;
	uint16_t s_result = ((aimath_q31_params_t *) result->tensor_params)->shift;

	// Shift the operands to the result scale/shift
	if(s_a < s_b){
		if(s_b < s_result){
            z_correct = z_b - (z_a << (s_b - s_a));
			for(i = 0; i < aimath_tensor_elements(a); i++)
			{
				((int32_t *) result->data)[i] = (int32_t) (((((int64_t) ((int32_t *) a->data)[i] << (s_b - s_a))
                                                    - (int64_t) ((int32_t *) b->data)[i] + z_correct) << (s_result - s_b))
                                                    + z_result);
			}
		} else {
            z_correct = z_b - (z_a << (s_b - s_a));
			for(i = 0; i < aimath_tensor_elements(a); i++)
			{
				((int32_t *) result->data)[i] = (int32_t) (((((int64_t) ((int32_t *) a->data)[i] << (s_b - s_a))
                                                    - (int64_t) ((int32_t *) b->data)[i] + z_correct) >> (s_b - s_result))
                                                    + z_result);
			}
		}
	}
	else{
		if(s_a < s_result){
            z_correct = (z_b << (s_a - s_b)) - z_a;
			for(i = 0; i < aimath_tensor_elements(a); i++)
			{
				((int32_t *) result->data)[i] = (int32_t) ((((int64_t) ((int32_t *) a->data)[i]
                                                    - ((int64_t) ((int32_t *) b->data)[i] << (s_a - s_b)) + z_correct) << (s_result - s_a))
                                                    + z_result);
			}
		} else {
            z_correct = (z_b << (s_a - s_b)) - z_a;
			for(i = 0; i < aimath_tensor_elements(a); i++)
			{
				((int32_t *) result->data)[i] = (int32_t) ((((int64_t) ((int32_t *) a->data)[i]
                                                    - ((int64_t) ((int32_t *) b->data)[i] << (s_a - s_b)) + z_correct) >> (s_a - s_result))
                                                    + z_result);
			}
		}
	}
	return;
}

void aimath_q31_default_tensor_sub_same_shift(const aitensor_t *a, const aitensor_t *b, aitensor_t *result)
{
    uint32_t i;

	int64_t z_a = (int64_t) ((aimath_q31_params_t *) a->tensor_params)->zero_point;
	int64_t z_b = (int64_t) ((aimath_q31_params_t *) b->tensor_params)->zero_point;
	int64_t z_result = (int64_t) ((aimath_q31_params_t *) result->tensor_params)->zero_point;

	int64_t z_correct = - z_a + z_b + z_result;

    for(i = 0; i < aimath_tensor_elements(a); i++)
	{
		((int32_t *) result->data)[i] = (int32_t) ((int64_t) ((int32_t *) a->data)[i] - (int64_t) ((int32_t *) b->data)[i] + z_correct);
	}
	return;
}

void aimath_q31_default_copy_tensor(const aitensor_t *from, aitensor_t *to)
{
	uint32_t i;
	for(i = 0; i < aimath_tensor_elements(from); i++)
	{
		((int32_t *) to->data)[i] = ((int32_t *) from->data)[i];
	}
	return;
}

void aimath_q31_default_transpose_vector(aitensor_t *vector)
{
	uint16_t temp;
	temp = vector->shape[0];
	vector->shape[0] = vector->shape[1];
	vector->shape[1] = temp;
	return;
}

// ToDo: Check zero point correction
void aimath_q31_default_norm_squared(const aitensor_t *x, void *result)
{
	uint32_t i;
	int64_t sum, acc;

	int64_t z_x = (int64_t) ((aimath_q31_params_t *) x->tensor_params)->zero_point;
	int64_t z_result = (int64_t) ((aiscalar_q31_t *) result)->zero_point;

	uint16_t output_shift = 2 * ((aimath_q31_params_t *) x->tensor_params)->shift - ((aiscalar_q31_t *) result)->shift;

	sum = 0;
	for(i = 0; i < aimath_tensor_elements(x); i++)
	{
		sum += (int64_t) ((int32_t *) x->data)[i] * (int64_t) ((int32_t *) x->data)[i];
	}

	// Zero point correction
	if(z_x != 0){
        acc = 0;
        for(i = 0; i < aimath_tensor_elements(x); i++){
            acc += (int64_t) ((int32_t *) x->data)[i];
        }
        sum -= 2 * z_x * acc;
	}

	((aiscalar_q31_t *) result)->value = (int32_t) ((sum >> output_shift) + z_result);
	return;
}

AISTRING_STORAGE_WRAPPER(aistring_error_q31_tensor_sqrt_1, "[aimath_q31_default_tensor_sqrt] Sqrt of a negative integer is not supported\n");

void aimath_q31_default_tensor_sqrt(const aitensor_t *x, aitensor_t *result)
{
    uint32_t i;

	int64_t z_x = (int64_t) ((aimath_q31_params_t *) x->tensor_params)->zero_point;
	int64_t z_result = (int64_t) ((aimath_q31_params_t *) result->tensor_params)->zero_point;

	uint16_t s_x = ((aimath_q31_params_t *) x->tensor_params)->shift;
	uint16_t s_result = ((aimath_q31_params_t *) result->tensor_params)->shift;

    for(i = 0; i < aimath_tensor_elements(x); i++)
	{
	    if(((int32_t *) x->data)[i] < z_x){
            #ifdef AIDEBUG_PRINT_ERROR_MESSAGES
                AILOG_E(aistring_error_q31_tensor_sqrt_1);
            #endif // AIDEBUG_PRINT_ERROR_MESSAGES
            return;
	    }
	    // q_res = sqrt(s_x) / s_res * sqrt(q_x - z_x) + z_res
		((int32_t *) result->data)[i] = (int32_t) ((aimath_q31_default_sqrt((int64_t) ((int32_t *) x->data)[i] - z_x) >> ((s_x >> 2) - s_result)) + z_result);
	}
	return;
}

void aimath_q31_default_sigmoid(const aitensor_t *x, aitensor_t *result)
{
	uint32_t i;
	//                    1.0      , 2.375    , 5.0
	int64_t borders[3] = {268435456, 637534208, 1342177280};
	uint16_t border_shift = 4 + 24;

	uint16_t x_shift = ((aimath_q31_params_t *) x->tensor_params)->shift;
	int32_t x_zero_point = ((aimath_q31_params_t *) x->tensor_params)->zero_point;

	int64_t res;
	int64_t x_abs;
	int64_t x_data_point;

	if(x_shift > border_shift){
		borders[0] <<= (x_shift - border_shift);
		borders[1] <<= (x_shift - border_shift);
		borders[2] <<= (x_shift - border_shift);
		border_shift = 2 * border_shift - x_shift;
	}

	for(i = 0; i < aimath_tensor_elements(x); i++)
	{
		x_data_point = (int64_t) ((int32_t *) x->data)[i] - (int64_t) x_zero_point;
		x_abs = x_data_point < 0 ? -x_data_point : x_data_point;

		if(x_shift < border_shift){
			x_abs <<= (border_shift - x_shift);
		}

		if(x_abs < borders[0]){
            // 0,25 � |X| + 0,5
			if(border_shift < 2 + 24){
				res = (x_abs >> (2 + 24 - border_shift)) + 0;
			}
			else{
				res = (x_abs << (border_shift - 2 - 24)) + 0;
			}
		}
		else if(x_abs < borders[1]){
            // 0,0125 � |X| + 0,625
			if(border_shift < 3 + 24){
				res = (x_abs >> (3 + 24 - border_shift)) + 536870912;
			}
			else{
				res = (x_abs << (border_shift - 3 - 24)) + 536870912;
			}
		}
		else if(x_abs < borders[2]){
            // 0,03125 � |X| + 0,84375
			if(border_shift < 5 + 24){
				res = (x_abs >> (5 + 24 - border_shift)) + 1476395008;
			}
			else{
				res = (x_abs << (border_shift - 5 - 24)) + 1476395008;
			}
		}
		else{
            // 1
			res = ((int64_t) 1 << 31) - 1;
		}

		if(x_data_point < 0){
            res = 0 - res;
		}

		((int32_t *) result->data)[i] = (int32_t) res;
	}

	((aimath_q31_params_t *) result->tensor_params)->shift = 32;
	((aimath_q31_params_t *) result->tensor_params)->zero_point = - ((int64_t) 1 << 31);

	return;
}

void aimath_q31_default_d_sigmoid(const aitensor_t *sigmoid_x, aitensor_t *result)
{
	uint32_t i;
	int64_t acc;

	int64_t z_sig_x = (int64_t) ((aimath_q31_params_t *) sigmoid_x->tensor_params)->zero_point;
	int64_t z_result = - ((int64_t) 1 << 31);

	uint16_t s_sig_x = ((aimath_q31_params_t *) sigmoid_x->tensor_params)->shift;

    // ToDo: Change values
	((aimath_q31_params_t *) result->tensor_params)->shift = 34;
	((aimath_q31_params_t *) result->tensor_params)->zero_point = - ((int64_t) 1 << 31);

	uint16_t output_shift = 2 * s_sig_x - ((aimath_q31_params_t *) result->tensor_params)->shift;

	for(i = 0; i < aimath_tensor_elements(sigmoid_x); i++)
	{
		// sigmoid'(x) = sigmoid(x) * (1 - sigmoid(x))
		acc = ((int64_t)((int32_t *) sigmoid_x->data)[i] - z_sig_x) * (int64_t)((int64_t)0x7FFFFFFF - (int64_t)((int32_t *) sigmoid_x->data)[i]);

        ((int32_t *) result->data)[i] = (int32_t) ((acc >> output_shift) + z_result);
	}
	return;
}

void aimath_q31_default_relu(const aitensor_t *x, aitensor_t *result) {
    uint32_t i;
    uint16_t shift = ((aimath_q31_params_t *) x->tensor_params)->shift;
    int32_t zero_point = ((aimath_q31_params_t *) x->tensor_params)->zero_point;

    for(i = 0; i < aimath_tensor_elements(x); i++)
	{
        ((int32_t *) result->data)[i] = ((int32_t *) x->data)[i] > zero_point ? ((int32_t *) x->data)[i] : zero_point;
	}

	((aimath_q31_params_t *) result->tensor_params)->shift = shift;
	((aimath_q31_params_t *) result->tensor_params)->zero_point = zero_point;

	return;
}

void aimath_q31_default_d_relu(const aitensor_t *x, aitensor_t *result)
{
	uint32_t i;

    int32_t zero_point = ((aimath_q31_params_t *) x->tensor_params)->zero_point;

	for(i = 0; i < aimath_tensor_elements(x); i++)
	{
		((int32_t *) result->data)[i] = ((int32_t *) x->data)[i] >= zero_point ? 1 : 0;
	}

	((aimath_q31_params_t *) result->tensor_params)->shift = 0;
	((aimath_q31_params_t *) result->tensor_params)->zero_point = 0;

	return;
}

void aimath_q31_default_leaky_relu(const aitensor_t *x, const void *alpha, aitensor_t *result)
{
    uint32_t i;
    int64_t acc;
    int32_t z_alpha = ((aiscalar_q31_t *) alpha)->zero_point;
	int32_t z_x = ((aimath_q31_params_t *) x->tensor_params)->zero_point;
	int32_t z_result = z_x;

	uint16_t s_x = ((aimath_q31_params_t *) x->tensor_params)->shift;
	uint16_t s_alpha = ((aiscalar_q31_t *) alpha)->shift;
	uint16_t output_shift = s_alpha; // Output has same shift as input

	for(i = 0; i < aimath_tensor_elements(x); i++)
	{
        if (((int32_t *) x->data)[i] > z_x) {
            ((int32_t *) result->data)[i] = ((int32_t *) x->data)[i];
        } else {
            // Check if zero point correction is needed
            if(z_x != 0 && z_alpha != 0){
                // q_alpha * q_x
                acc = (int64_t) ((aiscalar_q31_t *) alpha)->value * (int64_t) ((int32_t *) x->data)[i];

                // - q_x * z_alpha
                acc -= (int64_t) ((int32_t *) x->data)[i] * z_alpha;

                // - q_alpha * z_x
                acc -= (int64_t) ((aiscalar_q31_t *) alpha)->value * z_x;

                // + z_alpha * z_x
                acc += z_alpha * z_x;

                ((int32_t *) result->data)[i] = (int32_t) ((acc >> output_shift) + z_result);
            }
            else if(z_x != 0){
                // q_alpha * q_x
                acc = (int64_t) ((aiscalar_q31_t *) alpha)->value * (int64_t) ((int32_t *) x->data)[i];

                // - q_alpha * z_x
                acc -= (int64_t) ((aiscalar_q31_t *) alpha)->value * z_x;

                ((int32_t *) result->data)[i] = (int32_t) ((acc >> output_shift) + z_result);
            }
            else if(z_alpha != 0){
                // q_alpha * q_x
                acc = (int64_t) ((aiscalar_q31_t *) alpha)->value * (int64_t) ((int32_t *) x->data)[i];

                // - q_x * z_alpha
                acc -= (int64_t) ((int32_t *) x->data)[i] * z_alpha;

                ((int32_t *) result->data)[i] = (int32_t) ((acc >> output_shift) + z_result);
            }
            else{
                // q_alpha * q_x
                acc = (int64_t) ((aiscalar_q31_t *) alpha)->value * (int64_t) ((int32_t *) x->data)[i];

                ((int32_t *) result->data)[i] = (int32_t) ((acc >> output_shift) + z_result);
            }
        }
	}

	((aimath_q31_params_t *) result->tensor_params)->shift = (uint16_t)s_x;
	((aimath_q31_params_t *) result->tensor_params)->zero_point = (int32_t)z_x;

	return;
}

void aimath_q31_default_d_leaky_relu(const aitensor_t *x, const void *alpha, aitensor_t *result)
{
	uint32_t i;
    int32_t alpha_value = ((aiscalar_q31_t *) alpha)->value;
    int32_t alpha_shift = ((aiscalar_q31_t *) alpha)->shift;
    int32_t alpha_zero_point = ((aiscalar_q31_t *) alpha)->zero_point;

    int32_t zero_point = ((aimath_q31_params_t *) x->tensor_params)->zero_point;

	for(i = 0; i < aimath_tensor_elements(x); i++)
	{
		((int32_t *) result->data)[i] = ((int32_t *) x->data)[i] >= zero_point ? (((int32_t) 1<<alpha_shift) + alpha_zero_point) : alpha_value;
	}

	((aimath_q31_params_t *) result->tensor_params)->shift = alpha_shift;
	((aimath_q31_params_t *) result->tensor_params)->zero_point = alpha_zero_point;

	return;
}

void aimath_q31_default_tanh(const aitensor_t *x, aitensor_t *result)
{
	uint32_t i;
	int64_t borders[3] = {268435456, 637534208, 1342177280}; //  {1.0, 2.375, 5.0}
	uint16_t border_shift = 4 + 24;
	uint16_t x_shift = ((aimath_q31_params_t *) x->tensor_params)->shift - 1; // Reduce shift (multiply by 2)
	int32_t x_zero_point = ((aimath_q31_params_t *) x->tensor_params)->zero_point;

	int64_t res;
	int64_t x_abs;
	int64_t x_data_point;

	if(x_shift > border_shift){
		borders[0] <<= (x_shift - border_shift);
		borders[1] <<= (x_shift - border_shift);
		borders[2] <<= (x_shift - border_shift);
		border_shift = 2 * border_shift - x_shift;
	}

	for(i = 0; i < aimath_tensor_elements(x); i++)
	{
		x_data_point = (int64_t) ((int32_t *) x->data)[i] - (int64_t) x_zero_point;
		x_abs = x_data_point < 0 ? -x_data_point : x_data_point;

		if(x_shift < border_shift){
			x_abs <<= (border_shift - x_shift);
		}

		if(x_abs < borders[0]){
            // 0,25 � |X| + 0,5
			if(border_shift < 2 + 24){
				res = (x_abs >> (2 + 24 - border_shift)) + 0;
			}
			else{
				res = (x_abs << (border_shift - 2 - 24)) + 0;
			}
		}
		else if(x_abs < borders[1]){
            // 0,0125 � |X| + 0,625
			if(border_shift < 3 + 24){
				res = (x_abs >> (3 + 24 - border_shift)) + 536870912;
			}
			else{
				res = (x_abs << (border_shift - 3- 24)) + 536870912;
			}
		}
		else if(x_abs < borders[2]){
            // 0,03125 � |X| + 0,84375
			if(border_shift < 5 + 24){
				res = (x_abs >> (5 + 24 - border_shift)) + 1476395008;
			}
			else{
				res = (x_abs << (border_shift - 5 - 24)) + 1476395008;
			}
		}
		else{
            // 1
			res = ((int64_t) 1 << 31) - 1;
		}

		if(x_data_point < 0){
            res = 0 - res;
		}

		((int32_t *) result->data)[i] = (int32_t) res;
	}

	((aimath_q31_params_t *) result->tensor_params)->shift = 31;
	((aimath_q31_params_t *) result->tensor_params)->zero_point = 0;

	return;
}

void aimath_q31_default_d_tanh(const aitensor_t *tanh_x, aitensor_t *result)
{
    uint32_t i;
	int64_t acc;
    int32_t x_data;

	uint16_t s_tanh_x = ((aimath_q31_params_t *) tanh_x->tensor_params)->shift;

	((aimath_q31_params_t *) result->tensor_params)->shift = 31;
	((aimath_q31_params_t *) result->tensor_params)->zero_point = 0;

	uint16_t output_shift = 2 * s_tanh_x - ((aimath_q31_params_t *) result->tensor_params)->shift;

	for(i = 0; i < aimath_tensor_elements(tanh_x); i++)
	{
		x_data = ((int32_t *) tanh_x->data)[i];
		// tanh'(x) = 1 - tanh(x)^2
		acc = (int64_t)0x3FFFFFFFFFFFFFFF - (int64_t) x_data * (int64_t) x_data;

        ((int32_t *) result->data)[i] = (int32_t) (acc >> output_shift);
	}
	return;
}

void aimath_q31_default_softsign(const aitensor_t *x, aitensor_t *result)
{
    uint32_t i;
    int64_t acc;
    int64_t acc_denom;
    uint16_t shift = ((aimath_q31_params_t *) x->tensor_params)->shift;
    int32_t zero_point = ((aimath_q31_params_t *) x->tensor_params)->zero_point;

    for(i = 0; i < aimath_tensor_elements(x); i++)
	{
        if (((int32_t *) x->data)[i] > zero_point) {
            acc_denom = ((int32_t *) x->data)[i] - zero_point + (1 << shift);
            acc = ((int32_t *) x->data)[i] - zero_point;
            acc = (acc << 31) / acc_denom;
            ((int32_t *) result->data)[i] = acc;
        }
        else if (((int32_t *) x->data)[i] < zero_point){
            acc_denom = zero_point - ((int32_t *) x->data)[i] + (1 << shift);
            acc = ((int32_t *) x->data)[i] - zero_point;
            acc = (acc << 31) / acc_denom;
            ((int32_t *) result->data)[i] = acc;
        }
        else {
            ((int32_t *) result->data)[i] = zero_point;
        }
	}

	((aimath_q31_params_t *) result->tensor_params)->shift = 31;
	((aimath_q31_params_t *) result->tensor_params)->zero_point = 0;

	return;
}

void aimath_q31_default_d_softsign(const aitensor_t *x, aitensor_t *result)
{
    uint32_t i;
    int64_t acc;
    int64_t acc_denom;

    uint16_t shift = ((aimath_q31_params_t *) x->tensor_params)->shift;
    int32_t zero_point = ((aimath_q31_params_t *) x->tensor_params)->zero_point;

    for(i = 0; i < aimath_tensor_elements(x); i++)
	{
        if (((int32_t *) x->data)[i] > zero_point) {
            acc_denom = ((int32_t *) x->data)[i] - zero_point + (1 << shift);
            acc = ((int64_t)1 << ((shift * 2))) / ((int64_t) acc_denom);
            acc = ((acc << 31)-1) / ((int64_t) acc_denom);
            ((int32_t *) result->data)[i] = acc;
        }
        else if (((int32_t *) x->data)[i] <= zero_point){
            acc_denom = zero_point - ((int32_t *) x->data)[i] + (1 << shift);
            acc = ((int64_t)1 << ((shift * 2))) / ((int64_t) acc_denom);
            acc = ((acc << 31)-1) / ((int64_t) acc_denom);
            ((int32_t *) result->data)[i] = acc;
        }
	}

	((aimath_q31_params_t *) result->tensor_params)->shift = 31;
	((aimath_q31_params_t *) result->tensor_params)->zero_point = 0;

	return;
}

void aimath_q31_default_softmax(const aitensor_t *x, aitensor_t *result) {
    uint32_t i, j;
    // Borders for linear piecewise approximation of exponential function {-1.0, -2.0, -3.0, -5.0}
	int64_t borders[4] = {-268435456, -536870912, -805306368, -1342177280};
	uint16_t border_shift = 28;
	int32_t max;

	uint16_t x_shift = ((aimath_q31_params_t *) x->tensor_params)->shift;
	int32_t x_zero_point = ((aimath_q31_params_t *) x->tensor_params)->zero_point;

	int64_t res;
	int64_t acc = 0;
	int32_t e_x[aimath_tensor_elements(x)];
	int64_t x_data_point;

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
        max = ((int32_t *) x->data)[i * multiplier];
        for (j = 1; j < multiplier; j++) {
            if (((int32_t *) x->data)[i * multiplier + j] > max) {
                max = ((int32_t *) x->data)[i * multiplier + j];
            }
        }

        // Subtract maximum value
        x_zero_point = max;

        // calc exp functions
        acc = 0;
        for(j = 0; j < multiplier; j++)
        {
            x_data_point = (int64_t) ((int32_t *) x->data)[i * multiplier + j] - (int64_t) x_zero_point;

            if (x_data_point > borders[0]) {
                // LPA between 0 and -1
                res = ((2705829376LL * x_data_point) >> x_shift) + 2147483647;
            }
            else if (x_data_point > borders[1]) {
                // LPA between -1 and -2
                res = ((987842496 * x_data_point) >> x_shift) + 429496832;
            }
            else if (x_data_point > borders[2]) {
                // LPA between -2 and -3
                res = ((386547072 * x_data_point) >> x_shift) - 773094144;
            }
            else if (x_data_point > borders[3]) {
                // LPA between -3 and -5
                res = ((107374184 * x_data_point) >> x_shift) - 1610612736;
            }
            else {
                // LPA smaller than -5
                res = -2147483647;
            }

            acc += (res + 2147483647);
            e_x[i * multiplier + j] = (int32_t) res;
        }

        //calc softmax
        for(j = 0; j < multiplier; j++)
        {
            res = (((uint64_t)(e_x[i * multiplier + j] + 2147483647) << 32) / acc) - 2147483647;
            ((int32_t *) result->data)[i * multiplier + j] = (int32_t) res;
        }
 	}

 	// Set shift and zero_point such that output lie in interval [0, 1]
 	((aimath_q31_params_t *) result->tensor_params)->shift = 32;
	((aimath_q31_params_t *) result->tensor_params)->zero_point = -2147483647;

	return;
}

void aimath_q31_default_elu(const aitensor_t *x, const void *alpha, aitensor_t *result)
{
	uint32_t i;
	// Borders for linear piecewise approximation of exponential function {-1, -2, -3, -4}
	int64_t borders[4] = {-536870912, -1073741824, -1610612736, -2147483647};
	// y-axis intercepts of LPA {0.375, 0.6875, 0.875, 1}
	uint64_t b[4] = {201326592, 369098752, 469762048, 536870912};
	uint16_t border_shift = 29; //default value range (-4, 4)

	uint16_t x_shift = ((aimath_q31_params_t *) x->tensor_params)->shift;
	int64_t x_zero_point = (int64_t) ((aimath_q31_params_t *) x->tensor_params)->zero_point;
	uint16_t alpha_shift = ((aiscalar_q31_t *) alpha)->shift;
	int64_t alpha_zero_point = (int64_t) ((aiscalar_q31_t *) alpha)->zero_point;
    int64_t alpha_data = (int64_t) ((aiscalar_q31_t *) alpha)->value;

	int64_t res;
	int64_t x_data_point;

	if(x_shift > border_shift){
		borders[0] <<= (x_shift - border_shift);
		borders[1] <<= (x_shift - border_shift);
		borders[2] <<= (x_shift - border_shift);
		borders[3] <<= (x_shift - border_shift);
		b[0] <<= (x_shift - border_shift);
		b[1] <<= (x_shift - border_shift);
		b[2] <<= (x_shift - border_shift);
		b[3] <<= (x_shift - border_shift);
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
	}

	for(i = 0; i < aimath_tensor_elements(x); i++)
	{
		x_data_point = (int64_t) ((int32_t *) x->data)[i] - x_zero_point;

        if(x_data_point > 0){
            res = x_data_point;
        }
        else{
            if(x_data_point > borders[0]){
                // LPA between 0 and -1
                res = ((335544320 * x_data_point) >> 29);
            }
            else if(x_data_point > borders[1]){
                // LPA between -1 and -2
                res = ((134217728 * x_data_point) >> 29) - b[0];
            }
            else if(x_data_point > borders[2]){
                // LPA between -2 and -3
                res = ((50331648 * x_data_point) >> 29) - b[1];
            }
            else if(x_data_point > borders[3]){
                // LPA between -3 and -4
                res = ((16777216 * x_data_point) >> 29) - b[2];
            }
            else{
                // LPA smaller than -4
                res = -b[3];
            }

            res = res * (alpha_data - alpha_zero_point);
            res >>= alpha_shift;
        }
		((int32_t *) result->data)[i] = (int32_t) (res + x_zero_point);
	}

	// Set shift and zero_point to value range of input x
	((aimath_q31_params_t *) result->tensor_params)->shift = x_shift;
	((aimath_q31_params_t *) result->tensor_params)->zero_point = x_zero_point;

	return;
}

void aimath_q31_default_d_elu(const aitensor_t *x, const void *alpha, aitensor_t *result)
{
    uint32_t i;
    // Borders for linear piecewise approximation of exponential function {-1, -2, -3, -4}
	int64_t borders[4] = {-268435456, -536870912, -805306368, -1342177280};
	uint16_t border_shift = 28;

	uint16_t x_shift = ((aimath_q31_params_t *) x->tensor_params)->shift;
	int32_t x_zero_point = ((aimath_q31_params_t *) x->tensor_params)->zero_point;
	uint16_t alpha_shift = ((aiscalar_q31_t *) alpha)->shift;
	int64_t alpha_zero_point = (int64_t) ((aiscalar_q31_t *) alpha)->zero_point;
    int64_t alpha_data = (int64_t) ((aiscalar_q31_t *) alpha)->value;

	int64_t res;
	int64_t x_data_point;

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

    for(i = 0; i < aimath_tensor_elements(x); i++)
	{
	    x_data_point = (int64_t) ((int32_t *) x->data)[i] - x_zero_point;
	    if(x_data_point > 0){
            res = 2147483647; // result = 1
        }
        else{
            if (x_data_point > borders[0]) {
                // LPA between 0 and -1
                res = ((2705829376LL * x_data_point) >> x_shift) + 2147483647;
            }
            else if (x_data_point > borders[1]) {
                // LPA between -1 and -2
                res = ((987842496 * x_data_point) >> x_shift) + 429496832;
            }
            else if (x_data_point > borders[2]) {
                // LPA between -2 and -3
                res = ((386547072 * x_data_point) >> x_shift) - 773094144;
            }
            else if (x_data_point > borders[3]) {
                // LPA between -3 and -5
                res = ((107374184 * x_data_point) >> x_shift) - 1610612736;
            }
            else {
                // LPA smaller than -5
                res = -2147483647;
            }
            res = (res + 2147483647) * (alpha_data - alpha_zero_point);
            res >>= alpha_shift;
            res -= 2147483647;
        }

        ((int32_t *) result->data)[i] = (int32_t) res;
    }

    // Set shift and zero_point such that output lie in interval [0, 1]
	((aimath_q31_params_t *) result->tensor_params)->shift = 32;
	((aimath_q31_params_t *) result->tensor_params)->zero_point = -2147483647;

	return;
}

// Set values to zero_point
void aimath_q31_default_zero_tensor(aitensor_t *tensor)
{
	uint32_t i;
	int32_t zero_point = ((aimath_q31_params_t *) tensor->tensor_params)->zero_point;

	for(i = 0; i < aimath_tensor_elements(tensor); i++)
	{
		((int32_t *) tensor->data)[i] = zero_point;
	}
	return;
}

// Init shift, zero_point and values to zero
void aimath_q31_default_init_zeros(aitensor_t *tensor)
{
	uint32_t i;
	((aimath_q31_params_t *) tensor->tensor_params)->shift = 0;
	((aimath_q31_params_t *) tensor->tensor_params)->zero_point = 0;

	for(i = 0; i < aimath_tensor_elements(tensor); i++)
	{
		((int32_t *) tensor->data)[i] = 0;
	}
	return;
}

void aimath_q31_default_tensor_init_uniform(aitensor_t *tensor, float from, float to)
{
	uint32_t i;
	aimath_q31_params_t *tensor_params = (aimath_q31_params_t *) tensor->tensor_params;

	for(i = 0; i < aimath_tensor_elements(tensor); i++)
	{
		((int32_t *) tensor->data)[i] = FLOAT_TO_Q31(((float) rand() / (float) RAND_MAX) * (to - from) + from, tensor_params->shift, tensor_params->zero_point);
	}
	return;
}

void aimath_q31_default_init_glorot_uniform(aitensor_t *tensor)
{
	aimath_q31_default_init_glorot_uniform_cdim(tensor, 0, 1);
}

void aimath_q31_default_init_glorot_uniform_cdim(aitensor_t *tensor, int8_t cin_axis, int8_t cout_axis)
{
	uint32_t i;
	float fan_in, fan_out, fan_avg;
    uint8_t cin_uaxis = cin_axis < 0 ? tensor->dim + cin_axis : cin_axis; // Negative axis = indexing from the end
    uint8_t cout_uaxis = cout_axis < 0 ? tensor->dim + cout_axis : cout_axis; // Negative axis = indexing from the end

	fan_in = 1.0f;
	fan_out = 1.0f;
	for(i = 0; i < tensor->dim; i++){
        if(i != cout_uaxis){
            fan_in *= tensor->shape[i];
        }
        if(i != cin_uaxis){
            fan_out *= tensor->shape[i];
        }
	}

	fan_avg = (fan_in + fan_out) / 2.0f;
	float r = sqrt(3.0f / fan_avg);
	aimath_q31_default_tensor_init_uniform(tensor, -r, r);
}

void aimath_q31_default_init_he_uniform(aitensor_t *tensor)
{
	aimath_q31_default_init_he_uniform_cdim(tensor, 1);
}

void aimath_q31_default_init_he_uniform_cdim(aitensor_t *tensor, int8_t cout_axis)
{
    uint32_t i;
	float fan_in, fan_avg;
    uint8_t cout_uaxis = cout_axis < 0 ? tensor->dim + cout_axis : cout_axis; // Negative axis = indexing from the end

	fan_in = 1.0f;
	for(i = 0; i < tensor->dim; i++){
        if(i != cout_uaxis){
            fan_in *= tensor->shape[i];
        }
	}

	fan_avg = fan_in  / 2.0f;
	float r = sqrt(3.0f / fan_avg);
	aimath_q31_default_tensor_init_uniform(tensor, -r, r);
}

int64_t aimath_q31_default_sqrt(int64_t x)
{
    if(x < 2){
        return x;
    }

    int64_t lower = aimath_q31_default_sqrt(x >> 2) << 1;
    int64_t upper = lower + 1;

    if(upper * upper > x){
        return lower;
    }
    else{
        return upper;
    }
}

void aimath_q31_default_sum_channelwise(const aitensor_t *x, int8_t channel_axis, aitensor_t *result){
    uint32_t i, j, k;
    uint32_t idx_multiplier1 = 1, idx_multiplier2 = 1;
    uint8_t uaxis = channel_axis < 0 ? x->dim + channel_axis : channel_axis; // Negative axis = indexing from the end

    for(i = 0; i < uaxis; i++){
        idx_multiplier1 *= x->shape[i];
    }
    for(i = uaxis+1; i < x->dim; i++){
        idx_multiplier2 *= x->shape[i];
    }

    for(i = 0; i < x->shape[uaxis]; i++){
        ((int32_t *) result->data)[i] = ((aimath_q31_params_t *) result->tensor_params)->zero_point;
        for(j = 0; j < idx_multiplier1; j++){
            for(k = 0; k < idx_multiplier2; k++){
                ((int32_t *) result->data)[i] += ((int32_t *) x->data)[i*idx_multiplier2 + j*idx_multiplier2*x->shape[uaxis] + k];
            }
        }
        ((int32_t *) result->data)[i] -= idx_multiplier1 * idx_multiplier2 * ((aimath_q31_params_t *) x->tensor_params)->zero_point;
    }
    return;
}

void aimath_q31_default_mse_gradients_sum(const aitensor_t *predicted, const aitensor_t *target, aitensor_t *result)
{
    int32_t temp_data[aimath_sizeof_tensor(predicted)];
    aitensor_t temp_tensor = AITENSOR_2D_Q31(predicted->shape, (aimath_q31_params_t *) predicted->tensor_params, temp_data);

    aimath_q31_default_tensor_sub_different_shift(predicted, target, &temp_tensor);

    float factor = 2.0f;

    aiscalar_q31_t factor_q31 = AISCALAR_Q31(factor, 16, 0);

    aimath_q31_default_scalar_mul(&factor_q31, &temp_tensor, result);

    return;
}

void aimath_q31_default_mse_gradients_mean(const aitensor_t *predicted, const aitensor_t *target, aitensor_t *result)
{
    int32_t temp_data[aimath_sizeof_tensor(predicted)];
    aitensor_t temp_tensor = AITENSOR_2D_Q31(predicted->shape, (aimath_q31_params_t *) predicted->tensor_params, temp_data);

    aimath_q31_default_tensor_sub_different_shift(predicted, target, &temp_tensor);

    float factor = 2.0f / (float) aimath_tensor_elements(predicted);

    aiscalar_q31_t factor_q31 = AISCALAR_Q31(factor, 16, 0);

    aimath_q31_default_scalar_mul(&factor_q31, &temp_tensor, result);

    return;
}

void aimath_q31_default_mse_loss_sum(const aitensor_t *predicted, const aitensor_t *target, void *result)
{
    int32_t temp_data[aimath_sizeof_tensor(predicted)];
    aitensor_t temp_tensor = AITENSOR_2D_Q31(predicted->shape, (aimath_q31_params_t *) predicted->tensor_params, temp_data);

    aimath_q31_default_tensor_sub_different_shift(predicted, target, &temp_tensor);
    aimath_q31_default_norm_squared(&temp_tensor, result);

    return;
}

void aimath_q31_default_mse_loss_mean(const aitensor_t *predicted, const aitensor_t *target, void *result)
{
    // calculate sum
    aimath_q31_default_mse_loss_sum(predicted, target, result);

    // scale with tensor size
    float factor = 1.0f / (float) aimath_tensor_elements(predicted);

    aiscalar_q31_t scalar_t = AISCALAR_Q31(factor, 16, 0);

    aiscalar_q31_t *scalar = &scalar_t;

	int64_t acc;

	int64_t z_scalar = (int64_t) ((aiscalar_q31_t *) scalar)->zero_point; // scalar
	int64_t z_result = (int64_t) ((aiscalar_q31_t *) result)->zero_point; // a

	uint16_t output_shift = ((aiscalar_q31_t *) scalar)->shift - ((aiscalar_q31_t *) result)->shift;

	// Check if zero point correction is needed
	if(z_result != 0 && z_scalar != 0){
        // q_scalar * q_result
        acc = (int64_t) ((aiscalar_q31_t *) scalar)->value * (int64_t) ((aiscalar_q31_t *) result)->value;

        // - q_result * z_scalar
        acc -= (int64_t) ((aiscalar_q31_t *) result)->value * z_scalar;

        // - q_scalar * z_result
        acc -= (int64_t) ((aiscalar_q31_t *) scalar)->value * z_result;

        // + z_scalar * z_result
        acc += z_scalar * z_result;

        ((aiscalar_q31_t *)result)->value = (int32_t) ((acc >> output_shift) + z_result);
	}
	else if(z_result != 0){
        // q_scalar * q_result
        acc = (int64_t) ((aiscalar_q31_t *) scalar)->value * (int64_t) ((aiscalar_q31_t *) result)->value;

        // - q_scalar * z_result
        acc -= (int64_t) ((aiscalar_q31_t *) scalar)->value * z_result;

        ((aiscalar_q31_t *)result)->value = (int32_t) ((acc >> output_shift) + z_result);
    }
	else if(z_scalar != 0){
        // q_scalar * q_result
        acc = (int64_t) ((aiscalar_q31_t *) scalar)->value * (int64_t) ((aiscalar_q31_t *) result)->value;

        // - q_result * z_scalar
        acc -= (int64_t) ((aiscalar_q31_t *) result)->value * z_scalar;

        ((aiscalar_q31_t *)result)->value = (int32_t) ((acc >> output_shift) + z_result);
	}
	else{
        // q_scalar * q_result
        acc = (int64_t) ((aiscalar_q31_t *) scalar)->value * (int64_t) ((aiscalar_q31_t *) result)->value;

        ((aiscalar_q31_t *)result)->value = (int32_t) ((acc >> output_shift) + z_result);
	}
	return;
}

