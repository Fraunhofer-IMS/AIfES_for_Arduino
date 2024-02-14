/**
 * \file basic/base/ailayer/ailayer_template.h
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
 * \brief  [DEPRECATED] This file acts as a reference for new layer structures.
 *      Some of the shown functions are mandatory, some are optional.
 * \details
 */

#ifndef TEMPLATE_LAYER
#define TEMPLATE_LAYER

#include "core/aifes_core.h"

typedef struct ailayer_template 	ailayer_template_t;

/** @brief Template for general AIfES layers
*
*/
struct ailayer_template {
	ailayer_t base; /**< Inherited field members from general ailayer struct. */ // The base or super "class" of the layer.

	// Configurations of the layer, that can be configured by the user when creating the layer
	uint32_t example_configuration;

	// Tensor pointer for trainable parameters [DEPRECATED]
	aitensor_t *params;
	aitensor_t *d_params;        // Gradients

	// If the shape of the result differs from the input shape, this array can be used for the result.
	// If not, the result shape array of the input tensor can be used for the result tensor too.
	uint16_t result_shape[2]; /**< Inference result tensor shape (n x m matrix). */

	aitensor_t *trainable_params[2]; /**< Pointer to the weights and biases (which are the trainable parameters). */
	aitensor_t *gradients[2]; /**< Gradients structure for the back propagation algorithm. */
	void *optimem[2]; /**< Memory field used by the trainings optimizer. */

	// Make the math functions accessable for expert developers
	void (*copy_tensor)(const aitensor_t *from, aitensor_t *to); /**< Function pointer to the used tensor copy function. */
	void (*tensor_add)(const aitensor_t *a, const aitensor_t *b, aitensor_t *result); /**< Function pointer to the used tensor addition. */
};

extern const aicore_layertype_t *ailayer_template_type;
/** @brief Initialize the given layer struct - __mandatory__
 *
 * Initializes and sets all parameters needed by the layer structure.
 *
 * @param *layer    The layer to initialize.
 * @param *input_layer  The layer that provides the inputs to the initialized layer.
 * @return  Pointer to the (successfully) initialized layer structure.
 */

ailayer_t *ailayer_template(ailayer_template_t *layer, ailayer_t *input_layer);

/** @brief Calculate the forward path for given layer - __mandatory__
 *
 * @param *self  Layer to calculate the forward path for.
 */
void ailayer_template_forward(ailayer_t *self);

/** @brief Calculate the backward (training) path for the given layer - __depends on usability in training__
 *
 * @param *self Layer to calculate the backward path for.
 */
void ailayer_template_backward(ailayer_t *self);

/** @brief Calculate the shape of the result tensor
 *
 * @param *self Layer to calculate the resulting shape for.
 */
void ailayer_template_calc_result_shape(ailayer_t *self);

/** @brief Calculate and return the parameter memory size
 * needed for this layer - __mandatory for hidden layers__
 *
 * @param *self The layer to calculate the parameter memory size for
 * @return  Calculated parameter memory size in bytes.
 */
uint32_t ailayer_template_sizeof_paramem(const ailayer_t *self);

/** @brief Distribute provided memory to the parameter pointers - __mandatory for hidden layers__
 *
 * @param *self The layer to set the memory fields for.
 * @param *memory_ptr Pointer to the buffer provided for the layer parameters.
 */
void ailayer_template_set_paramem(ailayer_t *self, void *memory_ptr);

/** @brief Calculate and return the necessary memory size
 * needed by this layer for training - __mandatory for hidden layers__
 *
 * @param *self The layer to calculate the gradient memory size for.
 * @return  Calculated gradient memory size in bytes.
 */
uint32_t ailayer_template_sizeof_trainmem(const ailayer_t *self);

/** @brief Distribute provided memory to the training gradients pointers - __mandatory for hidden layers__
 *
 * @param *self The layer to set the memory fields for.
 * @param *memory_ptr Pointer to the buffer provided for the layer gradients.
 */
void ailayer_template_set_trainmem(ailayer_t *self, void *memory_ptr);

#ifdef AIDEBUG_PRINT_MODULE_SPECS
/** @brief Print the layer specification
 *
 * @param *self The layer to print the specification for
 */
void ailayer_template_print_specs(const ailayer_t *self);
#endif

#endif // TEMPLATE_LAYER
