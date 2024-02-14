/**
 * \file basic/cmsis/aimath/aimath_f32_cmsis.c
 * \version 2.2.0
 * \date 16.03.2021
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

#include "basic/cmsis/aimath/aimath_f32_cmsis.h"
#include "CMSIS/DSP/Include/arm_math.h"


AISTRING_STORAGE_WRAPPER(aistring_error_f32_linear_cmsis_1, "[aimath_f32_cmsis_linear] MatMul input shapes doesn't match.\n");
AISTRING_STORAGE_WRAPPER(aistring_error_f32_linear_cmsis_2, "[aimath_f32_cmsis_linear] MatMul output shape doesn't match.\n");

/**
* Math CMSIS Matrix Multiplication and broadcast add
*
* Matrixmultiplication using the CMSIS DSP Library and broadcast add using for loop
*
*/
void aimath_f32_cmsis_linear(const aitensor_t *a, const aitensor_t *b, const aitensor_t *c, aitensor_t *result)
{

#ifdef AIDEBUG_SHAPE_CHECKS
	if(a->shape[1] != b->shape[0])
	{
        #ifdef AIDEBUG_PRINT_ERROR_MESSAGES
            AILOG_E(aistring_error_f32_linear_cmsis_1);
        #endif
		return;
	}
	if(a->shape[0] != result->shape[0] || b->shape[1] != result->shape[1])
	{
        #ifdef AIDEBUG_PRINT_ERROR_MESSAGES
            AILOG_E(aistring_error_f32_linear_cmsis_2);
        #endif
		return;
	}
#endif

	uint16_t i, j;

	float *c_data = c != 0 ? (float *) c->data : 0;
	float *result_data = (float *) result->data;


	aimath_f32_cmsis_mat_mul(a, b, result);


	for(i = 0; i < result->shape[0]; i++)
	{
		for(j = 0; j < result->shape[1]; j++)
		{
			result_data[i*result->shape[1] + j] += c_data[j];
		}
	}



	return;
}


/**
* Math CMSIS Matrix Multiplication
*
* Matrixmultiplication using the CMSIS DSP Library
*
* \ref https://arm-software.github.io/CMSIS_5/DSP/html/group__MatrixMult.html
*/
void aimath_f32_cmsis_mat_mul(const aitensor_t *a, const aitensor_t *b, aitensor_t *result)
{
	float *a_data = (float *) a->data;
	float *b_data = (float *) b->data;
	float *result_data = (float *) result->data;


	arm_matrix_instance_f32 a_mat;      /* Matrix a Instance */
	arm_matrix_instance_f32 b_mat;		/* Matrix b Instance */
	arm_matrix_instance_f32 result_mat;	/* Matrix result Instance */

	arm_mat_init_f32(&a_mat, a->shape[0], a->shape[1], a_data);
	arm_mat_init_f32(&b_mat, b->shape[0], b->shape[1], b_data);
	arm_mat_init_f32(&result_mat, result->shape[0], result->shape[1], result_data);


	arm_mat_mult_f32(&a_mat, &b_mat, &result_mat);
}

#endif // AIFES_WITH_CMSIS
#endif // __arm__
