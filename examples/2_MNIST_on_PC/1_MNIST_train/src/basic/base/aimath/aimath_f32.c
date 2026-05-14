/**
 * \file basic/base/aimath/aimath_f32.c
 * \version 2.0alpha
 * \date 28.10.2020
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
 * \brief 	Basic functions for f32 datatypes
 */

#include "basic/base/aimath/aimath_f32.h"

const aimath_dtype_t aif32_s = {
#ifdef AIDEBUG_PRINT_MODULE_SPECS
    .name = "F32",
#else
    .name = 0,
#endif
	.size = 4,
	.tensor_params_size = 0,
	.print_aitensor = aimath_f32_print_aitensor,
	.print_aiscalar = aimath_f32_print_aiscalar
};

const aimath_dtype_t *aif32 = &aif32_s;


void aimath_f32_print_aitensor(const aitensor_t *tensor)
{
	uint16_t i, j, k, n;
	printf("F32 [\n");
	if(tensor->dim == 1)
	{
		for(j = 0; j < tensor->shape[0]; j++)
		{
			printf("%10.5f\t", ((float *) tensor->data)[j]);
		}
	}
	else if(tensor->dim == 2)
	{
		for(i = 0; i < tensor->shape[0]; i++)
		{
			for(j = 0; j < tensor->shape[1]; j++)
			{
				printf("%10.5f\t", ((float *) tensor->data)[i*tensor->shape[1] + j]);
			}
			printf("\n");
		}
	}
	else if(tensor->dim == 4)
	{
		float (*tensor_data)[tensor->shape[0]][tensor->shape[1]][tensor->shape[2]][tensor->shape[3]] = tensor->data;

		for(n = 0; n < tensor->shape[0]; n++)
		{
			for(k = 0; k < tensor->shape[1]; k++)
			{
				for(i = 0; i < tensor->shape[2]; i++)
				{
					for(j = 0; j < tensor->shape[3]; j++)
					{
						printf("%10.5f\t", (*tensor_data)[n][k][i][j]);
					}
					printf("\n");
				}
				printf("\n");
			}
			printf("\n");
		}
	}
	printf("]\n");
	return;
}

void aimath_f32_print_aiscalar(const void *scalar, int (*print)(const char *format, ...))
{
    print("%f (F32)", *((float *) scalar));
}
