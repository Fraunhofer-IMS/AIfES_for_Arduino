/**
 * \file cnn/default/ailayer/ailayer_maxpool2d_default.h
 * \version 2.2.0
 * \date 18.10.2021
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
 * \brief Default implementation of the \link ailayer_maxpool2d.h MaxPool2D layer \endlink
 *
 * Hardware independent implementations of the MaxPool2D layer in \link aimath_f32.h F32 \endlink data-type.
 * For more information about the MaxPool2D layer refer to ailayer_maxpool2d.h.
 */

#ifndef AILAYER_MAXPOOL2D_DEFAULT
#define AILAYER_MAXPOOL2D_DEFAULT

#include "cnn/base/ailayer/ailayer_maxpool2d.h"

#include "cnn/default/aimath/aimath_cnn_f32_default.h"
#include "basic/default/aimath/aimath_f32_default.h"

#define HW(h, w)        {h, w}

#define AILAYER_MAXPOOL2D_F32_M(pool_size, stride, padding) \
            {{0,},pool_size,stride,padding,}
#define AILAYER_MAXPOOL2D_F32_A(pool_size, stride, padding) \
            {{0,},pool_size,stride,padding,}

typedef struct ailayer_maxpool2d   ailayer_maxpool2d_f32_t;

/// @brief Initializes and connect a \link ailayer_conv2d.h Conv2D layer \endlink with the \link aimath_f32.h F32 \endlink default implementation
///
/// **Example:** Create the layer structure:\n
/// In C:
/// \code{.c}
/// ailayer_maxpool2d_f32_t maxpool2d_layer = {
///     .pool_size =    {2, 2},
///     .stride =       {2, 2},
///     .padding =      {0, 0}
/// };
/// \endcode
/// In C, C++ and on Arduino:
/// \code{.c}
/// ailayer_maxpool2d_t maxpool2d_layer = AILAYER_CONV2D_F32_A(
///                                                             /* pool_size =*/    HW(2, 2),
///                                                             /* stride =*/       HW(2, 2),
///                                                             /* padding =*/      HW(0, 0)
///                                                            );
/// \endcode
///
/// **Example:** Initialize and connect the layer for data with channels first (all options are equivalent):\n
/// \code{.c}
/// maxpool2d_layer.channel_axis = 1;
/// x = ailayer_maxpool2d_f32_default(&maxpool2d_layer, x);
/// \endcode
/// or
/// \code{.c}
/// x = ailayer_maxpool2d_cfirst_f32_default(&maxpool2d_layer, x);
/// \endcode
/// or
/// \code{.c}
/// x = ailayer_maxpool2d_chw_f32_default(&maxpool2d_layer, x);
/// \endcode
///
/// **Example:** Initialize and connect the layer for data with channels last (all options are equivalent):\n
/// \code{.c}
/// maxpool2d_layer.channel_axis = -1;
/// x = ailayer_maxpool2d_f32_default(&maxpool2d_layer, x);
/// \endcode
/// or
/// \code{.c}
/// x = ailayer_maxpool2d_clast_f32_default(&maxpool2d_layer, x);
/// \endcode
/// or
/// \code{.c}
/// x = ailayer_maxpool2d_hwc_f32_default(&maxpool2d_layer, x);
/// \endcode
///
/// @param *layer        The layer structure to initialize.
/// @param *input_layer  The prior layer.
/// @return              The (successfully) initialized layer structure.
///
ailayer_t *ailayer_maxpool2d_f32_default(ailayer_maxpool2d_f32_t *layer, ailayer_t *input_layer);

/** @brief Initializes and connect a \link ailayer_maxpool2d.h MaxPool2D layer \endlink (channels first) with the \link aimath_f32.h F32 \endlink default implementation
 *
 * Code examples are given in the description of ailayer_maxpool2d_f32_default().
 *
 * @param *layer        The layer structure to initialize.
 * @param *input_layer  The prior layer.
 * @return              The (successfully) initialized layer structure.
 */
ailayer_t *ailayer_maxpool2d_cfirst_f32_default(ailayer_maxpool2d_f32_t *layer, ailayer_t *input_layer);

/** @brief Initializes and connect a \link ailayer_maxpool2d.h MaxPool2D layer \endlink (channels first) with the \link aimath_f32.h F32 \endlink default implementation
 *
 * Code examples are given in the description of ailayer_maxpool2d_f32_default().
 *
 * @param *layer        The layer structure to initialize.
 * @param *input_layer  The prior layer.
 * @return              The (successfully) initialized layer structure.
 */
ailayer_t *ailayer_maxpool2d_chw_f32_default(ailayer_maxpool2d_f32_t *layer, ailayer_t *input_layer);

/** @brief Initializes and connect a \link ailayer_maxpool2d.h MaxPool2D layer \endlink (channels last) with the \link aimath_f32.h F32 \endlink default implementation
 *
 * Code examples are given in the description of ailayer_maxpool2d_f32_default().
 *
 * @param *layer        The layer structure to initialize.
 * @param *input_layer  The prior layer.
 * @return              The (successfully) initialized layer structure.
 */
ailayer_t *ailayer_maxpool2d_clast_f32_default(ailayer_maxpool2d_f32_t *layer, ailayer_t *input_layer);

/** @brief Initializes and connect a \link ailayer_maxpool2d.h MaxPool2D layer \endlink (channels last) with the \link aimath_f32.h F32 \endlink default implementation
 *
 * Code examples are given in the description of ailayer_maxpool2d_f32_default().
 *
 * @param *layer        The layer structure to initialize.
 * @param *input_layer  The prior layer.
 * @return              The (successfully) initialized layer structure.
 */
ailayer_t *ailayer_maxpool2d_hwc_f32_default(ailayer_maxpool2d_f32_t *layer, ailayer_t *input_layer);


#endif // AILAYER_CONV2D_DEFAULT

