/**
 * \file basic/base/aimath/aimath_basic.c
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
 * \brief	Basic math operations for aitensor_t
 * \details Basic functions for aitensor_t handling, e.g. calculation of size, printing etc.
 */

#include "basic/base/aimath/aimath_basic.h"

void print_aitensor(const aitensor_t *tensor)
{
	tensor->dtype->print_aitensor(tensor);
	return;
}

void print_aiscalar(const void *scalar, const aimath_dtype_t *dtype)
{
    dtype->print_aiscalar(scalar);
	return;
}

void aimath_transpose_vector(aitensor_t *vector)
{
	uint16_t temp;
	temp = vector->shape[0];
	vector->shape[0] = vector->shape[1];
	vector->shape[1] = temp;
	return;
}

uint32_t aimath_tensor_elements(const aitensor_t *tensor)
{
	uint32_t elems = 1;
	uint8_t i;

	for(i = 0; i < tensor->dim; i++)
	{
		elems *= tensor->shape[i];
	}
	return elems;
}

uint32_t aimath_sizeof_dtype(const aimath_dtype_t *dtype)
{
	return dtype->size;
}

uint32_t aimath_sizeof_tensor_data(const aitensor_t *tensor)
{
	return aimath_sizeof_dtype(tensor->dtype) * aimath_tensor_elements(tensor);
}

uint32_t aimath_sizeof_tensor_params(const aitensor_t *tensor)
{
	return tensor->dtype->tensor_params_size;
}

uint32_t aimath_sizeof_tensor(const aitensor_t *tensor)
{
	return sizeof(aitensor_t) + aimath_sizeof_tensor_data(tensor) + aimath_sizeof_tensor_params(tensor);
}
