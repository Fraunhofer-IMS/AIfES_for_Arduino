/**
 * \file ailayer/ailayer_input.h
 * \internal
 * \date 20.10.2020
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
 * \brief Base \link ailayer layer \endlink implementation of the Input layer
 *
 * This is an "abstract" data-type independent implementation. To use the layer use one of the provided
 * implementations for a specific hardware and data-type (for example from ailayer_input_default.h) or set
 * the required math functions on your own.
 *
 * The Input layer is the start layer for every AIfES 2 model.
 */

#ifndef INPUT_LAYER
#define INPUT_LAYER

#include "core/aifes_core.h"

typedef struct ailayer_input 	ailayer_input_t;

/** @brief General \link ailayer_input.h Input layer \endlink structure
*
*/
struct ailayer_input {
	ailayer_t base; /**< Inherited field members from general ailayer struct. */

    /** @name Layer configuration
	 * @brief Required configuration parameters for the layer
	 *
	 * These fields have to be configured by the user before calling the initializer function.
	 */
	///@{
	uint8_t input_dim; /**< Dimension of the input tensor. */
	uint16_t *input_shape; /**< Shape of the input tensor. */
	///@}
};

/** @brief Input layer type
 *
 * Defines the type of the layer (for example for type checks and debug prints).
 * See aicore_layertype for more information about the layer type.
 */
extern const aicore_layertype_t *ailayer_input_type;

/** @brief Initialize the given Input layer
 *
 * This function represents the "constructor" of the abstract Input layer. It initializes the layer structure
 * and connects it to the previous layer.\n
 * This function is not intended to call it directly. Instead use one of the data type specific implementations
 * (like for example ailayer_input_f32_default()).
 *
 * @param *layer        The layer to initialize.
 * @return  Pointer to the (successfully) initialized general layer structure (ailayer_input.base).
 */
ailayer_t *ailayer_input(ailayer_input_t *layer);

/** @brief Calculate the forward pass for given Input layer
 *
 * *Implementation of ailayer.forward.*
 *
 * The standard layer interface demands a forward pass function
 * for the net scheduler, but theres no sense in 'calculating'
 * an input layer, so this function returns without doing something.
 *
 * @param *self  Layer to 'calculate' the forward pass for.
 */
void ailayer_input_forward(ailayer_t *self);

/** @brief Calculate the backward pass for the given Input layer
 *
 * *Implementation of ailayer.backward.*
 *
 * The standard layer interface demands a backward pass function
 * for the net scheduler, but theres no sense in 'calculating'
 * an input layer, so this function returns without doing something.
 *
 * @param *self Layer to 'calculate' the backward pass for.
 */
void ailayer_input_backward(ailayer_t *self);

/** @brief Calculate the shape of the result tensor
 *
 * *Implementation of ailayer.calc_result_shape*
 *
 * As the result shape is given by the configured ailayer_input.input_shape parameter,
 * this function returns without doing anything.
 *
 * @param *self Layer to calculate the resulting shape for.
 */
void ailayer_input_calc_result_shape(ailayer_t *self);

#ifdef AIDEBUG_PRINT_MODULE_SPECS
/** @brief Print the layer specification
 *
 * @param *self     The layer to print the specification for
 */
void ailayer_input_print_specs(const ailayer_t *self);
#endif // AIDEBUG_PRINT_MODULE_SPECS

#endif // INPUT_LAYER
