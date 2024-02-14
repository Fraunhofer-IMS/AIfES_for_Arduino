/**
 * \file cnn/default/ailayer/ailayer_reshape_default.h
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
 * \brief Default implementation of the \link ailayer_reshape.h Reshape layer \endlink
 *
 * Hardware independent implementations of the Reshape layer in \link aimath_f32.h F32 \endlink,
 *  \link aimath_q31.h Q31 \endlink and \link aimath_q7.h Q7 \endlink data-type.
 * For more information about the Reshape layer refer to ailayer_reshape.h.
 */

#ifndef AILAYER_RESHAPE_DEFAULT_H
#define AILAYER_RESHAPE_DEFAULT_H

#include "cnn/base/ailayer/ailayer_reshape.h"

#include "basic/default/aimath/aimath_f32_default.h"
#include "basic/default/aimath/aimath_q31_default.h"
#include "basic/default/aimath/aimath_q7_default.h"
#include "cnn/default/aimath/aimath_cnn_f32_default.h"

#define AILAYER_RESHAPE_F32_M(output_dim, infer_axis, output_shape)         {{0,},output_dim,infer_axis,output_shape}
#define AILAYER_RESHAPE_F32_A(output_dim, infer_axis, output_shape)         {{0,},output_dim,infer_axis,output_shape}
#define AILAYER_RESHAPE_Q31_M(output_dim, infer_axis, output_shape)         {{0,},output_dim,infer_axis,output_shape}
#define AILAYER_RESHAPE_Q31_A(output_dim, infer_axis, output_shape)         {{0,},output_dim,infer_axis,output_shape}
#define AILAYER_RESHAPE_Q7_M(output_dim, infer_axis, output_shape)          {{0,},output_dim,infer_axis,output_shape}
#define AILAYER_RESHAPE_Q7_A(output_dim, infer_axis, output_shape)          {{0,},output_dim,infer_axis,output_shape}

#define AILAYER_FLATTEN_F32_M()         {{{0,},},}
#define AILAYER_FLATTEN_F32_A()         {{{0,},},}
#define AILAYER_FLATTEN_Q31_M()         {{{0,},},}
#define AILAYER_FLATTEN_Q31_A()         {{{0,},},}
#define AILAYER_FLATTEN_Q7_M()          {{{0,},},}
#define AILAYER_FLATTEN_Q7_A()          {{{0,},},}

typedef struct ailayer_reshape   ailayer_reshape_f32_t;
typedef struct ailayer_reshape   ailayer_reshape_q31_t;
typedef struct ailayer_reshape   ailayer_reshape_q7_t;

typedef struct ailayer_flatten   ailayer_flatten_f32_t;
typedef struct ailayer_flatten   ailayer_flatten_q31_t;
typedef struct ailayer_flatten   ailayer_flatten_q7_t;

/// @brief Initializes and connect a \link ailayer_reshape.h Reshape layer \endlink with the \link aimath_f32.h F32 \endlink default implementation
///
/// **Example:** Create the layer structure:\n
/// In C:
/// \code{.c}
/// uint16_t output_shape[2] = {0, 1, 0}; // Axis 0 is the batch dimension and axis 2 is inferred
/// ailayer_reshape_f32_t reshape_layer = {
///     .output_dim = 3,
///     .infer_axis = 2,
///     .output_shape = output_shape
/// };
/// \endcode
/// In C, C++ and on Arduino:
/// \code{.c}
/// uint16_t output_shape[2] = {0, 1, 0}; // Axis 0 is the batch dimension and axis 2 is inferred
/// ailayer_reshape_f32_t reshape_layer = AILAYER_RESHAPE_F32_A(
///                                                                 /*output_dim =*/    3,
///                                                                 /*infer_axis =*/    2,
///                                                                 /*output_shape =*/  output_shape
///                                                             );
/// \endcode
///
/// **Example:** Initialize and connect the layer:\n
/// \code{.c}
/// x = ailayer_reshape_f32_default(&reshape_layer, x);
/// \endcode
///
/// @param *layer        The layer structure to initialize.
/// @param *input_layer  The prior layer.
/// @return              The (successfully) initialized layer structure.
///
ailayer_t *ailayer_reshape_f32_default(ailayer_reshape_f32_t *layer, ailayer_t *input_layer);

/// @brief Initializes and connect a \link ailayer_reshape.h Reshape layer \endlink with the \link aimath_q31.h Q31 \endlink default implementation
///
/// **Example:** Create the layer structure:\n
/// In C:
/// \code{.c}
/// uint16_t output_shape[2] = {0, 1, 0}; // Axis 0 is the batch dimension and axis 2 is inferred
/// ailayer_reshape_q31_t reshape_layer = {
///     .output_dim = 3,
///     .infer_axis = 2,
///     .output_shape = output_shape
/// };
/// \endcode
/// In C, C++ and on Arduino:
/// \code{.c}
/// uint16_t output_shape[2] = {0, 1, 0}; // Axis 0 is the batch dimension and axis 2 is inferred
/// ailayer_reshape_q31_t reshape_layer = AILAYER_RESHAPE_Q31_A(
///                                                                 /*output_dim =*/    3,
///                                                                 /*infer_axis =*/    2,
///                                                                 /*output_shape =*/  output_shape
///                                                             );
/// \endcode
///
/// **Example:** Initialize and connect the layer:\n
/// \code{.c}
/// x = ailayer_reshape_q31_default(&reshape_layer, x);
/// \endcode
///
/// @param *layer        The layer structure to initialize.
/// @param *input_layer  The prior layer.
/// @return              The (successfully) initialized layer structure.
///
ailayer_t *ailayer_reshape_q31_default(ailayer_reshape_q31_t *layer, ailayer_t *input_layer);

/// @brief Initializes and connect a \link ailayer_reshape.h Reshape layer \endlink with the \link aimath_q7.h Q7 \endlink default implementation
///
/// **Example:** Create the layer structure:\n
/// In C:
/// \code{.c}
/// uint16_t output_shape[2] = {0, 1, 0}; // Axis 0 is the batch dimension and axis 2 is inferred
/// ailayer_reshape_q7_t reshape_layer = {
///     .output_dim = 3,
///     .infer_axis = 2,
///     .output_shape = output_shape
/// };
/// \endcode
/// In C, C++ and on Arduino:
/// \code{.c}
/// uint16_t output_shape[2] = {0, 1, 0}; // Axis 0 is the batch dimension and axis 2 is inferred
/// ailayer_reshape_q7_t reshape_layer = AILAYER_RESHAPE_Q7_A(
///                                                             /*output_dim =*/    3,
///                                                             /*infer_axis =*/    2,
///                                                             /*output_shape =*/  output_shape
///                                                           );
/// \endcode
///
/// **Example:** Initialize and connect the layer:\n
/// \code{.c}
/// x = ailayer_reshape_q7_default(&reshape_layer, x);
/// \endcode
///
/// @param *layer        The layer structure to initialize.
/// @param *input_layer  The prior layer.
/// @return              The (successfully) initialized layer structure.
///
ailayer_t *ailayer_reshape_q7_default(ailayer_reshape_q7_t *layer, ailayer_t *input_layer);


/// @brief Initializes and connect a \link ailayer_reshape.h Flatten layer \endlink with the \link aimath_f32.h F32 \endlink default implementation
///
/// **Example:** Create the layer structure:\n
/// \code{.c}
/// ailayer_flatten_f32_t flatten_layer;
/// \endcode
/// or
/// \code{.c}
/// ailayer_flatten_f32_t flatten_layer = AILAYER_FLATTEN_F32_A();
/// \endcode
///
/// **Example:** Initialize and connect the layer:\n
/// \code{.c}
/// x = ailayer_flatten_f32_default(&flatten_layer, x);
/// \endcode
///
/// @param *layer        The layer structure to initialize.
/// @param *input_layer  The prior layer.
/// @return              The (successfully) initialized layer structure.
///
ailayer_t *ailayer_flatten_f32_default(ailayer_flatten_f32_t *layer, ailayer_t *input_layer);

/// @brief Initializes and connect a \link ailayer_reshape.h Flatten layer \endlink with the \link aimath_f32.h F32 \endlink default implementation
///
/// **Example:** Create the layer structure:\n
/// \code{.c}
/// ailayer_flatten_q31_t flatten_layer;
/// \endcode
/// or
/// \code{.c}
/// ailayer_flatten_q31_t flatten_layer = AILAYER_FLATTEN_Q31_A();
/// \endcode
///
/// **Example:** Initialize and connect the layer:\n
/// \code{.c}
/// x = ailayer_flatten_q31_default(&flatten_layer, x);
/// \endcode
///
/// @param *layer        The layer structure to initialize.
/// @param *input_layer  The prior layer.
/// @return              The (successfully) initialized layer structure.
///
ailayer_t *ailayer_flatten_q31_default(ailayer_flatten_q31_t *layer, ailayer_t *input_layer);

/// @brief Initializes and connect a \link ailayer_reshape.h Flatten layer \endlink with the \link aimath_f32.h F32 \endlink default implementation
///
/// **Example:** Create the layer structure:\n
/// \code{.c}
/// ailayer_flatten_q7_t flatten_layer;
/// \endcode
/// or
/// \code{.c}
/// ailayer_flatten_q7_t flatten_layer = AILAYER_FLATTEN_Q7_A();
/// \endcode
///
/// **Example:** Initialize and connect the layer:\n
/// \code{.c}
/// x = ailayer_flatten_q7_default(&flatten_layer, x);
/// \endcode
///
/// @param *layer        The layer structure to initialize.
/// @param *input_layer  The prior layer.
/// @return              The (successfully) initialized layer structure.
///
ailayer_t *ailayer_flatten_q7_default(ailayer_flatten_q7_t *layer, ailayer_t *input_layer);

#endif // AILAYER_RESHAPE_DEFAULT_H
