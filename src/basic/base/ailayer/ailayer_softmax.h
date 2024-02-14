
/**
 * \file basic/base/ailayer/ailayer_softmax.h
 * \internal
 * \date 07.12.2020
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
 * \brief Base \link ailayer layer \endlink implementation of the Softmax activation layer
 *
 * This is an "abstract" data-type independent implementation. To use the layer use one of the provided
 * implementations for a specific hardware and data-type (for example from ailayer_softmax_default.h) or set
 * the required math functions on your own.\n
 * <b>This layer is designed as an output-layer and has no backward implementation. If you want to train an ANN with this layer,
 * use the \link ailoss_crossentropy.h Cross-Entropy loss \endlink which has a combined Softmax-Cross-Entropy backward
 * function that is faster to compute!</b>
 *
 * \image html ailayer_softmax_schematic.png width=200px
 *
 * The Softmax layer is used as an activation function layer right after a dense layer. It calculates
 * @f[
 *  y_i = \frac{e^{x_i}}{\sum_{j=1}^{K} e^{x_j}}
 * @f]
 * for every element of the input tensor with \f$ K \f$ elements.
 *
 * The results of the forward pass of this layer are written to the result tensor of the base ailayer_t struct.
 */

#ifndef SOFTMAX_LAYER
#define SOFTMAX_LAYER

#include "core/aifes_core.h"

typedef struct ailayer_softmax 	ailayer_softmax_t;

/** @brief General \link ailayer_softmax.h Softmax layer \endlink struct
*
*/
struct ailayer_softmax {
	ailayer_t base; /**< Inherited field members from general ailayer struct. */

    /** @name Math functions
	 * @brief Required data type specific math functions
	 */
	///@{

	/** @brief Required math function: Softmax
	 *
	 * Requires a math function that calculates the element wise softmax of a tensor:\n
     * @f[
     *  result_{i} = \frac{e^{x_i}}{\sum_{j=1}^{K} e^{x_j}}
     * @f]
     *
     * @param x         N-dimensional tensor (input)
     * @param result    N-dimensional tensor (output)
	 */
	void (*softmax)(const aitensor_t *x, aitensor_t *result);

	///@}
};

/** @brief Softmax layer type
 *
 * Defines the type of the layer (for example for type checks and debug prints).
 * See aicore_layertype for more information about the layer type.
 */
extern const aicore_layertype_t *ailayer_softmax_type;

/** @brief Initialize and connect the given Softmax layer
 *
 * This function represents the "constructor" of the abstract Softmax layer. It initializes the layer structure
 * and connects it to the previous layer.\n
 * This function is not intended to call it directly. Instead use one of the data type specific implementations
 * (like for example ailayer_softmax_f32_default()).
 *
 * @param *layer        The layer to initialize.
 * @param *input_layer  The previous layer that provides the inputs to the layer.
 * @return  Pointer to the (successfully) initialized general layer structure (ailayer_softmax.base).
 */
ailayer_t *ailayer_softmax(ailayer_softmax_t *layer, ailayer_t *input_layer);

/** @brief Calculate the forward pass for given Softmax layer
 *
 * *Implementation of ailayer.forward.*
 *
 * It uses the result tensor of the previous layer as input and writes the result of the forward pass
 * to the result tensor (ailayer.result) of the given layer.
 *
 * Calculation of the forward pass result:
 * @f[
 *  x_{out} \leftarrow Softmax(x_{in})
 * @f]
 *
 * \f$ x_{in} \f$:	 Result of the forward pass of the previous layer\n
 * \f$ x_{out} \f$:	 Result of the forward pass of this layer\n\n
 *
 * Used math functions:
 * * ailayer_softmax.softmax
 *
 * @param *self Layer to calculate the forward path for.
 */
void ailayer_softmax_forward(ailayer_t *self);

/** @brief Calculate the shape of the result tensor
 *
 * *Implementation of ailayer.calc_result_shape.*
 *
 * As the result tensor shape is shared with the result tensor shape of the previous layer (no change in shape is needed),
 * this function returns without doing anything.
 *
 * @param *self Layer to calculate the resulting shape for.
 */
void ailayer_softmax_calc_result_shape(ailayer_t *self);

#ifdef AIDEBUG_PRINT_MODULE_SPECS
/** @brief Print the layer specification
 *
 * @param *self     The layer to print the specification for
 */
void ailayer_softmax_print_specs(const ailayer_t *self);
#endif // AIDEBUG_PRINT_MODULE_SPECS

#endif // SOFTMAX_LAYER
