/**
 * \file basic/base/aimath/aimath_q7.c
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

#include "basic/base/aimath/aimath_q7.h"

AISTRING_STORAGE_WRAPPER(aistring_dtype_q7, "Q7");

const aimath_dtype_t aiq7_s = {
#ifdef AIDEBUG_PRINT_MODULE_SPECS
    .name = aistring_dtype_q7,
#else
    .name = 0,
#endif
	.size = 1,
	.tensor_params_size = sizeof(aimath_q7_params_t),
	.print_aitensor = aimath_q7_print_aitensor,
	.print_aiscalar = aimath_q7_print_aiscalar
};
const aimath_dtype_t *aiq7 = &aiq7_s;

AISTRING_STORAGE_WRAPPER(aistring_q7_print_aitensor_bracket, " (");
AISTRING_STORAGE_WRAPPER(aistring_q7_print_aitensor_tab, ")\t");
AISTRING_STORAGE_WRAPPER(aistring_q7_print_aitensor_nl, "\n");
AISTRING_STORAGE_WRAPPER(aistring_q7_print_aitensor_1, "Q7 (S: ");
AISTRING_STORAGE_WRAPPER(aistring_q7_print_aitensor_2, "; ZP: ");
AISTRING_STORAGE_WRAPPER(aistring_q7_print_aitensor_3, ") [\n");
AISTRING_STORAGE_WRAPPER(aistring_q7_print_aitensor_4, "]\n");

void aimath_q7_print_aitensor(const aitensor_t *tensor)
{
	uint16_t i, j, k, n;

	uint16_t shift = ((aimath_q7_params_t *) tensor->tensor_params)->shift;
	int8_t zero_point = ((aimath_q7_params_t *) tensor->tensor_params)->zero_point;

	int8_t *tensor_data = (int8_t *) tensor->data;

	AIPRINT(aistring_q7_print_aitensor_1);
	AIPRINT_UINT("%u", (unsigned int) shift);
	AIPRINT(aistring_q7_print_aitensor_2);
	AIPRINT_INT("%d", zero_point);
	AIPRINT(aistring_q7_print_aitensor_3);
	if(tensor->dim == 1)
	{
		for(j = 0; j < tensor->shape[0]; j++)
		{
			AIPRINT_FLOAT("%10.5f", Q7_TO_FLOAT(tensor_data[j], shift, zero_point));
            AIPRINT(aistring_q7_print_aitensor_bracket);
            AIPRINT_INT("%4d", tensor_data[j]);
            AIPRINT(aistring_q7_print_aitensor_tab);
		}
	}
	else if(tensor->dim == 2)
	{
		for(i = 0; i < tensor->shape[0]; i++)
		{
			for(j = 0; j < tensor->shape[1]; j++)
			{
				AIPRINT_FLOAT("%10.5f", Q7_TO_FLOAT(tensor_data[i*tensor->shape[1] + j], shift, zero_point));
				AIPRINT(aistring_q7_print_aitensor_bracket);
				AIPRINT_INT("%4d", tensor_data[i*tensor->shape[1] + j]);
				AIPRINT(aistring_q7_print_aitensor_tab);
			}
			AIPRINT(aistring_q7_print_aitensor_nl);
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
						AIPRINT_FLOAT("%10.5f\t", Q7_TO_FLOAT(tensor_data[((n * tensor->shape[1] + k) * tensor->shape[2] + i) * tensor->shape[3] + j], shift, zero_point));
						AIPRINT(aistring_q7_print_aitensor_bracket);
						AIPRINT_INT("%4d", tensor_data[((n * tensor->shape[1] + k) * tensor->shape[2] + i) * tensor->shape[3] + j]);
						AIPRINT(aistring_q7_print_aitensor_tab);
					}
					AIPRINT(aistring_q7_print_aitensor_nl);
				}
				AIPRINT(aistring_q7_print_aitensor_nl);
			}
			AIPRINT(aistring_q7_print_aitensor_nl);
		}
	}
	AIPRINT(aistring_q7_print_aitensor_4);
	return;
}

AISTRING_STORAGE_WRAPPER(aistring_q7_print_aiscalar_1, " (Q7 | V: ");
AISTRING_STORAGE_WRAPPER(aistring_q7_print_aiscalar_2, "; S: ");
AISTRING_STORAGE_WRAPPER(aistring_q7_print_aiscalar_3, "; ZP: ");
AISTRING_STORAGE_WRAPPER(aistring_q7_print_aiscalar_4, ")");

void aimath_q7_print_aiscalar(const void *scalar)
{
    aiscalar_q7_t *scalar_q7 = (aiscalar_q7_t *) scalar;
    AIPRINT_FLOAT("%f", Q7_TO_FLOAT(scalar_q7->value, scalar_q7->shift, scalar_q7->zero_point));
    AIPRINT(aistring_q7_print_aiscalar_1);
    AIPRINT_INT("%d", scalar_q7->value);
    AIPRINT(aistring_q7_print_aiscalar_2);
    AIPRINT_UINT("%u", (unsigned int) scalar_q7->shift);
    AIPRINT(aistring_q7_print_aiscalar_3);
    AIPRINT_INT("%d", scalar_q7->zero_point);
    AIPRINT(aistring_q7_print_aiscalar_4);
}

AISTRING_STORAGE_WRAPPER(aistring_error_q7_calc_q_params_from_f32_1, "[aimath_q7_calc_q_params_from_f32] min_value has to be smaller than max_value.\n");
AISTRING_STORAGE_WRAPPER(aistring_error_q7_calc_q_params_from_f32_2, "[aimath_q7_calc_q_params_from_f32] One or more values are too big to quantize them to a 8 bit integer.\n");

void aimath_q7_calc_q_params_from_f32(float min_value, float max_value, aimath_q7_params_t *q_params)
{
    int8_t min_target = -128;
    uint8_t target_interval_bitlen = 8;
    int8_t value_interval_bitlen;

    if(max_value == 0 && min_value == 0){
        q_params->shift = 0;
        q_params->zero_point = 0;
        return;
    }

    // An interval that does not include the zero has to be handled special
    if(min_value > 0 && max_value > 0){
        min_value = 0;
    } else if(min_value < 0 && max_value < 0){
        max_value = 0;
    }

    float interval_old = max_value - min_value;
    float interval_new;

    #ifdef AIDEBUG_GENERAL_CHECKS
        if(min_value > max_value)
        {
            AILOG_E(aistring_error_q7_calc_q_params_from_f32_1);
            return;
        }
    #endif // AIDEBUG_GENERAL_CHECKS

    // Find the smalles 2^n interval that fits interval_old
    interval_new = 1.0f / (float) ((uint32_t) 1 << 24);
    for(value_interval_bitlen = -24; interval_new <= interval_old; value_interval_bitlen++){
        interval_new *= 2;
    }

    #ifdef AIDEBUG_GENERAL_CHECKS
        if((int16_t) target_interval_bitlen < value_interval_bitlen)
        {
            AILOG_E(aistring_error_q7_calc_q_params_from_f32_2);
            return;
        }
    #endif // AIDEBUG_GENERAL_CHECKS

    // Adapt the old interval borders to the new ones
    float min_new = min_value - (interval_new - interval_old) / 2.0f;

    uint16_t shift = target_interval_bitlen - value_interval_bitlen;
    int8_t zero_point = (int8_t)(-min_new * (float) (1 << shift) + ((-min_value) >= 0 ? 0.5f : -0.5f)) + min_target;

    q_params->shift = shift;
    q_params->zero_point = zero_point;
    return;
}

void aimath_q7_quantize_tensor_from_f32(const aitensor_t *tensor_f32, aitensor_t *tensor_q7)
{
    uint32_t i;
    uint16_t shift = ((aimath_q7_params_t *) tensor_q7->tensor_params)->shift;
    int8_t zero_point = ((aimath_q7_params_t *) tensor_q7->tensor_params)->zero_point;

    for(i = 0; i < aimath_tensor_elements(tensor_f32); i++){
        ((int8_t *) tensor_q7->data)[i] = FLOAT_TO_Q7(((float *) tensor_f32->data)[i], shift, zero_point);
    }

    return;
}


