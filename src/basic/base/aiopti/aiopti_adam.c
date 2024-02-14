/**
 * \file basic/base/aiopti/aiopti_adam.c
 * \version 2.2.0
 * \date 12.11.2020
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

#include "basic/base/aiopti/aiopti_adam.h"
#include "basic/base/aimath/aimath_basic.h"

AISTRING_STORAGE_WRAPPER(aistring_opti_adam, "Adam");

const aicore_optitype_t aiopti_adam_type_s = {
#ifdef AIDEBUG_PRINT_MODULE_SPECS
    .name = aistring_opti_adam,
	.print_specs = aiopti_adam_print_specs
#else
    .name = 0,
    .print_specs = 0
#endif
};
const aicore_optitype_t *aiopti_adam_type = &aiopti_adam_type_s;

aiopti_t *aiopti_adam(aiopti_adam_t *opti)
{
    opti->base.optimizer_type = aiopti_adam_type;
	opti->base.optimizer_configuration = opti;

	opti->base.sizeof_optimem = aiopti_adam_sizeof_optimem;
	opti->base.init_optimem = aiopti_adam_init_optimem;
	opti->base.zero_gradients = aiopti_adam_zero_gradients;
	opti->base.update_params = aiopti_adam_update_params;

	return (&opti->base);
}

uint32_t aiopti_adam_sizeof_optimem(aiopti_t *self, const aitensor_t *params){
	uint32_t memory = 0;
	memory += sizeof(aiopti_adam_momentums_t);
	memory += 2 * aimath_sizeof_tensor_data(params);
	return memory;
}

void aiopti_adam_init_optimem(aiopti_t *self, const aitensor_t *params, const aitensor_t *gradients, void *optimem)
{
	aiopti_adam_t *opti = (aiopti_adam_t *)(self->optimizer_configuration);
	uint32_t address_counter = 0;
	aiopti_adam_momentums_t *momentums = optimem;
	address_counter += sizeof(aiopti_adam_momentums_t);

	momentums->m.dtype = gradients->dtype;
	momentums->m.dim = gradients->dim;
	momentums->m.shape = gradients->shape;
	momentums->m.tensor_params = gradients->tensor_params;
	momentums->m.data = optimem + address_counter;
	address_counter += aimath_sizeof_tensor_data(&momentums->m);

	// No alignment needed, because momentums should have same dtype

	momentums->v.dtype = gradients->dtype;
	momentums->v.dim = gradients->dim;
	momentums->v.shape = gradients->shape;
	momentums->v.tensor_params = gradients->tensor_params;
	momentums->v.data = optimem + address_counter;
	address_counter += aimath_sizeof_tensor_data(&momentums->v);

	opti->zero_tensor(&momentums->m);
	opti->zero_tensor(&momentums->v);

	return;
}

void aiopti_adam_zero_gradients(aiopti_t *self, aitensor_t *gradients)
{
	aiopti_adam_t *opti = (aiopti_adam_t *)(self->optimizer_configuration);

    opti->zero_tensor(gradients);

	return;
}

void aiopti_adam_update_params(aiopti_t *self, aitensor_t *params, const aitensor_t *gradients, void *optimem)
{
	aiopti_adam_t *opti = (aiopti_adam_t *)(self->optimizer_configuration);

	aiopti_adam_momentums_t *momentums = optimem;

	uint8_t temp_tensor_data[aimath_sizeof_tensor_data(gradients)];
	aitensor_t temp_tensor = {
		.dim = gradients->dim,
		.shape = gradients->shape,
		.data = temp_tensor_data,
		.dtype = gradients->dtype,
		.tensor_params = gradients->tensor_params
	};

	// m = beta1 * m + (1-beta1) * g
	opti->scalar_mul(opti->beta1, &momentums->m, &momentums->m);
	opti->scalar_mul(opti->one_minus_beta1, gradients, &temp_tensor);
	opti->tensor_add(&momentums->m, &temp_tensor, &momentums->m);

	// v = beta2 * v + (1-beta2) * g*g
	opti->scalar_mul(opti->beta2, &momentums->v, &momentums->v);
	opti->multiply(gradients, gradients, &temp_tensor);
	opti->scalar_mul(opti->one_minus_beta2, &temp_tensor, &temp_tensor);
	opti->tensor_add(&momentums->v, &temp_tensor, &momentums->v);

	// lrt = lr * sqrt(1-beta2^t) / (1-beta1)^t
	// params = params - lrt * m / (sqrt(v) + eps)
	opti->sqrt(&momentums->v, &temp_tensor);
	opti->scalar_add(opti->eps, &temp_tensor, &temp_tensor);
	opti->divide(&momentums->m, &temp_tensor, &temp_tensor);
	opti->scalar_mul(opti->lrt, &temp_tensor, &temp_tensor);

	temp_tensor.tensor_params = params->tensor_params; // switch params like shift to the parameter tensor params
	opti->tensor_sub(params, &temp_tensor, params);

	return;
}

#ifdef AIDEBUG_PRINT_MODULE_SPECS
AISTRING_STORAGE_WRAPPER(aistring_opti_adam_print_specs_1, "lr: ");
AISTRING_STORAGE_WRAPPER(aistring_opti_adam_print_specs_2, "; beta1: ");
AISTRING_STORAGE_WRAPPER(aistring_opti_adam_print_specs_3, "; beta2: ");
AISTRING_STORAGE_WRAPPER(aistring_opti_adam_print_specs_4, "; eps: ");

void aiopti_adam_print_specs(const aiopti_t *self)
{
    aiopti_adam_t *self_casted = (aiopti_adam_t *) self->optimizer_configuration;

    AIPRINT(aistring_opti_adam_print_specs_1);
    self->dtype->print_aiscalar(self->learning_rate);
    AIPRINT(aistring_opti_adam_print_specs_2);
    self->dtype->print_aiscalar(self_casted->beta1);
    AIPRINT(aistring_opti_adam_print_specs_3);
    self->dtype->print_aiscalar(self_casted->beta2);
    AIPRINT(aistring_opti_adam_print_specs_4);
    self->dtype->print_aiscalar(self_casted->eps);
    return;
}
#endif
