/**
 * \file cnn/base/ailayer/ailayer_conv2d.h
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
 * \brief Base \link ailayer layer \endlink implementation of the Conv2D layer
 *
 * This is an "abstract" data-type independent implementation. To use the layer use one of the provided
 * implementations for a specific hardware and data-type (for example from ailayer_conv2d_default.h) or set
 * the required math functions on your own.
 *
 * The Conv2D layer (or 2D convolutional layer) is the core layer of a CNN and performs convolutions on 2-dimensional
 * data slices with trainable weights that can be seen as locally connected neurons with shared weights.
 * @f[
 *  Y = X * W + b
 * @f]
 *
 * When the channel axis equals 1 (channels first), the shapes of input, output and weights are
 * \f$ [N,C_{in},H_{in},W_{in}] \f$, \f$ [N,C_{out},H_{out},W_{out}] \f$ and \f$ [C_{out},C_{in},H_{kernel},W_{kernel}] \f$, respectively.
 *
 * When the channel axis equals -1 or 3 (channels last), the shapes of input, output and weights are
 * \f$ [N,H_{in},W_{in},C_{in}] \f$, \f$ [N,H_{out},W_{out},C_{out}] \f$ and \f$ [C_{out},H_{kernel},W_{kernel},C_{in}] \f$, respectively.
 *
 * N is the batch size.
 *
 * The results of the forward pass of this layer are written to the result tensor of the base ailayer_t struct.
 */

#ifndef CONV2D_LAYER
#define CONV2D_LAYER

#include "core/aifes_core.h"

#define AIFES_CHANNELS_FIRST    1
#define AIFES_CHANNELS_LAST     -1

#define AIFES_PADDING_SAME      0xFFFF  // Not fully implemented yet
#define AIFES_PADDING_VALID     0

typedef struct ailayer_conv2d 	ailayer_conv2d_t;

/** @brief General \link ailayer_conv2d.h Conv2D layer \endlink structure
*
*/
struct ailayer_conv2d {
	ailayer_t base; /**< Inherited field members from general ailayer struct. */

	/** @name Layer configuration
	 * @brief Required configuration parameters for the layer
	 *
	 * These fields have to be configured by the user before calling the initializer function.
	 */
	///@{
	// Values (configurable)
	uint32_t filter_count;  /**< Number of filters (\f$ C_{out} \f$). */
	uint16_t kernel_size[2];    /**< Size of the kernels (\f$ [H_{kernel}, W_{kernel}] \f$). */

	uint16_t stride[2];  /**< The stride in the direction of height and width (\f$ [S_{h}, S_{w}] \f$). */
	uint16_t dilation[2];  /**< The dilation in the direction of height and width (\f$ [D_{h}, D_{w}] \f$). */
	uint16_t padding[2]; /**< The (symmetric) zero padding in the direction of height and width (\f$ [P_{h}, P_{w}] \f$). */ // AIFES_PADDING_SAME = 0xFFFF

	int8_t channel_axis; /**< Index of the channel axis (1 for channels first and -1 or 3 for channels last). */
	///@}

	/** @name Trainable parameters
	 * @brief Data fields for the trainable parameters (weights, bias) of the layer
	 */
	///@{
	aitensor_t weights; /**< Tensor containing the weights. */
	aitensor_t bias;   /**< Tensor containing the bias weights. */

	uint16_t weights_shape[4]; /**< Weights tensor shape. */
	uint16_t bias_shape[1]; /**< Bias weights tensor shape. */

	aitensor_t *trainable_params[2]; /**< Pointer to the weights and bias (which are the trainable parameters). */
	aitensor_t *gradients[2]; /**< Gradients structure for the backpropagation algorithm. */
    void *optimem[2];   /**< Memory field used by the optimizer for training. */
	///@}

	uint16_t result_shape[4]; /**< The shape of the result. */

	/** @name Math functions
	 * @brief Required data type specific math functions
	 */
	///@{

	/** @brief Required math function: 2D-Convolution
	 *
	 * Requires a math function that performs a 2D-convolution and adds a channel-wise bias:\n
	 *
	 * @f[
     *  x_{out} = x_{in} \ast w + b
     * @f]
     *
     * @param input             Input data with dimension \f$ [N,C_{in},H_{in},W_{in}] \f$ (channels first) or \f$ [N,H_{in},W_{in},C_{in}] \f$ (channels last)
     * @param stride            The stride in the direction of height and width
     * @param dilation          The dilation in the direction of height and width
     * @param padding           The (symmetric) zero padding in the direction of height and width
     * @param weights           Convolution kernels with dimension \f$ [C_{out},C_{in},H_{kernel},W_{kernel}] \f$ (channels first) or \f$ [C_{out},H_{kernel},W_{kernel},C_{in}] \f$ (channels last)
     * @param bias              Bias with dimension \f$ C_{out} \f$
     * @param channel_axis      Index of the channel axis (1 for channels first and -1 or 3 for channels last).
     * @param work_space        Pointer to a work space buffer for intermediate results.
	 * @param output            Output after convolution with dimension \f$ [N,C_{out},H_{out},W_{out}] \f$ (channels first) or \f$ [N,H_{out},W_{out},C_{out}] \f$ (channels last)
     */
	void (*conv2d_fwd)(
                    const aitensor_t *input,
                    const uint16_t stride[2],    // [s_h, s_w]
                    const uint16_t dilation[2],  // [d_h, d_w]
                    const uint16_t padding[2],   // AIFES_PADDING_SAME not allowed
                    const aitensor_t *weights,
                    const aitensor_t *bias,
                    int8_t channel_axis,
                    void *work_space,
                    aitensor_t *output
                    );

	/** @brief Required math function: Gradients of 2D-Convolution with respect to the weights
	 *
	 * Requires a math function that calculates the gradients with respect to the weights \f$ \partial w = \mathrm{d} L / \mathrm{d} w \f$.
	 *
	 * @f[
     *  \partial w = x_{in} \ast delta_{out}
     * @f]
     *
     * @param input             Input data with dimension \f$ [N,C_{in},H_{in},W_{in}] \f$ (channels first) or \f$ [N,H_{in},W_{in},C_{in}] \f$ (channels last)
     * @param stride            The stride in the direction of height and width
     * @param dilation          The dilation in the direction of height and width
     * @param padding           The (symmetric) zero padding in the direction of height and width
     * @param output            Gradients backpropagated from the following layer with dimension \f$ [N,C_{out},H_{out},W_{out}] \f$ (channels first) or \f$ [N,H_{out},W_{out},C_{out}] \f$ (channels last)
     * @param channel_axis      Index of the channel axis (1 for channels first and -1 or 3 for channels last).
     * @param work_space        Pointer to a work space buffer for intermediate results.
	 * @param d_weights         Output gradients of the weights with dimension \f$ [C_{out},C_{in},H_{kernel},W_{kernel}] \f$ (channels first) or \f$ [C_{out},H_{kernel},W_{kernel},C_{in}] \f$ (channels last)
     */
    void (*conv2d_bwd)(
                    const aitensor_t *input,
                    const uint16_t stride[2],    // [s_h, s_w]
                    const uint16_t dilation[2],  // [d_h, d_w]
                    const uint16_t padding[2],
                    const aitensor_t *output,
                    int8_t channel_axis,
                    void *work_space,
                    aitensor_t *d_weights
    );

    /** @brief Required math function: Gradients of 2D-Convolution with respect to the input
	 *
	 * Requires a math function that calculates the gradients with respect to the input \f$ delta_{in} = \mathrm{d} L / \mathrm{d} x_{in} \f$.
	 *
	 * @f[
     *  delta_{in} = delta_{out} \ast' w
     * @f]
     *
     * \f$ \cdot \ast' \cdot \f$ is a transposed convolution.
     *
     * @param delta_out         Gradients backpropagated from the following layer with dimension \f$ [N,C_{in},H_{in},W_{in}] \f$ (channels first) or \f$ [N,H_{in},W_{in},C_{in}] \f$ (channels last)
     * @param stride            The stride in the direction of height and width
     * @param dilation          The dilation in the direction of height and width
     * @param padding           The (symmetric) zero padding in the direction of height and width
     * @param weights           Convolution kernels with dimension \f$ [C_{out},C_{in},H_{kernel},W_{kernel}] \f$ (channels first) or \f$ [C_{out},H_{kernel},W_{kernel},C_{in}] \f$ (channels last)
     * @param channel_axis      Index of the channel axis (1 for channels first and -1 or 3 for channels last).
     * @param work_space        Pointer to a work space buffer for intermediate results.
	 * @param delta_in          Resulting input gradients for backpropagation to the previous layer with dimension \f$ [N,C_{in},H_{in},W_{in}] \f$ (channels first) or \f$ [N,H_{in},W_{in},C_{in}] \f$ (channels last)
     */
    void (*conv2d_bwd_full)(
                    const aitensor_t *delta_out,
                    const uint16_t stride[2],    // [s_h, s_w]
                    const uint16_t dilation[2],  // [d_h, d_w]
                    const uint16_t padding[2],
                    const aitensor_t *weights,
                    int8_t channel_axis,
                    void *work_space,
                    aitensor_t *delta_in
    );

	/** @brief Required math function: Element wise tensor addition
	 *
	 * Requires a math function that adds two tensors element wise:
     * @f[
     *  result = a + b
     * @f]
	 */
	void (*tensor_add)(const aitensor_t *a, const aitensor_t *b, aitensor_t *result);

	/** @brief Required math function: Channel-wise sum
	 *
	 * Requires a math function that calculates the sum of all elements of each channel c. The result tensor is 1D.:
     * @f[
     *  result_c = \sum_i(x_{ci})
     * @f]
	 */
	void (*sum_channelwise)(const aitensor_t *x, int8_t channel_axis, aitensor_t *result);
};

/** @brief Conv2D layer type
 *
 * Defines the type of the layer (for example for type checks and debug prints).
 * See aicore_layertype for more information about the layer type.
 */
extern const aicore_layertype_t *ailayer_conv2d_type;

/** @brief Initialize and connect the given Conv2D layer
 *
 * This function represents the "constructor" of the abstract Conv2D layer. It initializes the layer structure
 * and connects it to the previous layer.\n
 * This function is not intended to call it directly. Instead use one of the data type specific implementations
 * (like for example ailayer_conv2d_f32_default()).
 *
 * @param *layer        The layer to initialize.
 * @param *input_layer  The previous layer that provides the inputs to the layer.
 * @return  Pointer to the (successfully) initialized general layer structure (ailayer_conv2d.base)
 */
ailayer_t *ailayer_conv2d(ailayer_conv2d_t *layer, ailayer_t *input_layer);

/** @brief Calculate the forward pass for given Conv2D layer
 *
 * *Implementation of ailayer.forward.*
 *
 * It uses the result tensor of the previous layer as input and writes the result of the forward pass
 * to the result tensor (ailayer.result) of the given layer.
 *
 * Calculation of the forward pass result:
 *
 * @f[
 *  x_{out} = x_{in} \ast w + b
 * @f]
 *
 * \f$ w \f$:	     Weights (convolution kernels)\n
 * \f$ b \f$:	     Bias vector\n
 * \f$ x_{in} \f$:	 Result of the forward pass of the previous layer\n
 * \f$ x_{out} \f$:	 Result of the forward pass of this layer\n\n
 *
 * Used math functions:
 * * ailayer_conv2d.conv2d_fwd
 *
 * @param *self Layer to calculate the forward path for.
 */
void ailayer_conv2d_forward(ailayer_t *self);

/** @brief Calculate the backward pass for given Conv2D layer
 *
 * *Implementation of ailayer.backward.*
 *
 * It uses the deltas tensor of the next layer as input and writes the result of the backward pass
 * to the deltas tensor (ailayer.deltas) of the given layer.
 *
 * Calculation of the backward pass result:
 * @f[
 *  \partial w \leftarrow w + x_{in} \ast delta_{out}
 * @f]
 * @f[
 *  \partial b \leftarrow b + \sum_i delta_{out;i}
 * @f]
 * @f[
 *  delta_{in} = delta_{out} \ast' w
 * @f]
 *
 * \f$ \cdot \ast' \cdot \f$ is a transposed convolution.\n
 * \f$ w \f$:	            Weights (convolution kernels)\n
 * \f$ b \f$:	            Bias vector\n
 * \f$ \partial w \f$:	    Gradients with respect to the weights\n
 * \f$ \partial b \f$:	    Gradients with respect to the bias\n
 * \f$ x_{in} \f$:	        Result of the forward pass of the previous layer\n
 * \f$ \delta_{in} \f$:	    Result of the backward pass of this layer\n
 * \f$ \delta_{out} \f$:	Result of the backward pass of the next layer\n\n
 *
 * Used math functions:
 * * ailayer_conv2d.conv2d_bwd
 * * ailayer_conv2d.conv2d_bwd_full
 *
 * @param *self Layer to calculate the backward path for.
 */
void ailayer_conv2d_backward(ailayer_t *self);

/** @brief Calculate the shape of the result tensor (ailayer.result)
 *
 * *Implementation of ailayer.calc_result_shape.*
 *
 * Resulting shape is \f$ [N,C_{out},H_{out},W_{out}] \f$ (channels first) or \f$ [N,H_{out},W_{out},C_{out}] \f$ (channels last) with\n
 *
 * @f[
 *  H_{out} = floor \left( \frac{H_{in} + 2 * P_h - D_h * (H_{kernel} - 1) - 1}{S_h} \right) + 1
 * @f]
 * @f[
 *  W_{out} = floor \left( \frac{W_{in} + 2 * P_w - D_w * (W_{kernel} - 1) - 1}{S_w} \right) + 1
 * @f]
 *
 * @param *self Layer to calculate the resulting shape for.
 */
void ailayer_conv2d_calc_result_shape(ailayer_t *self);

/** @brief Calculate and return the memory size needed for temporary results of the backward pass
 *
 * *Implementation of ailayer.sizeof_bwdmem.*
 *
 * Memory is required for temporary results of weights gradients and bias gradients.
 *
 * @param *self The layer to calculate the memory size for
 * @return  Calculated memory size in bytes.
 */
uint32_t ailayer_conv2d_sizeof_bwdmem(const ailayer_t *self);

/** @brief Calculate and return the parameter memory size needed for this layer
 *
 * *Implementation of ailayer.sizeof_paramem.*
 *
 * The parameter size is calculated for the \link ailayer_conv2d.weights weights \endlink and
 * \link ailayer_conv2d.bias bias \endlink tensors.
 *
 * @param *self The layer to calculate the parameter memory size for
 * @return  Calculated parameter memory size in bytes.
 */
uint32_t ailayer_conv2d_sizeof_paramem(const ailayer_t *self);

/** @brief Distribute provided memory to the parameter pointers
 *
 * *Implementation of ailayer.set_paramem.*
 *
 * Distributes the given buffer to the parameter pointers and sets
 * the tensor parameters for kernels and bias holding structures.\n
 * The required parameter size can be calculated with ailayer_conv2d_sizeof_paramem()
 *
 * @param *self         The layer to set the memory fields for.
 * @param *memory_ptr   The memory that can be used for the parameters
 */
void ailayer_conv2d_set_paramem(ailayer_t *self, void *memory_ptr);

/** @brief Calculate and return the memory size needed by this layer for training
 *
 * *Implementation of ailayer.sizeof_trainmem.*
 *
 * The memory size is calculated for the gradient tensors of weights and bias.
 *
 * @param *self The layer to calculate the gradient memory size for.
 * @return  Calculated gradient memory size in bytes.
 */
uint32_t ailayer_conv2d_sizeof_trainmem(const ailayer_t *self);

/** @brief Distribute provided memory to the gradients pointers
 *
 * *Implementation of ailayer.set_trainmem.*
 *
 * The required memory size can be calculated with ailayer_conv2d_sizeof_trainmem().
 *
 * @param *self         The layer to set the memory fields for.
 * @param *memory_ptr   The memory that can be used for the gradients
 */
void ailayer_conv2d_set_trainmem(ailayer_t *self, void *memory_ptr);

#ifdef AIDEBUG_PRINT_MODULE_SPECS
/** @brief Print the layer specification
 *
 * @param *self     The layer to print the specification for
 */
void ailayer_conv2d_print_specs(const ailayer_t *self);
#endif // AIDEBUG_PRINT_MODULE_SPECS

#endif // CONV2D_LAYER
