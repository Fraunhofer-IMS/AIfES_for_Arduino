/**
 * \file cnn/base/ailayer/ailayer_maxpool2d.h
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
 * \brief Base \link ailayer layer \endlink implementation of the MaxPool2D layer
 *
 * This is an "abstract" data-type independent implementation. To use the layer use one of the provided
 * implementations for a specific hardware and data-type (for example from ailayer_maxpool2d_default.h) or set
 * the required math functions on your own.
 *
 * The MaxPool2D layer (or 2D Max-Pooling layer) is used to compress the size of activations,
 * by only taking the maximum activation values within a small window (pooling kernel).
 *
 * When the channel axis equals 1 (channels first), the shapes of input and output are
 * \f$ [N,C,H_{in},W_{in}] \f$ and \f$ [N,C,H_{out},W_{out}] \f$, respectively.
 *
 * When the channel axis equals -1 or 3 (channels last), the shapes of input and output are
 * \f$ [N,H_{in},W_{in},C] \f$ and \f$ [N,H_{out},W_{out},C] \f$, respectively.
 *
 * N is the batch size.
 *
 * The results of the forward pass of this layer are written to the result tensor of the base ailayer_t struct.
 */

#ifndef AILAYER_MAXPOOL2D_H
#define AILAYER_MAXPOOL2D_H

#include "core/aifes_core.h"

#define AIFES_CHANNELS_FIRST    1
#define AIFES_CHANNELS_LAST     -1

typedef struct ailayer_maxpool2d 	ailayer_maxpool2d_t;

/** @brief General \link ailayer_maxpool2d.h layer \endlink structure
*
*/
struct ailayer_maxpool2d {
	ailayer_t base; /**< Inherited field members from general ailayer struct. */

	/** @name Layer configuration
	 * @brief Required configuration parameters for the layer
	 *
	 * These fields have to be configured by the user before calling the initializer function.
	 */
	///@{
	// Values (configurable)
	uint16_t pool_size[2];    /**< Size of the pooling kernel (\f$ [H_{pool}, W_{pool}] \f$).*/
	uint16_t stride[2];  /**< The stride in the direction of height and width (\f$ [S_{h}, S_{w}] \f$). */
	uint16_t padding[2]; /**< The (symmetric) minus infinity padding in the direction of height and width (\f$ [P_{h}, P_{w}], \; 0 \leq P_h < H_{pool}, 0 \leq P_w < W_{pool}  \f$). */// Must be 0 >= padding < pool_size; -inf padding

	int8_t channel_axis; /**< Index of the channel axis (1 for channels first and -1 or 3 for channels last). */
	///@}

	/** @name Training parameters
	 * @brief Data fields required for the training of the layer
	 */
	///@{
	// Values (autogenerated)
	uint32_t *max_locations;    /**< Pointer to memory section where the indices of the maximum values per pooling window are stored. */

    void *optimem[2];   /**< Memory field used by the optimizer for training. */
	///@}

	/** @name Math functions
	 * @brief Required data type specific math functions
	 */
	///@{

	/** @brief Required math function: 2D Max-Pooling
	 *
	 * Requires a math function that extracts the maximum from a kernel window.
	 *
	 * For training (max_locations != 0), the index of the max-value in the kernel window must be stored in max_locations.\n
	 * An element of max_locations simply consist of the concatenated 16-bit indices for height and width in the pooling window.
     *
     * @param input             Input data with dimension \f$ [N,C,H_{in},W_{in}] \f$ (channels first) or \f$ [N,H_{in},W_{in},C] \f$ (channels last)
     * @param pool_size         The size of the pooling window (height and width)
     * @param stride            The stride in the direction of height and width.
     * @param padding           The (symmetric) minus infinity padding in the direction of height and width
     * @param channel_axis      Index of the channel axis (1 for channels first and -1 or 3 for channels last).
     * @param workspace         Pointer to a work space buffer for intermediate results.
     * @param max_locations     Pointer to memory section where the indices of the maximum values per pooling window are stored.
	 * @param output            Output after max-pooling with dimension \f$ [N,C,H_{out},W_{out}] \f$ (channels first) or \f$ [N,H_{out},W_{out},C] \f$ (channels last)
     */
	void (*maxpool2d_fwd)(
                          const aitensor_t *input,
                          const uint16_t pool_size[2],
                          const uint16_t stride[2],
                          const uint16_t padding[2],
                          int8_t channel_axis,
                          void *work_space,
                          uint32_t *max_locations,
                          aitensor_t *output
                          );

	/** @brief Required math function: Gradients of 2D Max-Pooling with respect to the input
	 *
	 * Requires a math function that calculates the gradients with respect to the input \f$ delta_{in} = \mathrm{d} L / \mathrm{d} x_{in} \f$.
	 *
	 * This is done by simply copying the output gradient to the position in the input gradients depicted by max_locations.\n
	 * An element of max_locations consist of the concatenated 16-bit indices for height and width in the pooling window.
	 *
     * @param delta_out         Gradients backpropagated from the following layer with dimension \f$ [N,C,H_{out},W_{out}] \f$ (channels first) or \f$ [N,H_{out},W_{out},C] \f$ (channels last)
     * @param pool_size         The size of the pooling window (height and width)
     * @param stride            The stride in the direction of height and width.
     * @param padding           The (symmetric) minus infinity padding in the direction of height and width
     * @param channel_axis      Index of the channel axis (1 for channels first and -1 or 3 for channels last).
     * @param workspace         Pointer to a work space buffer for intermediate results.
     * @param max_locations     Pointer to memory section where the indices of the maximum values per pooling window are stored.
	 * @param delta_in          Resulting input gradients for backpropagation to the previous layer \f$ [N,C,H_{in},W_{in}] \f$ (channels first) or \f$ [N,H_{in},W_{in},C] \f$ (channels last)
     */
    void (*maxpool2d_bwd)(
                          const aitensor_t *delta_out,
                          const uint16_t pool_size[2],
                          const uint16_t stride[2],
                          const uint16_t padding[2],
                          int8_t channel_axis,
                          void *work_space,
                          const uint32_t *max_locations,
                          aitensor_t *delta_in
                          );
    ///@}

	uint16_t result_shape[4]; /**< The shape of the result. */
};

/** @brief MaxPool2D layer type
 *
 * Defines the type of the layer (for example for type checks and debug prints).
 * See aicore_layertype for more information about the layer type.
 */
extern const aicore_layertype_t *ailayer_maxpool2d_type;

/** @brief Initialize and connect the given MaxPool2D layer
 *
 * This function represents the "constructor" of the abstract MaxPool2D layer. It initializes the layer structure
 * and connects it to the previous layer.\n
 * This function is not intended to call it directly. Instead use one of the data type specific implementations
 * (like for example ailayer_maxpool2d_f32_default()).
 *
 * @param *layer        The layer to initialize.
 * @param *input_layer  The previous layer that provides the inputs to the layer.
 * @return  Pointer to the (successfully) initialized general layer structure (ailayer_maxpool2d.base)
 */
ailayer_t *ailayer_maxpool2d(ailayer_maxpool2d_t *layer, ailayer_t *input_layer);

/** @brief Calculate the forward pass for given MaxPool2D layer
 *
 * *Implementation of ailayer.forward.*
 *
 * It uses the result tensor of the previous layer as input and writes the result of the forward pass
 * to the result tensor (ailayer.result) of the given layer.
 *
 * Used math functions:
 * * ailayer_maxpool2d.maxpool2d_fwd
 *
 * @param *self Layer to calculate the forward path for.
 */
void ailayer_maxpool2d_forward(ailayer_t *self);

/** @brief Calculate the backward pass for given MaxPool2D layer
 *
 * *Implementation of ailayer.backward.*
 *
 * It uses the deltas tensor of the next layer as input and writes the result of the backward pass
 * to the deltas tensor (ailayer.deltas) of the given layer.
 *
 * Used math functions:
 * * ailayer_maxpool2d.maxpool2d_bwd
 *
 * @param *self Layer to calculate the backward path for.
 */
void ailayer_maxpool2d_backward(ailayer_t *self);

/** @brief Calculate the shape of the result tensor (ailayer.result)
 *
 * *Implementation of ailayer.calc_result_shape.*
 *
 * Resulting shape is \f$ [N,C,H_{out},W_{out}] \f$ (channels first) or \f$ [N,H_{out},W_{out},C] \f$ (channels last) with\n
 *
 * @f[
 *  H_{out} = floor \left( \frac{H_{in} + 2 * P_h - H_{pool}}{S_h} \right) + 1
 * @f]
 * @f[
 *  W_{out} = floor \left( \frac{W_{in} + 2 * P_w - W_{pool}}{S_w} \right) + 1
 * @f]
 *
 * @param *self Layer to calculate the resulting shape for.
 */
void ailayer_maxpool2d_calc_result_shape(ailayer_t *self);

/** @brief Calculate and return the memory size needed by this layer for training
 *
 * *Implementation of ailayer.sizeof_trainmem.*
 *
 * The memory size is calculated for storage of the max_locations.
 *
 * @param *self The layer to calculate the gradient memory size for.
 * @return  Calculated gradient memory size in bytes.
 */
uint32_t ailayer_maxpool2d_sizeof_trainmem(const ailayer_t *self);

/** @brief Distribute provided memory to the gradients pointers
 *
 * *Implementation of ailayer.set_trainmem.*
 *
 * The required memory size can be calculated with ailayer_maxpool2d_sizeof_trainmem().
 *
 * @param *self         The layer to set the memory fields for.
 * @param *memory_ptr   The memory that can be used for the gradients
 */
void ailayer_maxpool2d_set_trainmem(ailayer_t *self, void *memory_ptr);

#ifdef AIDEBUG_PRINT_MODULE_SPECS
/** @brief Print the layer specification
 *
 * @param *self     The layer to print the specification for
 */
void ailayer_maxpool2d_print_specs(const ailayer_t *self);
#endif // AIDEBUG_PRINT_MODULE_SPECS

#endif // AILAYER_MAXPOOL2D_H
