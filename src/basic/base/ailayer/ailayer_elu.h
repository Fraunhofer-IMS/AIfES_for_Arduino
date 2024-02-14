/**
 * \file basic/base/ailayer/ailayer_elu.h
 * \internal
 * \date 15.03.2021
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
 * \brief Base \link ailayer layer \endlink implementation of the ELU activation layer
 *
 * This is an "abstract" data-type independent implementation. To use the layer use one of the provided
 * implementations for a specific hardware and data-type (for example from ailayer_elu_default.h) or set
 * the required math functions on your own.
 *
 * \image html ailayer_elu_schematic.png width=200px
 *
 * The ELU layer is used as an activation function layer right after a dense layer. It calculates
  * @f[
  *  y = \begin{cases}
                    \alpha (e^x - 1) & \text{if } x \leq 0\\
                    x & \text{if } x > 0
                    \end{cases}
  * @f]
 * for every element of the input tensor.
 *
 * The results of the forward pass of this layer are written to the result tensor of the base ailayer_t struct.
 */

#ifndef ELU_LAYER
#define ELU_LAYER

#include "core/aifes_core.h"

typedef struct ailayer_elu 	ailayer_elu_t;

/** @brief General \link ailayer_elu.h ELU layer \endlink struct
*
*/
struct ailayer_elu {
    ailayer_t base; /**< Inherited field members from general ailayer struct. */

	/** @name Layer configuration
	 * @brief Required configuration parameters for the layer
	 *
	 * These fields have to be configured by the user before calling the initializer function.
	 */
	///@{
	void *alpha; /**< Parameter \f$ \alpha \f$ used to calculate ELU function for input values < 0. */
	const aimath_dtype_t *alpha_dtype; /**< Data type of scalar parameter \f$ \alpha \f$. */
	///@}

	/** @name Math functions
	 * @brief Required data type specific math functions
	 */
	///@{

    /** @brief Required math function: ELU
	 *
	 * Requires a math function that calculates the element wise ELU of a tensor:\n
     * @f[
     *  result_{i} = \begin{cases}
            \alpha \cdot (e^{x_i} - 1) & \text{if } x_i < 0 \\
            x_i & \text{if } x_i \geq 0
        \end{cases}
     * @f]
     *
     * @param x         N-dimensional tensor (input)
     * @param result    N-dimensional tensor (output)
	 */
	void (*elu)(const aitensor_t *x, const void *alpha, aitensor_t *result);

	/** @brief Required math function: Derivative of ELU
	 *
	 * Requires a math function that calculates the element wise ELU derivative of a tensor:\n
     * @f[
     *  result_{i} = \begin{cases}
                    \alpha \cdot e^{x_i} & \text{if } x_i < 0\\
                    1 & \text{if } x_i \geq 0
                    \end{cases}
     * @f]
     *
     * @param x         N-dimensional tensor (input)
     * @param result    N-dimensional tensor (output)
	 */
	void (*d_elu)(const aitensor_t *x, const void *alpha, aitensor_t *result);

	/** @brief Required math function: Element wise tensor multiplication
	 *
	 * Requires a math function that multiplies two tensors element wise:\n
     * @f[
     *  result = a \circ b
     * @f]
	 */
	void (*multiply)(const aitensor_t *a, const aitensor_t *b, aitensor_t *result);

	///@}
};

/** @brief ELU layer type
 *
 * Defines the type of the layer (for example for type checks and debug prints).
 * See aicore_layertype for more information about the layer type.
 */
extern const aicore_layertype_t *ailayer_elu_type;

/** @brief Initialize and connect the given ELU layer
 *
 * This function represents the "constructor" of the abstract ELU layer. It initializes the layer structure
 * and connects it to the previous layer.\n
 * This function is not intended to call it directly. Instead use one of the data type specific implementations
 * (like for example ailayer_elu_f32_default()).
 *
 * @param *layer        The layer to initialize.
 * @param *input_layer  The previous layer that provides the inputs to the layer.
 * @return  Pointer to the (successfully) initialized general layer structure (ailayer_elu.base)
 */
ailayer_t *ailayer_elu(ailayer_elu_t *layer, ailayer_t *input_layer);

/** @brief Calculate the forward pass for given ELU layer
 *
 * *Implementation of ailayer.forward.*
 *
 * It uses the result tensor of the previous layer as input and writes the result of the forward pass
 * to the result tensor (ailayer.result) of the given layer.
 *
 * Calculation of the forward pass result:
 * @f[
 *  x_{out} \leftarrow ELU(x_{in})
 * @f]
 *
 * \f$ x_{in} \f$:	 Result of the forward pass of the previous layer\n
 * \f$ x_{out} \f$:	 Result of the forward pass of this layer\n\n
 *
 * Used math functions:
 * * ailayer_elu.elu
 *
 * @param *self Layer to calculate the forward path for.
 */
void ailayer_elu_forward(ailayer_t *self);

/** @brief Calculate the backward pass for the given ELU layer
 *
 * *Implementation of ailayer.backward.*
 *
 * It uses the deltas tensor of the next layer as input and writes the result of the backward pass
 * to the deltas tensor (ailayer.deltas) of the given layer.
 *
 * Calculation of the errors for the previous layer:
 * @f[
 *  \delta_{in} \leftarrow \delta_{out} \circ ELU'(x_{in})
 * @f]
 *
 * \f$ x_{in} \f$:	 Result of the forward pass of the previous layer\n
 * \f$ \delta_{in} \f$:	 Result of the backward pass of this layer\n
 * \f$ \delta_{out} \f$:	 Result of the backward pass of the next layer\n\n
 *
 * Used math functions:
 * * ailayer_elu.elu
 * * ailayer_elu.d_elu
 * * ailayer_elu.multiply
 *
 * @param *self Layer to calculate the backward path for.
 */
void ailayer_elu_backward(ailayer_t *self);

/** @brief Calculate the shape of the result tensor
 *
 * *Implementation of ailayer.calc_result_shape.*
 *
 * As the result tensor shape is shared with the result tensor shape of the previous layer (no change in shape is needed),
 * this function returns without doing anything.
 *
 * @param *self Layer to calculate the resulting shape for.
 */
void ailayer_elu_calc_result_shape(ailayer_t *self);

#ifdef AIDEBUG_PRINT_MODULE_SPECS
/** @brief Print the layer specification
 *
 * @param *self     The layer to print the specification for
 */
void ailayer_elu_print_specs(const ailayer_t *self);
#endif // AIDEBUG_PRINT_MODULE_SPECS

#endif // ELU_LAYER
