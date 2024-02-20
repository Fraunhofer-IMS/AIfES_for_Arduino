/**
 * \file basic/base/ailayer/ailayer_dense.h
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
 * \brief Base \link ailayer layer \endlink implementation of the Dense layer
 *
 * This is an "abstract" data-type independent implementation. To use the layer use one of the provided
 * implementations for a specific hardware and data-type (for example from ailayer_dense_default.h) or set
 * the required math functions on your own.
 *
 * \image html ailayer_dense_schematic.png width=200px
 *
 * The Dense layer (or fully-connected layer) is the core layer of a FNN and calculates the weighted sums of its inputs
 * @f[
 *  y = x \cdot W + b
 * @f]
 * with the number of inputs \f$ K \f$, the number of neurons / outputs \f$ M \f$,
 * the input vektor \f$ x \in \mathbb{R}^{1 \times K} \f$, the output vektor \f$ y \in \mathbb{R}^{1 \times M} \f$,
 * the weights matrix \f$ W \in \mathbb{R}^{K \times M} \f$ and the bias \f$ b \in \mathbb{R}^{1 \times M} \f$.
 *
 * To increase the computational speed it is also possible to process a whole data batch at ones:
 * @f[
 *  Y = X \cdot W \oplus b
 * @f]
 * with the number of inputs \f$ K \f$, the number of neurons / outputs \f$ M \f$, the batch size \f$ N \f$,
 * the input matrix \f$ X \in \mathbb{R}^{N \times K} \f$, the output vektor \f$ Y \in \mathbb{R}^{N \times M} \f$,
 * the weights matrix \f$ W \in \mathbb{R}^{K \times M} \f$ and the bias \f$ b \in \mathbb{R}^{1 \times M} \f$.
 * In this case the \f$ \oplus \f$ means a broadcasted addition of the bias to the rows of the result of \f$ X \cdot W \f$:
 * @f[
 *  X \cdot W \oplus b \rightarrow X \cdot W + \left( \begin{array}{c} 1 \\ \vdots \\ 1 \\ \end{array}\right) \cdot b
 * @f]
 * Keep in mind that also the memory size for intermediate results increases with the batch size.
 *
 * The results of the forward pass of this layer are written to the result tensor of the base ailayer_t struct.
 */

#ifndef AILAYER_DENSE
#define AILAYER_DENSE

#include "core/aifes_core.h"
#include "basic/default/aimath/aimath_f32_default.h"
#include "basic/default/aimath/aimath_q7_default.h"
#include "basic/base/aimath/aimath_q31.h"

#define DENSE_WEIGHTS_SIZE(INPUTS, OUTPUTS)		((INPUTS) * (OUTPUTS))
#define DENSE_BIAS_SIZE(OUTPUTS)				(OUTPUTS)

#define DENSE_WEIGHTS_SHAPE(INPUTS, OUTPUTS)	{INPUTS, OUTPUTS}
#define DENSE_BIAS_SHAPE(OUTPUTS)				{1, OUTPUTS}

typedef struct ailayer_dense 	ailayer_dense_t;

/** @brief General \link ailayer_dense.h Dense layer \endlink structure
*
*/
struct ailayer_dense {
	ailayer_t base; /**< Inherited field members from general ailayer struct. */

    /** @name Layer configuration
	 * @brief Required configuration parameters for the layer
	 *
	 * These fields have to be configured by the user before calling the initializer function.
	 */
	///@{
	uint32_t neurons; /**< Layer neurons count (number of outputs). */
	///@}

	/** @name Trainable parameters
	 * @brief Data fields for the trainable parameters (weights, bias) of the layer
	 */
	///@{
	aitensor_t weights; /**< Tensor containing the layer weights. */
	aitensor_t bias; /**< Tensor containing the layer bias weights. */

	uint16_t weights_shape[2]; /**< Weights tensor shape (n x m matrix). */
	uint16_t bias_shape[1]; /**< Bias weights tensor shape (n x m matrix). */

	aitensor_t *trainable_params[2]; /**< Pointer to the weights and biases (which are the trainable parameters). */
	aitensor_t *gradients[2]; /**< Gradients structure for the back propagation algorithm. */
	void *optimem[2]; /**< Memory field used by the trainings optimizer. */
	///@}

    /** @name Math functions
	 * @brief Required data type specific math functions
	 */
	///@{

	/** @brief Required math function: Linear transformation
	 *
	 * Requires a math function that performs a linear transformation:\n
     * @f[
     *  result = a \cdot b \oplus c = a \cdot b + \left( \begin{array}{c} 1 \\ \vdots \\ 1 \\ \end{array}\right) \cdot c
     * @f]
     *
     * @param a         Matrix with dimension \f$ N \times K \f$ (input)
     * @param b         Matrix with dimension \f$ K \times M \f$ (input)
     * @param c         Laying vektor with dimension \f$ 1 \times M \f$ (input)
     * @param result    Matrix with dimension \f$ N \times M \f$ (output)
	 */
	void (*linear)(const aitensor_t *a, const aitensor_t *b, const aitensor_t *c, aitensor_t *result);

	/** @brief Required math function: Matrix multiplication with transposed a
	 *
	 * Requires a math function that performs a matrix multiplication on two 2D tensors:\n
     * @f[
     *  result = a^T \cdot b
     * @f]
     *
     * @param a         Matrix with dimension \f$ K \times N \f$ (input)
     * @param b         Matrix with dimension \f$ K \times M \f$ (input)
     * @param result    Matrix with dimension \f$ N \times M \f$ (output)
	 */
	void (*mat_mul_at)(const aitensor_t *a, const aitensor_t *b, aitensor_t *result);

	/** @brief Required math function: Matrix multiplication with transposed b
	 *
	 * Requires a math function that performs a matrix multiplication on two 2D tensors:\n
     * @f[
     *  result = a \cdot b^T
     * @f]
     *
     * @param a         Matrix with dimension \f$ N \times K \f$ (input)
     * @param b         Matrix with dimension \f$ M \times K \f$ (input)
     * @param result    Matrix with dimension \f$ N \times M \f$ (output)
	 */
	void (*mat_mul_bt)(const aitensor_t *a, const aitensor_t *b, aitensor_t *result);

	/** @brief Required math function: Element wise tensor addition
	 *
	 * Requires a math function that adds two tensors element-wise:
     * @f[
     *  result = a + b
     * @f]
	 */
	void (*tensor_add)(const aitensor_t *a, const aitensor_t *b, aitensor_t *result);

	/** @brief Required math function: Channel-wise sum
	 *
	 * Requires a math function that calculates the sum of all elements of each channel c. The result tensor is 1D.:
     * @f[
     *  result_c = \sum_i(a_{ci})
     * @f]
	 */
	void (*sum_channelwise)(const aitensor_t *a, int8_t channel_axis, aitensor_t *result);

	///@}

	uint16_t result_shape[2]; /**< Inference result tensor (ailayer.result) shape. */
};

/** @brief Dense layer type
 *
 * Defines the type of the layer (for example for type checks and debug prints).
 * See aicore_layertype for more information about the layer type.
 */
extern const aicore_layertype_t *ailayer_dense_type;

/** @brief Initialize and connect the given Dense layer
 *
 * This function represents the "constructor" of the abstract Dense layer. It initializes the layer structure
 * and connects it to the previous layer.\n
 * This function is not intended to call it directly. Instead use one of the data type specific implementations
 * (like for example ailayer_dense_f32_default()).
 *
 * @param *layer        The layer to initialize.
 * @param *input_layer  The previous layer that provides the inputs to the layer.
 * @return  Pointer to the (successfully) initialized general layer structure (ailayer_dense.base)
 */
ailayer_t *ailayer_dense(ailayer_dense_t *layer, ailayer_t *input_layer);

/** @brief Calculate the forward pass for given Dense layer
 *
 * *Implementation of ailayer.forward.*
 *
 * It uses the result tensor of the previous layer as input and writes the result of the forward pass
 * to the result tensor (ailayer.result) of the given layer.
 *
 * Calculation of the forward pass result:
 * @f[
 *  x_{out} \leftarrow x_{in} \cdot w \oplus b
 * @f]
 *
 * \f$ w \f$:	 Weights matrix\n
 * \f$ b \f$:	 Bias vector\n
 * \f$ x_{in} \f$:	 Result of the forward pass of the previous layer\n
 * \f$ x_{out} \f$:	 Result of the forward pass of this layer\n\n
 *
 * Used math functions:
 * * ailayer_dense.linear
 *
 * @param *self Layer to calculate the forward path for.
 */
void ailayer_dense_forward(ailayer_t *self);

/** @brief Calculate the backward pass for the given Dense layer
 *
 * *Implementation of ailayer.backward.*
 *
 * It uses the deltas tensor of the next layer as input and writes the result of the backward pass
 * to the deltas tensor (ailayer.deltas) of the given layer.
 *
 * Calculation of the gradients:
 * @f[
 *  \partial w \leftarrow \partial w + x_{in}^T \cdot \delta_{out}
 * @f]
 * @f[
 *  \partial b \leftarrow \partial b + \left( \begin{array}{c} 1 & \cdots & 1 \\ \end{array}\right) \cdot \delta_{out}
 * @f]
 *
 * Calculation of the errors for the previous layer:
 * @f[
 *  \delta_{in} \leftarrow w^T \cdot \delta_{out} = (\delta_{out}^T \cdot w)^T
 * @f]
 *
 * \f$ w \f$:	 Weights matrix\n
 * \f$ b \f$:	 Bias vektor\n
 * \f$ \partial w \f$:	 Gradients matrix for the weights\n
 * \f$ \partial b \f$:	 Gradients vektor for the bias\n
 * \f$ x_{in} \f$:	 Result of the forward pass of the previous layer\n
 * \f$ \delta_{in} \f$:	 Result of the backward pass of this layer\n
 * \f$ \delta_{out} \f$:	 Result of the backward pass of the next layer\n\n
 *
 * Used math functions:
 * * ailayer_dense.mat_mul_at
 * * ailayer_dense.mat_mul_bt
 * * ailayer_dense.tensor_add
 * * ailayer.dense.sum_channelwise
 *
 * @param *self Layer to calculate the backward path for.
 */
void ailayer_dense_backward(ailayer_t *self);

/** @brief Calculate the shape of the result tensor (ailayer.result)
 *
 * *Implementation of ailayer.calc_result_shape.*
 *
 * Resulting shape is [count_inputs x count_neurons]
 *
 * @param *self Layer to calculate the resulting shape for.
 */
void ailayer_dense_calc_result_shape(ailayer_t *self);

uint32_t ailayer_dense_sizeof_bwdmem(const ailayer_t *self);

/** @brief Calculate and return the parameter memory size needed for this layer
 *
 * *Implementation of ailayer.sizeof_paramem.*
 *
 * The parameter size is calculated for the \link ailayer_dense.weights weights \endlink and
 * \link ailayer_dense.bias bias \endlink tensors, including the data and tensor_params fields.
 *
 * @param *self The layer to calculate the parameter memory size for
 * @return  Calculated parameter memory size in bytes.
 */
uint32_t ailayer_dense_sizeof_paramem(const ailayer_t *self);

/** @brief Distribute provided memory to the parameter pointers
 *
 * *Implementation of ailayer.set_paramem.*
 *
 * Distributes the given buffer to the parameter pointers and sets
 * the tensor parameters for weights and bias holding structures.\n
 * The required parameter size can be calculated with ailayer_dense_sizeof_paramem()
 *
 * @param *self         The layer to set the memory fields for.
 * @param *memory_ptr   The memory that can be used for the parameters
 */
void ailayer_dense_set_paramem(ailayer_t *self, void *memory_ptr);

/** @brief Calculate and return the memory size needed by this layer for training
 *
 * *Implementation of ailayer.sizeof_trainmem.*
 *
 * The memory size is calculated for the gradient tensors of weights and bias.
 *
 * @param *self The layer to calculate the gradient memory size for.
 * @return  Calculated gradient memory size in bytes.
 */
uint32_t ailayer_dense_sizeof_trainmem(const ailayer_t *self);

/** @brief Distribute provided memory to the gradients pointers
 *
 * *Implementation of ailayer.set_trainmem.*
 *
 * The required memory size can be calculated with ailayer_dense_sizeof_trainmem().
 *
 * @param *self         The layer to set the memory fields for.
 * @param *memory_ptr   The memory that can be used for the gradients
 */
void ailayer_dense_set_trainmem(ailayer_t *self, void *memory_ptr);

#ifdef AIDEBUG_PRINT_MODULE_SPECS
/** @brief Print the layer specification
 *
 * @param *self     The layer to print the specification for
 */
void ailayer_dense_print_specs(const ailayer_t *self);
#endif // AIDEBUG_PRINT_MODULE_SPECS

#endif // AILAYER_DENSE
