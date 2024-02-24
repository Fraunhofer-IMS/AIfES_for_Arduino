/**
 * \file basic/base/aiopti/aiopti_sgd.c
 * \version 2.0alpha
 * \date 20.10.2020
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
 * \brief
 * \details
 */

#include "basic/base/aiopti/aiopti_sgd.h"
#include "basic/base/aimath/aimath_basic.h"

const aicore_optitype_t aiopti_sgd_type_s = {
#ifdef AIDEBUG_PRINT_MODULE_SPECS
    .name = "SGD",
	.print_specs = aiopti_sgd_print_specs
#else
    .name = 0,
    .print_specs = 0
#endif
};
const aicore_optitype_t *aiopti_sgd_type = &aiopti_sgd_type_s;

aiopti_t *aiopti_sgd(aiopti_sgd_t *opti)
{
    opti->base.optimizer_type = aiopti_sgd_type;
	opti->base.optimizer_configuration = opti;

	opti->base.begin_step = 0;
	opti->base.end_step = 0;
	opti->base.zero_gradients = aiopti_sgd_zero_gradients;

	// Need to be set in the inherited implementation
    opti->base.update_params = 0;
    opti->base.sizeof_optimem = 0;
    opti->base.init_optimem = 0;

	return &opti->base;
}


uint32_t aiopti_sgd_sizeof_optimem_with_momentum(aiopti_t *self, const aitensor_t *params){
	uint32_t memory = 0;
	memory += sizeof(aitensor_t);
	memory += aimath_sizeof_tensor_data(params);
	return memory;
}

uint32_t aiopti_sgd_sizeof_optimem_without_momentum(aiopti_t *self, const aitensor_t *params){
	return 0;
}

void aiopti_sgd_init_optimem_with_momentum(aiopti_t *self, const aitensor_t *params, const aitensor_t *gradients, void *optimem)
{
	aiopti_sgd_t *opti = (aiopti_sgd_t *)(self->optimizer_configuration);
	uint32_t address_counter = 0;
	aitensor_t *v = (aitensor_t *) optimem;
	address_counter += sizeof(aitensor_t);

	v->dtype = gradients->dtype;
	v->dim = gradients->dim;
	v->shape = gradients->shape;
	v->tensor_params = gradients->tensor_params;
	v->data = optimem + address_counter;
	address_counter += aimath_sizeof_tensor_data(v);

	opti->zero_tensor(v);

	return;
}

void aiopti_sgd_init_optimem_without_momentum(aiopti_t *self, const aitensor_t *params, const aitensor_t *gradients, void *optimem)
{
	return;
}

void aiopti_sgd_zero_gradients(aiopti_t *self, aitensor_t *gradients)
{
	aiopti_sgd_t *opti = (aiopti_sgd_t *)(self->optimizer_configuration);

    opti->zero_tensor(gradients);

	return;
}

void aiopti_sgd_update_params_with_momentum(aiopti_t *self, aitensor_t *params, const aitensor_t *gradients, void *optimem)
{
	aiopti_sgd_t *opti = (aiopti_sgd_t *)(self->optimizer_configuration);

	aitensor_t *v = (aitensor_t *) optimem;

	uint8_t temp_tensor_data[aimath_sizeof_tensor_data(params)];
	aitensor_t temp_tensor = {
		.dim = gradients->dim,
		.shape = gradients->shape,
		.data = temp_tensor_data,
		.dtype = gradients->dtype,
		.tensor_params = gradients->tensor_params
	};

	// v = momentum * v + gradients
	opti->scalar_mul(opti->momentum, v, &temp_tensor);
	opti->tensor_add(&temp_tensor, gradients, v);

	// p = p - lr * v
    opti->scalar_mul(opti->base.learning_rate, v, &temp_tensor);
    opti->tensor_sub(params, &temp_tensor, params);

	return;
}

void aiopti_sgd_update_params_without_momentum(aiopti_t *self, aitensor_t *params, const aitensor_t *gradients, void *optimem)
{
	aiopti_sgd_t *opti = (aiopti_sgd_t *)(self->optimizer_configuration);

	uint8_t temp_tensor_data[aimath_sizeof_tensor_data(params)];
	aitensor_t temp_tensor = {
		.dim = gradients->dim,
		.shape = gradients->shape,
		.data = temp_tensor_data,
		.dtype = gradients->dtype,
		.tensor_params = gradients->tensor_params
	};

	// p = p - lr * g
    opti->scalar_mul(opti->base.learning_rate, gradients, &temp_tensor);
    opti->tensor_sub(params, &temp_tensor, params);
	return;
}

#ifdef AIDEBUG_PRINT_MODULE_SPECS
void aiopti_sgd_print_specs(const aiopti_t *self, int (*print)(const char *format, ...))
{
    aiopti_sgd_t *self_casted = (aiopti_sgd_t *) self->optimizer_configuration;

    print("lr: ");
    self->dtype->print_aiscalar(self->learning_rate, print);

    print("; momentum: ");
    self->dtype->print_aiscalar(self_casted->momentum, print);
    return;
}
#endif
