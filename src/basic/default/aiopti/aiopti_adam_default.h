/**
 * \file basic/default/aiopti/aiopti_adam_default.h
 * \internal
 * \date 28.10.2020
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
 * \brief Default implementation of the \link aiopti_adam.h Adam optimizer \endlink
 *
 * Hardware independent implementations of the Adam optimizer in \link aimath_f32.h F32 \endlink,
 * \link aimath_q31.h Q31 \endlink data-type.
 * For more information about the Adam optimizer refer to aiopti_adam.h.
 */

#ifndef AIOPTI_ADAM_DEFAULT
#define AIOPTI_ADAM_DEFAULT

#include "basic/base/aiopti/aiopti_adam.h"

#include "basic/default/aimath/aimath_f32_default.h"
#include "basic/default/aimath/aimath_q7_default.h"

#define AIOPTI_ADAM_F32(learning_rate, beta1, beta2, eps) {{{0,},},learning_rate, beta1, beta2, eps}
#define AIOPTI_ADAM_Q31(learning_rate, beta1, beta2, eps) {{{0,},},learning_rate, beta1, beta2, eps}

typedef struct aiopti_adam_f32 	aiopti_adam_f32_t;
//typedef struct aiopti_adam_q31 	aiopti_adam_q31_t;

/** @brief Data-type specific \link aiopti_adam.h Adam optimizer \endlink struct for \link aimath_f32.h F32 \endlink
 *
 * Adds data fields for the learning rate and the configuration values in \link aimath_f32.h F32 \endlink to the base implementation.
 */
struct aiopti_adam_f32 {
	aiopti_adam_t base; /**< Inherited field members from general optimizer struct. */

    /** @name Optimizer configuration
	 * @brief Required configuration parameters for the optimizer
     *
     * These fields have to be configured by the user before calling the initializer function.
	 */
	///@{
	aiscalar_f32_t learning_rate; /**< Storage for aiopti.learning_rate scalar in F32 */

	aiscalar_f32_t beta1; /**< Storage for aiopti_adam.beta1 scalar in F32 */
	aiscalar_f32_t beta2; /**< Storage for aiopti_adam.beta2 scalar in F32 */
	aiscalar_f32_t eps; /**< Storage for aiopti_adam.eps scalar in F32 */
	///@}

	/** @name Variables for internal computation
     *
     * These fields are automatically configured in the initializer function.
	 */
	///@{
	aiscalar_f32_t beta1t; /**< Storage for aiopti_adam.beta1t scalar in F32 */
	aiscalar_f32_t beta2t; /**< Storage for aiopti_adam.beta2t scalar in F32 */
	aiscalar_f32_t one_minus_beta1; /**< Storage for aiopti_adam.one_minus_beta1 scalar in F32 */
	aiscalar_f32_t one_minus_beta2; /**< Storage for aiopti_adam.one_minus_beta2 scalar in F32 */
	aiscalar_f32_t lrt; /**< Storage for aiopti_adam.lrt scalar in F32 */
	///@}
};


/** @brief Initializes an \link aiopti_adam.h Adam optimizer \endlink with the \link aimath_f32.h F32 \endlink default implementation
 *
 * **Example:** Create the optimizer structure:\n
 * In C:
 * \code{.c}
 * aiopti_adam_f32_t adam_optimizer = {
 *     .learning_rate = 0.01f,
 *
 *     .beta1 = 0.9f,
 *     .beta2 = 0.999f,
 *     .eps = 1e-7f
 * };
 * \endcode
 * In C, C++ and on Arduino:
 * \code{.c}
 * aiopti_adam_f32_t adam_optimizer = AIOPTI_ADAM_F32(0.01f, 0.9f, 0.999f, 1e-7f);
 * \endcode
 *
 * **Example:** Initialize the optimizer:\n
 * \code{.c}
 * aiopti_t *optimizer;
 *
 * optimizer = aiopti_adam_f32_default(&adam_optimizer);
 * \endcode
 *
 * @param *opti    The optimizer structure to initialize.
 * @return         The (successfully) initialized optimizer structure.
 */
aiopti_t *aiopti_adam_f32_default(aiopti_adam_f32_t *opti);

/** @brief \link aimath_f32.h F32 \endlink default implementation of the aiopti.begin_step function for ADAM
 *
 * *Implementation of aiopti.begin_step.*
 *
 * The ADAM optimizer needs some modification of the learning rate in every optimization step.
 * This function deals with aiscalars and has to be implemented for every data-type individually.
 *
 * The calculations are:\n
 * @f[
 *      lr_t \leftarrow lr \cdot \frac{\sqrt{1 - \beta^t_2}} {(1 - \beta_1)^t}
 * @f]
 *
 * This is not primary time critical function, because it only deals with scalars -> No special hardware implementation necessary (but possible).
 *
 * @param *self  The optimizer structure
 */
void aiopti_adam_f32_default_begin_step(aiopti_t *self);

/** @brief \link aimath_f32.h F32 \endlink default implementation of the aiopti.end_step function for ADAM
 *
 * *Implementation of aiopti.end_step.*
 *
 * The ADAM optimizer needs some modification of the learning rate in every optimization step.
 * This function deals with aiscalars and has to be implemented for every data-type individually.
 *
 * The calculations are:\n
 * @f[
 *      \beta^t_1 \leftarrow \beta^t_1 \cdot \beta_1
 * @f]
 *
 * @f[
 *      \beta^t_2 \leftarrow \beta^t_2 \cdot \beta_2
 * @f]
 *
 * This is not primary time critical function, because it only deals with scalars -> No special hardware implementation necessary (but possible).
 *
 * @param *self  The optimizer structure
 */
void aiopti_adam_f32_default_end_step(aiopti_t *self);

#endif // AIOPTI_ADAM_DEFAULT
