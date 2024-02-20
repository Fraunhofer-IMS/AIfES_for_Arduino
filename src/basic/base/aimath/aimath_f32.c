/**
 * \file basic/base/aimath/aimath_f32.c
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
 * \brief 	Basic functions for f32 datatypes
 */

#include "basic/base/aimath/aimath_f32.h"

AISTRING_STORAGE_WRAPPER(aistring_dtype_f32, "F32");

const aimath_dtype_t aif32_s = {
#ifdef AIDEBUG_PRINT_MODULE_SPECS
    .name = aistring_dtype_f32,
#else
    .name = 0,
#endif
	.size = 4,
	.tensor_params_size = 0,
	.print_aitensor = aimath_f32_print_aitensor,
	.print_aiscalar = aimath_f32_print_aiscalar
};
const aimath_dtype_t *aif32 = &aif32_s;

AISTRING_STORAGE_WRAPPER(aistring_f32_print_aitensor_tab, "\t");
AISTRING_STORAGE_WRAPPER(aistring_f32_print_aitensor_nl, "\n");
AISTRING_STORAGE_WRAPPER(aistring_f32_print_aitensor_1, "F32 [");
AISTRING_STORAGE_WRAPPER(aistring_f32_print_aitensor_2, "]\n");

void aimath_f32_print_aitensor(const aitensor_t *tensor)
{
	uint16_t i, j, k, n;
	AIPRINT(aistring_f32_print_aitensor_1);
	if(tensor->dim == 1)
	{
		for(j = 0; j < tensor->shape[0]; j++)
		{
			AIPRINT_FLOAT("%10.5f", ((float *) tensor->data)[j]);
			AIPRINT(aistring_f32_print_aitensor_tab);
		}
	}
	else if(tensor->dim == 2)
	{
	    AIPRINT(aistring_f32_print_aitensor_nl);
		for(i = 0; i < tensor->shape[0]; i++)
		{
			for(j = 0; j < tensor->shape[1]; j++)
			{
				AIPRINT_FLOAT("%10.5f", ((float *) tensor->data)[i*tensor->shape[1] + j]);
                AIPRINT(aistring_f32_print_aitensor_tab);
			}
			AIPRINT(aistring_f32_print_aitensor_nl);
		}
	}
	else if(tensor->dim == 4)
	{
	    AIPRINT(aistring_f32_print_aitensor_nl);
		float (*tensor_data)[tensor->shape[0]][tensor->shape[1]][tensor->shape[2]][tensor->shape[3]] = tensor->data;

		for(n = 0; n < tensor->shape[0]; n++)
		{
			for(k = 0; k < tensor->shape[1]; k++)
			{
				for(i = 0; i < tensor->shape[2]; i++)
				{
					for(j = 0; j < tensor->shape[3]; j++)
					{
						AIPRINT_FLOAT("%10.5f", (*tensor_data)[n][k][i][j]);
						AIPRINT(aistring_f32_print_aitensor_tab);
					}
					AIPRINT(aistring_f32_print_aitensor_nl);
				}
				AIPRINT(aistring_f32_print_aitensor_nl);
			}
			AIPRINT(aistring_f32_print_aitensor_nl);
		}
	}
	AIPRINT(aistring_f32_print_aitensor_2);
	return;
}

AISTRING_STORAGE_WRAPPER(aistring_f32_print_aiscalar_1, " (F32)");

void aimath_f32_print_aiscalar(const void *scalar)
{
    AIPRINT_FLOAT("%f", *((float *) scalar));
    AIPRINT(aistring_f32_print_aiscalar_1);
}
