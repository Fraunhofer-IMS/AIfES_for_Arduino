/**
 * \file cnn/default/ailayer/ailayer_conv2d_default.h
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
 * \brief Default implementation of the \link ailayer_conv2d.h Conv2D layer \endlink
 *
 * Hardware independent implementations of the Conv2D layer in \link aimath_f32.h F32 \endlink data-type.
 * For more information about the Conv2D layer refer to ailayer_conv2d.h.
 */

#ifndef AILAYER_CONV2D_DEFAULT
#define AILAYER_CONV2D_DEFAULT

#include "cnn/base/ailayer/ailayer_conv2d.h"

#include "cnn/default/aimath/aimath_cnn_f32_default.h"
#include "basic/default/aimath/aimath_f32_default.h"

#define HW(h, w)        {h, w}

#define AILAYER_CONV2D_F32_M(filters, kernel_size, stride, dilation, padding, weights, bias) \
            {{0,},filters,kernel_size,stride,dilation,padding,0,{0,0,0,0,(float *) weights},{0,0,0,0,(float *) bias}}
#define AILAYER_CONV2D_F32_A(filters, kernel_size, stride, dilation, padding) \
            {{0,},filters,kernel_size,stride,dilation,padding,0,{0,0,0,0,0},{0,0,0,0,0}}

typedef struct ailayer_conv2d   ailayer_conv2d_f32_t;

/// @brief Initializes and connect a \link ailayer_conv2d.h Conv2D layer \endlink with the \link aimath_f32.h F32 \endlink default implementation
///
/// **Example:** Create the layer structure with pretrained weights:\n
/// In C:
/// \code{.c}
/// // Use constant data only for inference. For training remove the const qualifier!!
/// const float kernel_data_conv2d[] = {0.0f, -0.1f,
///                                     0.1f, 0.2f,
///
///                                     -0.2f, 0.2f,
///                                     -0.1f, 0.0f};
/// const float bias_data_conv2d[] = {0.0f, 0.0f};
/// ailayer_conv2d_f32_t conv2d_layer = {
///     .filter_count =     3,
///     .kernel_size =      {2, 2},
///     .stride =           {1, 1},
///     .dilation =         {1, 1},
///     .padding =          {0, 0},
///     .weights.data =     kernel_data_conv2d,
///     .bias.data =        bias_data_conv2d
/// };
/// \endcode
/// In C, C++ and on Arduino:
/// \code{.c}
/// // Use constant data only for inference. For training remove the const qualifier!!
/// const float kernel_data_conv2d[] = {0.0f, -0.1f,
///                                     0.1f, 0.2f,
///
///                                     -0.2f, 0.2f,
///                                     -0.1f, 0.0f};
/// const float bias_data_conv2d[] = {0.0f, 0.0f};
/// ailayer_conv2d_t conv2d_layer         = AILAYER_CONV2D_F32_M(
///                                                                 /* filters =*/     2,
///                                                                 /* kernel_size =*/ HW(2, 2),
///                                                                 /* stride =*/      HW(1, 1),
///                                                                 /* dilation =*/    HW(1, 1),
///                                                                 /* padding =*/     HW(0, 0),
///                                                                 /* weights =*/     kernel_data_conv2d,
///                                                                 /* bias =*/        bias_data_conv2d
///                                                              );
/// \endcode
///
/// **Example:** Create the layer structure for automatic parameter distribution:\n
/// In C:
/// \code{.c}
/// ailayer_conv2d_f32_t conv2d_layer = {
///     .filter_count =     3,
///     .kernel_size =      {2, 2},
///     .stride =           {1, 1},
///     .dilation =         {1, 1},
///     .padding =          {0, 0}
/// };
/// \endcode
/// In C, C++ and on Arduino:
/// \code{.c}
/// ailayer_conv2d_t conv2d_layer         = AILAYER_CONV2D_F32_A(
///                                                                 /* filters =*/     2,
///                                                                 /* kernel_size =*/ HW(2, 2),
///                                                                 /* stride =*/      HW(1, 1),
///                                                                 /* dilation =*/    HW(1, 1),
///                                                                 /* padding =*/     HW(0, 0)
///                                                              );
/// \endcode
///
/// **Example:** Initialize and connect the layer for data with channels first (all options are equivalent):\n
/// \code{.c}
/// conv2d_layer.channel_axis = 1;
/// x = ailayer_conv2d_f32_default(&conv2d_layer, x);
/// \endcode
/// or
/// \code{.c}
/// x = ailayer_conv2d_cfirst_f32_default(&conv2d_layer, x);
/// \endcode
/// or
/// \code{.c}
/// x = ailayer_conv2d_chw_f32_default(&conv2d_layer, x);
/// \endcode
///
/// **Example:** Initialize and connect the layer for data with channels last (all options are equivalent):\n
/// \code{.c}
/// conv2d_layer.channel_axis = -1;
/// x = ailayer_conv2d_f32_default(&conv2d_layer, x);
/// \endcode
/// or
/// \code{.c}
/// x = ailayer_conv2d_clast_f32_default(&conv2d_layer, x);
/// \endcode
/// or
/// \code{.c}
/// x = ailayer_conv2d_hwc_f32_default(&conv2d_layer, x);
/// \endcode
///
/// @param *layer        The layer structure to initialize.
/// @param *input_layer  The prior layer.
/// @return              The (successfully) initialized layer structure.
///
ailayer_t *ailayer_conv2d_f32_default(ailayer_conv2d_f32_t *layer, ailayer_t *input_layer);


/** @brief Initializes and connect a \link ailayer_conv2d.h Conv2D layer \endlink (channels first) with the \link aimath_f32.h F32 \endlink default implementation
 *
 * Code examples are given in the description of ailayer_conv2d_f32_default().
 *
 * @param *layer        The layer structure to initialize.
 * @param *input_layer  The prior layer.
 * @return              The (successfully) initialized layer structure.
 */
ailayer_t *ailayer_conv2d_cfirst_f32_default(ailayer_conv2d_f32_t *layer, ailayer_t *input_layer);

/** @brief Initializes and connect a \link ailayer_conv2d.h Conv2D layer \endlink (channels first) with the \link aimath_f32.h F32 \endlink default implementation
 *
 * Code examples are given in the description of ailayer_conv2d_f32_default().
 *
 * @param *layer        The layer structure to initialize.
 * @param *input_layer  The prior layer.
 * @return              The (successfully) initialized layer structure.
 */
ailayer_t *ailayer_conv2d_chw_f32_default(ailayer_conv2d_f32_t *layer, ailayer_t *input_layer);


/** @brief Initializes and connect a \link ailayer_conv2d.h Conv2D layer \endlink (channels last) with the \link aimath_f32.h F32 \endlink default implementation
 *
 * Code examples are given in the description of ailayer_conv2d_f32_default().
 *
 * @param *layer        The layer structure to initialize.
 * @param *input_layer  The prior layer.
 * @return              The (successfully) initialized layer structure.
 */
ailayer_t *ailayer_conv2d_clast_f32_default(ailayer_conv2d_f32_t *layer, ailayer_t *input_layer);

/** @brief Initializes and connect a \link ailayer_conv2d.h Conv2D layer \endlink (channels last) with the \link aimath_f32.h F32 \endlink default implementation
 *
 * Code examples are given in the description of ailayer_conv2d_f32_default().
 *
 * @param *layer        The layer structure to initialize.
 * @param *input_layer  The prior layer.
 * @return              The (successfully) initialized layer structure.
 */
ailayer_t *ailayer_conv2d_hwc_f32_default(ailayer_conv2d_f32_t *layer, ailayer_t *input_layer);

/** @brief \link aimath_f32.h F32 \endlink default implementation of the ailayer.init_params function for the Conv2D layer
 *
 * *Implementation of ailayer.init_params.*
 *
 * The function will initialize the weights and bias depending on the following activation function.
 *
 * | Activation function				| Weights-init	| Bias-init  |
 * |------------------------------------|---------------|------------|
 * | None, tanh, logistic, softmax	    | Glorot		| Zeros      |
 * | ReLu and variants				    | He			| Zeros      |
 *
 * @param *self  The layer structure
 */
void ailayer_conv2d_init_params_f32_default(ailayer_t *self);

#endif // AILAYER_CONV2D_DEFAULT


