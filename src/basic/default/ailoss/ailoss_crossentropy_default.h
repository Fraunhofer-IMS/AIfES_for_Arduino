/**
 * \file basic/default/ailoss/ailoss_crossentropy_default.h
 * \internal
 * \date 14.01.2021
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
 * \brief Default implementation of the \link ailoss_crossentropy.h Cross-Entropy loss \endlink
 *
 * Hardware independent implementations of the Cross-Entropy loss in \link aimath_f32.h F32 \endlink data-type.
 * For more information about the Cross-Entropy loss refer to ailoss_mse.h.
 */

#ifndef AILOSS_CROSSENTROPY_DEFAULT
#define AILOSS_CROSSENTROPY_DEFAULT

#include "basic/base/ailoss/ailoss_crossentropy.h"

#include "basic/default/aimath/aimath_f32_default.h"

typedef struct ailoss_crossentropy 	ailoss_crossentropy_f32_t;

/** @brief Initializes and connect a \link ailoss_crossentropy.h Cross-Entropy loss \endlink with the \link aimath_f32.h F32 \endlink default implementation
 *
 * The labels must me either binary (when the output layer is a Sigmoid layer), for example
 * @f[
 *   \left( \begin{array}{ccc} 1 & 0 & 0 & 1 \\ 1 & 1 & 1 & 0 \\ 0 & 0 & 1 & 0 \end{array}\right)
 * @f]
 *
 * or row wise one-hot encoded (when the output layer is a Softmax layer), for example
 * @f[
 *   \left( \begin{array}{ccc} 0 & 0 & 0 & 1 \\ 1 & 0 & 0 & 0 \\ 0 & 0 & 1 & 0 \end{array}\right)
 * @f]
 *
 * If you want to provide labels as integers, please use ailoss_crossentropy_sparse8_f32_default() loss.
 *
 * Example: Create the loss structure:\n
 * \code{.c}
 * ailoss_crossentropy_f32_t crossentropy_loss;
 * \endcode
 *
 * Example: Initialize and connect the loss to the layer structure:\n
 * \code{.c}
 * aimodel_t model;
 * ...
 * model.output_layer = ailayer_sigmoid_f32_default(&sigmoid_layer, x);
 *
 * model.loss = ailoss_crossentropy_f32_default(&crossentropy_loss, model.output_layer);
 * \endcode
 *
 * @param *loss         The loss structure to initialize.
 * @param *input_layer  The output layer of the model (**Must be either a Sigmoid or a Softmax layer!**).
 * @return              The (successfully) initialized loss structure.
 */
ailoss_t *ailoss_crossentropy_f32_default(ailoss_crossentropy_f32_t *loss, ailayer_t *input_layer);

#endif // AILOSS_CROSSENTROPY_DEFAULT

