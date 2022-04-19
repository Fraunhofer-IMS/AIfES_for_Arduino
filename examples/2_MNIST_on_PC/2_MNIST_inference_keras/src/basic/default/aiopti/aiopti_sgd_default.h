/**
 * \file basic/default/aiopti/aiopti_sgd_default.h
 * \internal
 * \date 28.10.2020
 * \endinternal
 * \version 2.0alpha
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
 * \brief Default implementation of the \link aiopti_sgd.h Stochastic Gradient Descend optimizer \endlink
 *
 * Hardware independent implementations of the Stochastic Gradient Descend optimizer in \link aimath_f32.h F32 \endlink data-type.
 * For more information about the Stochastic Gradient Descend optimizer refer to aiopti_sgd.h.
 */

#ifndef AIOPTI_SGD_DEFAULT
#define AIOPTI_SGD_DEFAULT

#include "basic/base/aiopti/aiopti_sgd.h"

#include "basic/default/aimath/aimath_f32_default.h"

typedef struct aiopti_sgd_f32 	aiopti_sgd_f32_t; /**< New data type name for code reduction. */

/** @brief Data-type specific \link aiopti_sgd.h SGD optimizer \endlink struct for \link aimath_f32.h F32 \endlink
 *
 * Adds data fields for the learning rate and the momentum in \link aimath_f32.h F32 \endlink to the base implementation.
 */
struct aiopti_sgd_f32 {
	aiopti_sgd_t base; /**< Inherited field members from general aiopti_sgd struct. */

	aiscalar_f32_t learning_rate; /**< Storage for aiopti.learning_rate scalar in F32 */

	aiscalar_f32_t momentum; /**< Storage for aiopti_sgd.momentum scalar in F32 */
};

/** @brief Initializes a \link aiopti_sgd.h SGD optimizer \endlink with the \link aimath_f32.h F32 \endlink default implementation
 *
 * You can set the momentum to zero to save memory space.
 *
 * Example: Create the optimizer structure:\n
 * \code{.c}
 * aiopti_sgd_f32_t sgd_optimizer = {
 *     .learning_rate = 0.01f,
 *
 *     .momentum = 0.9f
 * };
 * \endcode
 *
 * Example: Initialize the optimizer:\n
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

#endif // AIOPTI_SGD_DEFAULT
