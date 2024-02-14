/**
 * \file cnn/base/ailayer/ailayer_reshape.h
 * \version 2.2.0
 * \date 20.10.2020
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
 * \brief Base \link ailayer layer \endlink implementation of the Reshape layer
 *
 * This is an "abstract" data-type independent implementation. To use the layer use one of the provided
 * implementations for a specific hardware and data-type (for example from ailayer_reshape_default.h) or set
 * the required math functions on your own.
 *
 * The Reshape layer (also used as Flatten layer) is used to reshape a tensor during the forward pass
 * without changing any data. This can be useful for example to connect a \link ailayer_dense.h Dense layer \endlink
 * to a convolutional layer by using the Flatten layer.
 *
 * The results of the forward pass of this layer are written to the result tensor of the base ailayer_t struct.
 */

#ifndef AILAYER_RESHAPE_H
#define AILAYER_RESHAPE_H

#include "core/aifes_core.h"

typedef struct ailayer_reshape 	ailayer_reshape_t;
typedef struct ailayer_flatten 	ailayer_flatten_t;

/** @brief General \link ailayer_reshape.h layer \endlink structure
*
*/
struct ailayer_reshape {
	ailayer_t base; /**< Inherited field members from general ailayer struct. */

	/** @name Layer configuration
	 * @brief Required configuration parameters for the layer
	 *
	 * These fields have to be configured by the user before calling the initializer function.
	 */
	///@{
	uint8_t output_dim; /**< Output dimension count (length of output shape array). */
	uint8_t infer_axis; /**< Specifies the axis that is inferred from the elements of the tensor and remaining axes (Set to zero if not used). */
	uint16_t *output_shape; /**< Target shape for reshape operation. */
	///@}

	/** @name Math functions
	 * @brief Required data type specific math functions
	 */
	///@{

	/** @brief Optional math function: Reshape
	 *
	 * Optional math function that reshapes the tensor x (may be due to copying or just changing the shape).\n
	 * Set to zero if not required.
     *
     * @param x         N-dimensional tensor (input)
     * @param result    M-dimensional tensor (output)
	 */
	void (*reshape)(const aitensor_t *x, aitensor_t *result);

	///@}
};

/** @brief Structure of the Flatten layer, based on the \link ailayer_reshape.h reshape layer \endlink
*
* This layer is used to convert a ND tensor to a 2D tensor, for example as an input for a Dense layer.
*/
struct ailayer_flatten {
	ailayer_reshape_t base; /**< Inherited field members from reshape layer struct. */

	uint16_t output_shape[2]; /**< Target 2D shape for flatten operation. */
};

/** @brief Reshape layer type
 *
 * Defines the type of the layer (for example for type checks and debug prints).
 * See aicore_layertype for more information about the layer type.
 */
extern const aicore_layertype_t *ailayer_reshape_type;

/** @brief Initialize and connect the given Reshape layer
 *
 * This function represents the "constructor" of the abstract Reshape layer. It initializes the layer structure
 * and connects it to the previous layer.\n
 * This function is not intended to call it directly. Instead use one of the data type specific implementations
 * (like for example ailayer_reshape_f32_default()).
 *
 * @param *layer        The layer to initialize.
 * @param *input_layer  The previous layer that provides the inputs to the layer.
 * @return  Pointer to the (successfully) initialized general layer structure (ailayer_reshape.base)
 */
ailayer_t *ailayer_reshape(ailayer_reshape_t *layer, ailayer_t *input_layer);

/** @brief Calculate the forward pass for given Reshape layer
 *
 * *Implementation of ailayer.forward.*
 *
 * It uses the result tensor of the previous layer as input and writes the result of the forward pass
 * to the result tensor (ailayer.result) of the given layer.
 *
 * When no reshape function is given (ailayer_reshape.reshape = 0), this function does nothing (as the target shape
 * is already defined).
 *
 * Used math functions (optional; if set to zero, only the shape is changed):
 * * ailayer_reshape.reshape
 *
 * @param *self Layer to calculate the forward path for.
 */
void ailayer_reshape_forward(ailayer_t *self);

/** @brief Calculate the backward pass for given Reshape layer
 *
 * *Implementation of ailayer.backward.*
 *
 * It uses the deltas tensor of the next layer as input and writes the result of the backward pass
 * to the deltas tensor (ailayer.deltas) of the given layer.
 *
 * When no reshape function is given (ailayer_reshape.reshape = 0), this function does nothing (as the target shape
 * is already defined).
 *
 * Used math functions (optional; if set to zero, only the shape is changed):
 * * ailayer_reshape.reshape
 *
 * @param *self Layer to calculate the backward path for.
 */
void ailayer_reshape_backward(ailayer_t *self);

/** @brief Calculate the shape of the result tensor (ailayer.result)
 *
 * *Implementation of ailayer.calc_result_shape.*
 *
 * Resulting shape is given by ailayer_reshape.output_shape and optionally the inferred axis ailayer_reshape.infer_axis.
 *
 * @param *self Layer to calculate the resulting shape for.
 */
void ailayer_reshape_calc_result_shape(ailayer_t *self);

#ifdef AIDEBUG_PRINT_MODULE_SPECS
/** @brief Print the layer specification
 *
 * @param *self     The layer to print the specification for
 */
void ailayer_reshape_print_specs(const ailayer_t *self);
#endif // AIDEBUG_PRINT_MODULE_SPECS

#endif // AILAYER_RESHAPE_H
