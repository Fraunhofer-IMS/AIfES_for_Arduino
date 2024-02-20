/**
 * \file basic/base/aiopti/aiopti_sgd.h
 * \internal
 * \date 20.10.2020
 * \endinternal
 * \version 2.2.0
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
 * \brief Base \link aiopti optimizer \endlink implementation of the Stochastic Gradient Descent (with momentum) optimizer
 *
 * This is an "abstract" data-type independent implementation. To use the optimizer, use one of the provided
 * implementations for a specific hardware and data-type (for example from aiopti_sgd_default.h) or set
 * the required math functions on your own.
 *
 * The Stochastic Gradient Descent (SGD) optimizer is the most basic optimizer in backpropagation.
 * It uses the pre-calculated gradients to optimize the given parameters. In addition, a momentum term can be configured.\n
 * For every parameter \f$ p \f$ of the parameters to optimize (trainable parameters) and the related gradient \f$ g \f$  it calculates
 * @f[
 *  p_t = p_{t-1} - lr \cdot g_t
 * @f]
 * if the momentum \f$ \mu = 0 \f$ and
 * @f[
 *  v_t = \mu \cdot v_{t-1} + g_t
 * @f]
 * @f[
 *  p_t = p_{t-1} - lr \cdot v_t
 * @f]
 * if the momentum \f$ \mu \neq 0 \f$ in every optimization step.\n
 * \f$ lr \f$ is the learning rate that defines how big the optimization steps should be,
 * and therefore how fast the training will be.
 * \f$ v \f$ is the momentum term or velocity related to the parameter and must be stored in the optimization memory
 * for every parameter when momentum is set.
 */

#ifndef AIOPTI_SGD
#define AIOPTI_SGD

#include "core/aifes_core.h"

typedef struct aiopti_sgd 	aiopti_sgd_t; /**< New data type name for code reduction. */

/** @brief General \link aiopti_sgd.h Stochastic Gradient Descent (SGD) optimizer \endlink struct
*
*/
struct aiopti_sgd {
	aiopti_t base; /**< Inherited field members from general optimizer struct. */

    /** @name Optimizer configuration
	 * @brief Required configuration parameters for the optimizer
	 *
	 * These fields have to be configured by the user before calling the initializer function.
	 */
	///@{
	void *momentum; /**< aiscalar: Momentum(set to null to save optimization memory) */
	///@}

    /** @name Math functions
	 * @brief Required data type specific math functions
	 */
	///@{

	/** @brief Required math function: Element wise tensor subtraction
	 *
	 * Requires a math function that subtracts two tensors element wise:\n
     * @f[
     *  result = a - b
     * @f]
	 */
	void (*tensor_sub)(const aitensor_t *a, const aitensor_t *b, aitensor_t *result);

	/** @brief Required math function: Element wise tensor addition
	 *
	 * Requires a math function that adds two tensors element wise:\n
     * @f[
     *  result = a + b
     * @f]
	 */
	void (*tensor_add)(const aitensor_t *a, const aitensor_t *b, aitensor_t *result);

	/** @brief Required math function: Multiplication of a scalar with a tensor
	 *
	 * Requires a math function that multiplies a scalar with a tensor:\n
     * @f[
     *  result = scalar \cdot a
     * @f]
	 */
	void (*scalar_mul)(const void *scalar, const aitensor_t *a, aitensor_t *result);

	/** @brief Required math function: Sets the elements of a tensor to zero
	 *
	 * Requires a math function that sets the elements of the given tensor to zero (or to the zero_point):\n
     * @f[
     *  tensor_i = 0
     * @f]
	 */
	void (*zero_tensor)(aitensor_t *tensor);

	///@}
};

/** @brief SGD optimizer type
 *
 * Defines the type of the optimizer (for example for type checks and debug prints).
 * See aicore_optitype for more information about the optimizer type.
 */
extern const aicore_optitype_t *aiopti_sgd_type;


/** @brief Initialize the given SGD optimizer
 *
 * This function represents the "constructor" of the abstract SGD optimizer.\n
 * This function is not intended to call it directly. Instead use one of the data type specific implementations
 * (like for example aiopti_sgd_f32_default()).
 *
 * @param *opti        The optimizer to initialize.
 * @return  Pointer to the (successfully) initialized general optimizer structure (aiopti_sgd.base)
 */
aiopti_t *aiopti_sgd(aiopti_sgd_t *opti);

/** @brief Calculates the required memory for the optimization step when the momentum is not zero
 *
 * *Implementation of aiopti.sizeof_optimem.*
 *
 * Calculates the size of the memory space that must be reserved.
 * The memory is used for the velocity tensor (momentum term) and is calculated by:
 *
 * \code sizeof(aitensor) + sizeof(params.data) \endcode
 *
 * @param *self     The optimizer
 * @param *params   The tensor of trainable parameters to calculate the memory for
 */
uint32_t aiopti_sgd_sizeof_optimem_with_momentum(aiopti_t *self, const aitensor_t *params);

/** @brief Calculates the required memory for the optimization step when the momentum is zero
 *
 * *Implementation of aiopti.sizeof_optimem.*
 *
 * Calculates the size of the memory space that must be reserved.
 * The required memory is zero because no velocity term is needed.
 *
 * @param *self     The optimizer
 * @param *params   The tensor of trainable parameters to calculae the memory for
 */
uint32_t aiopti_sgd_sizeof_optimem_without_momentum(aiopti_t *self, const aitensor_t *params);

/** @brief Initialization of the optimization memory buffer when the momentum is not zero
 *
 * *Implementation of aiopti.init_optimem.*
 *
 * Initialize the velocity tensor with zeros:
 * @f[
 *  v_{0,i} \leftarrow 0
 * @f]
 *
 * Used math functions:
 * * aiopti_sgd.zero_tensor
 *
 * @param *self         The optimizer
 * @param *params       The tensor of trainable parameters
 * @param *gradients    The gradients associated to the parameters
 * @param *optimem      The optimization memory (containing the velocities) associated to the parameters
 */
void aiopti_sgd_init_optimem_with_momentum(aiopti_t *self, const aitensor_t *params, const aitensor_t *gradients, void *optimem);

/** @brief Initialization of the optimization memory buffer when the momentum is zero
 *
 * *Implementation of aiopti.init_optimem.*
 *
 * Does nothing because no optimization memory is needed in this case.
 *
 * @param *self         The optimizer
 * @param *params       The tensor of trainable parameters
 * @param *gradients    The gradients associated to the parameters
 * @param *optimem      The optimization memory associated to the parameters
 */
void aiopti_sgd_init_optimem_without_momentum(aiopti_t *self, const aitensor_t *params, const aitensor_t *gradients, void *optimem);

/** @brief Set the gradients to zero
 *
 * *Implementation of aiopti.zero_gradients.*
 *
 * @f[
 *  g_{i} \leftarrow 0
 * @f]
 *
 * Used math functions:
 * * aiopti_sgd.zero_tensor
 *
 * @param *self         The optimizer
 * @param *gradients    The gradients to set to zero
 */
void aiopti_sgd_zero_gradients(aiopti_t *self, aitensor_t *gradients);

/** @brief Update the given parameter tensor with respect to the gradients when the momentum is not zero
 *
 * *Implementation of aiopti.update_params.*
 *
 * Calculate and update the values of the trainable parameters (perform one update step):
 * @f[
 *  v_t \leftarrow \mu \cdot v_{t-1} + g_t
 * @f]
 * @f[
 *  p_t \leftarrow p_{t-1} - lr \cdot v_t
 * @f]
 *
 * \f$ v \f$:	 Velocity tensor (momentum term), stored in the optimem\n
 * \f$ p \f$:	 Tensor of trainable parameters to update (params)\n
 * \f$ g \f$:	 Gradients\n
 * \f$ lr \f$:	 Learning rate / Optimization step size\n
 * \f$ \mu \f$:	 Momentum\n\n
 *
 * Used math functions:
 * * aiopti_sgd.scalar_mul
 * * aiopti_sgd.tensor_add
 * * aiopti_sgd.tensor_sub
 *
 * @param *self         The optimizer
 * @param *params       The tensor of trainable parameters \f$ p \f$ to update
 * @param *gradients    The gradients \f$ g \f$ associated to the parameters
 * @param *optimem      The buffer to store the velocity \f$ v \f$
 */
void aiopti_sgd_update_params_with_momentum(aiopti_t *self, aitensor_t *params, const aitensor_t *gradients, void *optimem);

/** @brief Update the given parameter tensor with respect to the gradients when the momentum is zero
 *
 * *Implementation of aiopti.update_params.*
 *
 * Calculate and update the values of the trainable parameters (perform one update step):
 * @f[
 *  p_t \leftarrow p_{t-1} - lr \cdot g_t
 * @f]
 *
 * Used math functions:
 * * aiopti_sgd.scalar_mul
 * * aiopti_sgd.tensor_sub
 *
 * \f$ p \f$:	 Tensor of trainable parameters to update (params)\n
 * \f$ g \f$:	 Gradients\n
 * \f$ lr \f$:	 Learning rate / Optimization step size\n\n
 *
 * @param *self         The optimizer
 * @param *params       The tensor of trainable parameters \f$ p \f$ to update
 * @param *gradients    The gradients \f$ g \f$ associated to the parameters
 * @param *optimem      Not required because no velocity is stored
 */
void aiopti_sgd_update_params_without_momentum(aiopti_t *self, aitensor_t *params, const aitensor_t *gradients, void *optimem);

#ifdef AIDEBUG_PRINT_MODULE_SPECS
/** @brief Print the optimizer specification
 *
 * @param *self     The optimizer to print the specification for
 */
void aiopti_sgd_print_specs(const aiopti_t *self);
#endif // AIDEBUG_PRINT_MODULE_SPECS

#endif // AIOPTI_SGD

