/**
 * \file basic/base/aimath/aimath_q31.c
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

#include "basic/base/aimath/aimath_q31.h"

AISTRING_STORAGE_WRAPPER(aistring_dtype_q31, "Q31");

const aimath_dtype_t aiq31_s = {
#ifdef AIDEBUG_PRINT_MODULE_SPECS
    .name = aistring_dtype_q31,
#else
    .name = 0,
#endif
	.size = 4,
	.tensor_params_size = sizeof(aimath_q31_params_t),
	.print_aitensor = aimath_q31_print_aitensor,
	.print_aiscalar = aimath_q31_print_aiscalar
};
const aimath_dtype_t *aiq31 = &aiq31_s;

AISTRING_STORAGE_WRAPPER(aistring_q31_print_aitensor_bracket, " (");
AISTRING_STORAGE_WRAPPER(aistring_q31_print_aitensor_tab, ")\t");
AISTRING_STORAGE_WRAPPER(aistring_q31_print_aitensor_nl, "\n");
AISTRING_STORAGE_WRAPPER(aistring_q31_print_aitensor_1, "Q31 (S: ");
AISTRING_STORAGE_WRAPPER(aistring_q31_print_aitensor_2, "; ZP: ");
AISTRING_STORAGE_WRAPPER(aistring_q31_print_aitensor_3, ") [\n");
AISTRING_STORAGE_WRAPPER(aistring_q31_print_aitensor_4, "]\n");

void aimath_q31_print_aitensor(const aitensor_t *tensor)
{
	uint16_t i, j, k, n;

	uint16_t shift = ((aimath_q31_params_t *) tensor->tensor_params)->shift;
	int32_t zero_point = ((aimath_q31_params_t *) tensor->tensor_params)->zero_point;

	int32_t *tensor_data = (int32_t *) tensor->data;

	AIPRINT(aistring_q31_print_aitensor_1);
	AIPRINT_UINT("%u", (unsigned int) shift);
	AIPRINT(aistring_q31_print_aitensor_2);
	AIPRINT_LONG_INT("%ld", (long int) zero_point);
	AIPRINT(aistring_q31_print_aitensor_3);
	if(tensor->dim == 1)
	{
		for(j = 0; j < tensor->shape[0]; j++)
		{
			AIPRINT_FLOAT("%10.5f", Q31_TO_FLOAT(tensor_data[j], shift, zero_point));
            AIPRINT(aistring_q31_print_aitensor_bracket);
            AIPRINT_LONG_INT("%10ld", tensor_data[j]);
            AIPRINT(aistring_q31_print_aitensor_tab);
		}
	}
	else if(tensor->dim == 2)
	{
		for(i = 0; i < tensor->shape[0]; i++)
		{
			for(j = 0; j < tensor->shape[1]; j++)
			{
				AIPRINT_FLOAT("%10.5f", Q31_TO_FLOAT(tensor_data[i*tensor->shape[1] + j], shift, zero_point));
				AIPRINT(aistring_q31_print_aitensor_bracket);
				AIPRINT_LONG_INT("%10ld", (long int) tensor_data[i*tensor->shape[1] + j]);
				AIPRINT(aistring_q31_print_aitensor_tab);
			}
			AIPRINT(aistring_q31_print_aitensor_nl);
		}
	}
	else if(tensor->dim == 4)
	{
		for(n = 0; n < tensor->shape[0]; n++)
		{
			for(k = 0; k < tensor->shape[1]; k++)
			{
				for(i = 0; i < tensor->shape[2]; i++)
				{
					for(j = 0; j < tensor->shape[3]; j++)
					{
						AIPRINT_FLOAT("%10.5f\t", Q31_TO_FLOAT(tensor_data[((n * tensor->shape[1] + k) * tensor->shape[2] + i) * tensor->shape[3] + j], shift, zero_point));
						AIPRINT(aistring_q31_print_aitensor_bracket);
						AIPRINT_LONG_INT("%10ld", (long int) tensor_data[((n * tensor->shape[1] + k) * tensor->shape[2] + i) * tensor->shape[3] + j]);
						AIPRINT(aistring_q31_print_aitensor_tab);
					}
					AIPRINT(aistring_q31_print_aitensor_nl);
				}
				AIPRINT(aistring_q31_print_aitensor_nl);
			}
			AIPRINT(aistring_q31_print_aitensor_nl);
		}
	}
	AIPRINT(aistring_q31_print_aitensor_4);
	return;
}

AISTRING_STORAGE_WRAPPER(aistring_q31_print_aiscalar_1, " (Q31 | V: ");
AISTRING_STORAGE_WRAPPER(aistring_q31_print_aiscalar_2, "; S: ");
AISTRING_STORAGE_WRAPPER(aistring_q31_print_aiscalar_3, "; ZP: ");
AISTRING_STORAGE_WRAPPER(aistring_q31_print_aiscalar_4, ")");

void aimath_q31_print_aiscalar(const void *scalar)
{
    aiscalar_q31_t *scalar_q31 = (aiscalar_q31_t *) scalar;
    AIPRINT_FLOAT("%f", Q31_TO_FLOAT(scalar_q31->value, scalar_q31->shift, scalar_q31->zero_point));
    AIPRINT(aistring_q31_print_aiscalar_1);
    AIPRINT_LONG_INT("%ld", (long int) scalar_q31->value);
    AIPRINT(aistring_q31_print_aiscalar_2);
    AIPRINT_UINT("%u", (unsigned int) scalar_q31->shift);
    AIPRINT(aistring_q31_print_aiscalar_3);
    AIPRINT_LONG_INT("%ld", (long int) scalar_q31->zero_point);
    AIPRINT(aistring_q31_print_aiscalar_4);
}

void aimath_q31_calc_q_params_from_f32(float min_value, float max_value, aimath_q31_params_t *q_params)
{
    int32_t min_target = -2147483647;
    uint8_t target_interval_bitlen = 32;
    uint8_t value_interval_bitlen;

    float interval_old = max_value - min_value;
    float interval_new;

    // Find the smalles 2^n interval that fits interval_old
    // value_interval_bitlen = (int) ceil(log2(interval_old))
    for(value_interval_bitlen = 0; (1 << value_interval_bitlen) <= (int32_t)(interval_old); value_interval_bitlen++){
    }
    interval_new = (float) (1 << value_interval_bitlen);

    // Adapt the old interval borders to the new ones
    float min_new = min_value - (interval_new - interval_old) / 2.0f;

    uint16_t shift = target_interval_bitlen - value_interval_bitlen;
    int32_t zero_point = (int32_t)(-min_new * (float) (1 << shift) + ((-min_value) >= 0 ? 0.5f : -0.5f)) + min_target;

    q_params->shift = shift;
    q_params->zero_point = zero_point;
}

void aimath_q31_quantize_tensor_from_f32(const aitensor_t *tensor_f32, aitensor_t *tensor_q31)
{
    uint32_t i;
    uint16_t shift = ((aimath_q31_params_t *) tensor_q31->tensor_params)->shift;
    int32_t zero_point = ((aimath_q31_params_t *) tensor_q31->tensor_params)->zero_point;

    for(i = 0; i < aimath_tensor_elements(tensor_f32); i++){
        ((int32_t *) tensor_q31->data)[i] = FLOAT_TO_Q31(((float *) tensor_f32->data)[i], shift, zero_point);
    }

    return;
}
