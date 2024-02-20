/**
 * \file basic/default/aiopti/aiopti_sgd_default.h
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
 * \brief Default implementation of the \link aiopti_sgd.h Stochastic Gradient Descend optimizer \endlink
 *
 * Hardware independent implementations of the Stochastic Gradient Descend optimizer in \link aimath_f32.h F32 \endlink,
 * \link aimath_q31.h Q31 \endlink data-type.
 * For more information about the Stochastic Gradient Descend optimizer refer to aiopti_sgd.h.
 */

#ifndef AIOPTI_SGD_DEFAULT
#define AIOPTI_SGD_DEFAULT

#include "basic/base/aiopti/aiopti_sgd.h"

#include "basic/default/aimath/aimath_f32_default.h"
#include "basic/default/aimath/aimath_q31_default.h"
#include "basic/default/aimath/aimath_q7_default.h"

#define AIOPTI_SGD_F32(learning_rate) {{{0,},},learning_rate, 0.0f}
#define AIOPTI_SGD_WITH_MOMENTUM_F32(learning_rate, momentum) {{{0,},},learning_rate, momentum}
#define AIOPTI_SGD_Q31(learning_rate) {{{0,},},learning_rate, {0,0,0}}
#define AIOPTI_SGD_WITH_MOMENTUM_Q31(learning_rate, momentum) {{{0,},},learning_rate, momentum}

typedef struct aiopti_sgd_f32 	aiopti_sgd_f32_t; /**< New data type name for code reduction. */
typedef struct aiopti_sgd_q31 	aiopti_sgd_q31_t; /**< New data type name for code reduction. */
typedef struct aiopti_sgd_q7 	aiopti_sgd_q7_t; /**< New data type name for code reduction. */

/** @brief Data-type specific \link aiopti_sgd.h SGD optimizer \endlink struct for \link aimath_f32.h F32 \endlink
 *
 * Adds data fields for the learning rate and the momentum in \link aimath_f32.h F32 \endlink to the base implementation.
 */
struct aiopti_sgd_f32 {
	aiopti_sgd_t base; /**< Inherited field members from general aiopti_sgd struct. */

	aiscalar_f32_t learning_rate; /**< Storage for aiopti.learning_rate scalar in F32 */

	aiscalar_f32_t momentum; /**< Storage for aiopti_sgd.momentum scalar in F32 */
};

/** @brief Data-type specific \link aiopti_sgd.h SGD optimizer \endlink struct for \link aimath_q31.h Q31 \endlink
 *
 * Adds data fields for the learning rate and the momentum in \link aimath_q31.h Q31 \endlink to the base implementation.
 */
struct aiopti_sgd_q31 {
	aiopti_sgd_t base; /**< Inherited field members from general optimizer struct. */

	aiscalar_q31_t learning_rate; /**< Data-type specific learning rate */

	aiscalar_q31_t momentum; /**< Data-type specific momentum */
};

/** @brief Initializes a \link aiopti_sgd.h SGD optimizer \endlink with the \link aimath_f32.h F32 \endlink default implementation
 *
 * You can set the momentum to zero to save memory space.
 *
 * **Example:** Create the optimizer structure (with disabled momentum):\n
 * In C:
 * \code{.c}
 * aiopti_sgd_f32_t sgd_optimizer = {
 *     .learning_rate = 0.01f,
 *
 *     .momentum = 0.0f
 * };
 * \endcode
 * In C, C++ and on Arduino:
 * \code{.c}
 * aiopti_sgd_f32_t sgd_optimizer = AIOPTI_SGD_F32(0.01f);
 * \endcode
 *
 * **Example:** Create the optimizer structure (with enabled momentum):\n
 * In C:
 * \code{.c}
 * aiopti_sgd_f32_t sgd_optimizer = {
 *     .learning_rate = 0.01f,
 *
 *     .momentum = 0.9f
 * };
 * \endcode
 * In C, C++ and on Arduino:
 * \code{.c}
 * aiopti_sgd_f32_t sgd_optimizer = AIOPTI_SGD_WITH_MOMENTUM_F32(0.01f, 0.9f);
 * \endcode
 *
 * **Example:** Initialize the optimizer:\n
 * \code{.c}
 * aiopti_t *optimizer;
 *
 * optimizer = aiopti_sgd_f32_default(&sgd_optimizer);
 * \endcode
 *
 * @param *opti    The optimizer structure to initialize.
 * @return         The (successfully) initialized optimizer structure.
 */
aiopti_t *aiopti_sgd_f32_default(aiopti_sgd_f32_t *opti);

/** @brief Initializes a \link aiopti_sgd.h SGD optimizer \endlink with the \link aimath_q31.h Q31 \endlink default implementation
 *
 * You can set the momentum to zero to save memory space.
 *
 * **Example:** Create the optimizer structure (with disabled momentum):\n
 * In C:
 * \code{.c}
 * aiopti_sgd_q31_t sgd_optimizer = {
 *     .learning_rate = AISCALAR_Q31(0.01f, 28, 0),
 *
 *     .momentum = AISCALAR_Q31(0.0f, 0, 0)
 * };
 * \endcode
 * In C, C++ and on Arduino:
 * \code{.c}
 * aiopti_sgd_q31_t sgd_optimizer = AIOPTI_SGD_Q31(AISCALAR_Q31(0.01f, 28, 0));
 * \endcode
 *
 * **Example:** Create the optimizer structure (with enabled momentum):\n
 * In C:
 * \code{.c}
 * aiopti_sgd_q31_t sgd_optimizer = {
 *     .learning_rate = AISCALAR_Q31(0.01f, 28, 0),
 *
 *     .momentum = AISCALAR_Q31(0.9f, 28, 0)
 * };
 * \endcode
 * In C, C++ and on Arduino:
 * \code{.c}
 * aiopti_sgd_q31_t sgd_optimizer = AIOPTI_SGD_WITH_MOMENTUM_Q31(AISCALAR_Q31(0.01f, 28, 0), AISCALAR_Q31(0.9f, 28, 0));
 * \endcode
 *
 * **Example:** Initialize the optimizer:\n
 * \code{.c}
 * aiopti_t *optimizer;
 *
 * optimizer = aiopti_sgd_q31_default(&sgd_optimizer);
 * \endcode
 *
 * @param *opti    The optimizer structure to initialize.
 * @return         The (successfully) initialized optimizer structure.
 */
aiopti_t *aiopti_sgd_q31_default(aiopti_sgd_q31_t *opti);

#endif // AIOPTI_SGD_DEFAULT
