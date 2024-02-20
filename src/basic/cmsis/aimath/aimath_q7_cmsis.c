/**
 * \file basic/cmsis/aimath/aimath_q7_cmsis.c
 * \version 2.2.0
 * \date 15.03.2021
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

#include "aifes_config.h"

#if __arm__
#ifdef AIFES_WITH_CMSIS

#include "basic/cmsis/aimath/aimath_q7_cmsis.h"
#include "CMSIS/NN/Include/arm_nnfunctions.h"

AISTRING_STORAGE_WRAPPER(aistring_error_q7_linear32_cmsis_1, "[aimath_q7_cmsis_linear32] MatMul input shapes doesn't match.\n");
AISTRING_STORAGE_WRAPPER(aistring_error_q7_linear32_cmsis_2, "[aimath_q7_cmsis_linear32] MatMul output shape doesn't match.\n");
AISTRING_STORAGE_WRAPPER(aistring_error_q7_linear32_cmsis_3, "[aimath_q7_cmsis_linear32] Third operand shift does not match.\n");

void aimath_q7_cmsis_linear32_bt(const aitensor_t *a, const aitensor_t *b, const aitensor_t *c, aitensor_t *result)
{
	uint16_t i;
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
		AILOG_E(aistring_error_q7_linear32_cmsis_1);
		return;
	}
	if(a->shape[0] != result->shape[0] || b->shape[0] != result->shape[1])
	{
		AILOG_E(aistring_error_q7_linear32_cmsis_2);
		return;
	}
#endif
#ifdef AIDEBUG_GENERAL_CHECKS
	if(c_shift != a_shift + b_shift)
	{
		AILOG_E(aistring_error_q7_linear32_cmsis_3);
		return;
	}
#endif // AIDEBUG_GENERAL_CHECKS

    int8_t *a_data_row = a_data;
    int8_t *result_data_row = result_data;
    for(i = 0; i < a->shape[0]; i++){
        arm_nn_vec_mat_mult_t_s8(a_data_row,
                                 b_data,
                                 c_data,
                                 result_data_row,
                                 -z_a,
                                 0,
                                 z_result,
                                 0x7FFFFFFF,
                                 -output_shift,
                                 b->shape[1],
                                 b->shape[0],
                                 -128,
                                 127);
        a_data_row += a->shape[1];
        result_data_row += result->shape[1];
    }

	return;
}

#endif // AIFES_WITH_CMSIS
#endif //__arm__
