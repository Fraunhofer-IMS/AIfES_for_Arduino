/**
 * \file basic/base/aiopti/aiopti_adam.h
 * \internal
 * \date 12.11.2020
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
 * \brief Base \link aiopti optimizer \endlink implementation of the [Adam optimizer](https://arxiv.org/pdf/1412.6980.pdf)
 *
 * This is an "abstract" data-type independent implementation. To use the optimizer, use one of the provided
 * implementations for a specific hardware and data-type (for example from aiopti_adam_default.h) or set
 * the required math functions on your own.
 *
 * The [Adam optimizer](https://arxiv.org/pdf/1412.6980.pdf) is based on SGD and uses first-order and second-order moments for adaptive estimation.
 * It uses the pre-calculated gradients to optimize the given parameters.
 * For every parameter \f$ p \f$ of the parameters to optimize (trainable parameters) and the related gradient \f$ g \f$  it calculates
 * @f[
 *  m_t = \beta_1 \cdot m_{t-1} + (1 - \beta_1) \cdot g_t
 * @f]
 * @f[
 *  v_t = \beta_2 \cdot v_{t-1} + (1 - \beta_2) \cdot g^2_t
 * @f]
 * @f[
 *  p_t = p_{t-1} - lr_t \cdot \frac{m_t} {\sqrt{v_t} + \hat{\epsilon}}
 * @f]
 * in every optimization step with \f$ lr_t = lr \cdot \frac{\sqrt{1 - \beta^t_2}} {(1 - \beta_1)^t} \f$.\n
 * \f$ lr \f$ is the learning rate that defines how big the optimization steps should be,
 * and therefore how fast the training will be.
 * \f$ m \f$ and \f$ v \f$ are the first- and second-order moments related to the parameter and must be stored in the optimization memory
 * for every parameter.
 */

#ifndef AIOPTI_ADAM
#define AIOPTI_ADAM

#include "core/aifes_core.h"

typedef struct aiopti_adam 				aiopti_adam_t; /**< New data type name for code reduction. */
typedef struct aiopti_adam_momentums 	aiopti_adam_momentums_t; /**< New data type name for code reduction. */

/** @brief General \link aiopti_adam.h Adam optimizer \endlink struct
*
*/
struct aiopti_adam {
	aiopti_t base; /**< Inherited field members from general optimizer struct. */

    /** @name Optimizer configuration
	 * @brief Required configuration parameters for the optimizer
	 *
	 * These fields have to be configured by the user before calling the initializer function.
	 */
	///@{
	void *beta1; /**< aiscalar: Exponential decay rate for the first moment estimates \f$ \beta_1 \f$ */
	void *beta2; /**< aiscalar: Exponential decay rate for the second moment estimates \f$ \beta_2 \f$ */
	void *eps; /**< aiscalar: Small positive number for numerical stability \f$ \hat{\epsilon} \f$ (avoid dividing by 0)*/
	///@}

	/** @name Variables for internal computation
     *
     * These fields are automatically configured in the initializer function.
	 */
	///@{
	void *beta1t; /**< aiscalar: Auxiliary variable to calculate \f$ \beta_1^t \f$ */
	void *beta2t; /**< aiscalar: Auxiliary variable to calculate \f$ \beta_2^t \f$ */
	void *one_minus_beta1; /**< aiscalar: Auxiliary variable to calculate \f$ (1 - \beta_1) \f$ */
	void *one_minus_beta2; /**< aiscalar: Auxiliary variable to calculate \f$ (1 - \beta_2) \f$ */
	void *lrt; /**< aiscalar: Auxiliary variable to calculate \f$ lr_t = lr \cdot \frac{\sqrt{1-\beta_2^t}}{(1-\beta_1^t)} \f$ */
	///@}

    /** @name Math functions
	 * @brief Required data type specific math functions
	 */
	///@{

	/** @brief Required math function: Element wise tensor multiplication
	 *
	 * Requires a math function that multiplies two tensors element wise:\n
     * @f[
     *  result = a \odot b
     * @f]
	 */
	void (*multiply)(const aitensor_t *a, const aitensor_t *b, aitensor_t *result);

	/** @brief Required math function: Element wise tensor dividation
	 *
	 * Requires a math function that devides two tensors element wise:\n
     * @f[
     *  result_i = a_i \div b_i
     * @f]
	 */
	void (*divide)(const aitensor_t *a, const aitensor_t *b, aitensor_t *result);

	/** @brief Required math function: Element wise tensor addition
	 *
	 * Requires a math function that adds two tensors element wise:\n
     * @f[
     *  result = a + b
     * @f]
	 */
	void (*tensor_add)(const aitensor_t *a, const aitensor_t *b, aitensor_t *result);

	/** @brief Required math function: Element wise tensor subtraction
	 *
	 * Requires a math function that subtracts two tensors element wise:\n
     * @f[
     *  result = a - b
     * @f]
	 */
	void (*tensor_sub)(const aitensor_t *a, const aitensor_t *b, aitensor_t *result);

	/** @brief Required math function: Multiplication of a scalar with a tensor
	 *
	 * Requires a math function that multiplies a scalar with a tensor:\n
     * @f[
     *  result = scalar \cdot a
     * @f]
	 */
	void (*scalar_mul)(const void *scalar, const aitensor_t *a, aitensor_t *result);

	/** @brief Required math function: Element wise addition of a scalar to a tensor
	 *
	 * Requires a math function that adds a scalar to every element of a tensor:\n
     * @f[
     *  result = scalar \cdot \left( \begin{array}{c} 1 & \cdots & 1 \\ \vdots & \ddots & \vdots \\ 1 & \cdots & 1 \\ \end{array}\right) + a
     * @f]
	 */
	void (*scalar_add)(const void *scalar, const aitensor_t *a, aitensor_t *result);

	/** @brief Required math function: Square root
	 *
	 * Requires a math function that calculates the element wise square root of a tensor:\n
     * @f[
     *  result_{i} = \sqrt{x_i}
     * @f]
	 */
	void (*sqrt)(const aitensor_t *x, aitensor_t *result);

	/** @brief Required math function: Sets the elements of a tensor to zero
	 *
	 * Requires a math function that sets the elements of the given tensor to zero (or to the zero_point):\n
     * @f[
     *  tensor_i = 0
     * @f]
	 */
	void (*zero_tensor)(aitensor_t *tensor);

    ///@{
};

/** @brief Adam optimizer type
 *
 * Defines the type of the optimizer (for example for type checks and debug prints).
 * See aicore_optitype for more information about the optimizer type.
 */
extern const aicore_optitype_t *aiopti_adam_type;

/** @brief Struct for the momentum tensors of an Adam optimizer
 *
 * This structure is made for easy access of the momentum tensors on the optimization memory.
 *
 */
struct aiopti_adam_momentums {
	aitensor_t m; /**< First momentum vector. */
	aitensor_t v; /**< Second momentum vector. */
};

/** @brief Initialize the given Adam optimizer
 *
 * This function represents the "constructor" of the abstract Adam optimizer.\n
 * This function is not intended to call it directly. Instead use one of the data type specific implementations
 * (like for example aiopti_adam_f32_default()).
 *
 * @param *opti        The optimizer to initialize.
 * @return  Pointer to the (successfully) initialized general optimizer structure (aiopti_adam.base)
 */
aiopti_t *aiopti_adam(aiopti_adam_t *opti);

/** @brief Calculates the required memory for the optimization step
 *
 * *Implementation of aiopti.sizeof_optimem.*
 *
 * Calculates the size of the memory space that must be reserved.
 * The memory is used for the first and second moment tensors and is calculated by:
 *
 * \code 2 * (sizeof(aitensor) + sizeof(params.data)) \endcode
 *
 * @param *self     The optimizer
 * @param *params   The tensor of trainable parameters to calculate the memory for
 */
uint32_t aiopti_adam_sizeof_optimem(aiopti_t *self, const aitensor_t *params);

/** @brief Initialization of the optimization memory buffer
 *
 * *Implementation of aiopti.init_optimem.*
 *
 * Initialize the first and second moment tensors with zeros:
 * @f[
 *  m_{0,i} \leftarrow 0
 * @f]
 * @f[
 *  v_{0,i} \leftarrow 0
 * @f]
 *
 * Used math functions:
 * * aiopti_adam.zero_tensor
 *
 * @param *self         The optimizer
 * @param *params       The tensor of trainable parameters
 * @param *gradients    The gradients associated to the parameters
 * @param *optimem      The optimization memory (containing the first and second moment) associated to the parameters
 */
void aiopti_adam_init_optimem(aiopti_t *self, const aitensor_t *params, const aitensor_t *gradients, void *optimem);

/** @brief Set the gradients to zero
 *
 * *Implementation of aiopti.zero_gradients.*
 *
 * @f[
 *  g_{i} \leftarrow 0
 * @f]
 *
 * Used math functions:
 * * aiopti_adam.zero_tensor
 *
 * @param *self         The optimizer
 * @param *gradients    The gradients to set to zero
 */
void aiopti_adam_zero_gradients(aiopti_t *self, aitensor_t *gradients);

/** @brief Update the given parameter tensor with respect to the gradients
 *
 * *Implementation of aiopti.update_params.*
 *
 * Calculate and update the values of the trainable parameters (perform one update step):
 * @f[
 *  m_t \leftarrow \beta_1 \cdot m_{t-1} + (1 - \beta_1) \cdot g_t
 * @f]
 * @f[
 *  v_t \leftarrow \beta_2 \cdot v_{t-1} + (1 - \beta_2) \cdot g^2_t
 * @f]
 * @f[
 *  p_t \leftarrow p_{t-1} - lr_t \cdot \frac{m_t} {\sqrt{v_t} + \hat{\epsilon}}
 * @f]
 *
 * \f$ m \f$:	 First moment estimates\n
 * \f$ v \f$:	 Second moment estimates\n
 * \f$ p \f$:	 Tensor of trainable parameters to update (params)\n
 * \f$ g \f$:	 Gradients\n
 * \f$ lr \f$:	 Learning rate / Optimization step size\n
 * \f$ \beta_1 \f$:	 Exponential decay rate for the first moment estimates\n
 * \f$ \beta_2 \f$:	 Exponential decay rate for the second moment estimates\n
 * \f$ \hat{\epsilon} \f$:	 Small positive number for numerical stability\n\n
 *
 * Used math functions:
 * * aiopti_adam.scalar_mul
 * * aiopti_adam.tensor_add
 * * aiopti_adam.multiply
 * * aiopti_adam.sqrt
 * * aiopti_adam.scalar_add
 * * aiopti_adam.divide
 * * aiopti_adam.tensor_sub
 *
 * @param *self         The optimizer
 * @param *params       The tensor of trainable parameters \f$ p \f$ to update
 * @param *gradients    The gradients \f$ g \f$ associated to the parameters
 * @param *optimem      The buffer to store the first and second momentums \f$ m \f$ and \f$ v \f$
 */
void aiopti_adam_update_params(aiopti_t *self, aitensor_t *params, const aitensor_t *gradients, void *optimem);

#ifdef AIDEBUG_PRINT_MODULE_SPECS
/** @brief Print the optimizer specification
 *
 * @param *self     The optimizer to print the specification for
 */
void aiopti_adam_print_specs(const aiopti_t *self);
#endif // AIDEBUG_PRINT_MODULE_SPECS

#endif // AIOPTI_ADAM

