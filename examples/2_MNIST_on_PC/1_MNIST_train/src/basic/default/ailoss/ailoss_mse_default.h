/**
 * \file basic/default/ailoss/ailoss_mse_default.h
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
 * \brief Default implementation of the \link ailoss_mse.h Mean Squared Error loss \endlink
 *
 * Hardware independent implementations of the Mean Squared Error loss in \link aimath_f32.h F32 \endlink data-type.
 * For more information about the Mean Squared Error loss refer to ailoss_mse.h.
 */

#ifndef AILOSS_MSE_DEFAULT
#define AILOSS_MSE_DEFAULT

#include "basic/base/ailoss/ailoss_mse.h"

#include "basic/default/aimath/aimath_f32_default.h"

typedef struct ailoss_mse 	ailoss_mse_f32_t;

/** @brief Initializes and connect a \link ailoss_mse.h Mean Squared Error loss \endlink with the \link aimath_f32.h F32 \endlink default implementation
 *
 * Example: Create the loss structure:\n
 * \code{.c}
 * ailoss_mse_f32_t mse_loss;
 * \endcode
 *
 * Example: Initialize and connect the loss to the layer structure:\n
 * \code{.c}
 * aimodel_t model;
 * ...
 * model.loss = ailoss_mse_f32_default(&mse_loss, model.output_layer);
 * \endcode
 *
 * @param *loss         The loss structure to initialize.
 * @param *input_layer  The output layer of the model.
 * @return              The (successfully) initialized loss structure.
 */
ailoss_t *ailoss_mse_f32_default(ailoss_mse_f32_t *loss, ailayer_t *input_layer);

#endif // AILOSS_MSE_DEFAULT
