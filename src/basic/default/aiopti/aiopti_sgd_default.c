/**
 * \file basic/default/aiopti/aiopti_sgd_default.c
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

#include "basic/default/aiopti/aiopti_sgd_default.h"

aiopti_t *aiopti_sgd_f32_default(aiopti_sgd_f32_t *opti)
{
    aiopti_t* return_opti;

    opti->base.base.dtype = aif32;

	// Call "constructor" of base "class"
    return_opti = aiopti_sgd(&opti->base);

    return_opti->learning_rate = &(opti->learning_rate);
    opti->base.momentum = &opti->momentum;

    // Check if a momentum is set and configure the right functions
    if (opti->momentum != 0.0f) {
        return_opti->update_params = aiopti_sgd_update_params_with_momentum;
        return_opti->sizeof_optimem = aiopti_sgd_sizeof_optimem_with_momentum;
        return_opti->init_optimem = aiopti_sgd_init_optimem_with_momentum;
    }
    else {
        return_opti->update_params = aiopti_sgd_update_params_without_momentum;
        return_opti->sizeof_optimem = aiopti_sgd_sizeof_optimem_without_momentum;
        return_opti->init_optimem = aiopti_sgd_init_optimem_without_momentum;
    }

	// Set f32 math functions of sgd optimizer
    opti->base.zero_tensor = aimath_f32_default_zero_tensor;
	opti->base.tensor_add = aimath_f32_default_tensor_add;
	opti->base.tensor_sub = aimath_f32_default_tensor_sub;
	opti->base.scalar_mul = aimath_f32_default_scalar_mul;

	return return_opti;
}

aiopti_t *aiopti_sgd_q31_default(aiopti_sgd_q31_t *opti)
{
	aiopti_t* return_opti;

    opti->base.base.dtype = aiq31;

	// Call "constructor" of base "class"
    return_opti = aiopti_sgd(&opti->base);

    return_opti->learning_rate = &(opti->learning_rate);
    opti->base.momentum = &opti->momentum;

    // Check if a momentum is set and configure the right functions
    if (opti->momentum.value != opti->momentum.zero_point) {
        return_opti->update_params = aiopti_sgd_update_params_with_momentum;
        return_opti->sizeof_optimem = aiopti_sgd_sizeof_optimem_with_momentum;
        return_opti->init_optimem = aiopti_sgd_init_optimem_with_momentum;
    }
    else {
        return_opti->update_params = aiopti_sgd_update_params_without_momentum;
        return_opti->sizeof_optimem = aiopti_sgd_sizeof_optimem_without_momentum;
        return_opti->init_optimem = aiopti_sgd_init_optimem_without_momentum;
    }

	// Set q31 math functions of sgd optimizer
    opti->base.zero_tensor = aimath_q31_default_zero_tensor;
	opti->base.tensor_add = aimath_q31_default_tensor_add_same_shift;
	opti->base.tensor_sub = aimath_q31_default_tensor_sub_different_shift;
	opti->base.scalar_mul = aimath_q31_default_scalar_mul;

	return return_opti;
}

