/**
 * \file basic/base/ailoss/ailoss_crossentropy.h
 * \internal
 * \date 12.01.2021
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
 * \brief Base \link ailoss loss \endlink implementation of the Cross-Entropy loss
 *
 * This is an "abstract" data-type independent implementation. To use the loss, use one of the provided
 * implementations for a specific hardware and data-type (for example from ailoss_crossentropy_default.h) or set
 * the required math functions on your own.
 *
 * The Cross-Entropy loss ist best suitable for classification tasks
 *  and <b>works only with \link ailayer_sigmoid.h Sigmoid \endlink or \link ailayer_softmax.h Softmax \endlink output layers</b>.
 *
 * For **binary classification** (binary targets 0 or 1) with **Sigmoid output layer**, the loss / cost is calculated as
 * @f[
 *  L(y, \hat{y}) = - \sum_{i=0}^{N} (y_i \log(\hat{y}_i) + (1-y) \log(1-\hat{y}_i))
 * @f]
 * with the predicted values \f$ \hat{y}_i \f$ and the target values \f$ y_i \f$.
 * \f$ N \f$ is the number of elements of the \f$ y \f$ tensor.
 *
 * For **categorigal classification** (one-hot encoded targets) with **Softmax output layer**, the loss / cost is calculated as
 * @f[
 *  L(y, \hat{y}) = - \sum_{i=0}^{N} y_{i} \log(\hat{y}_{i})
 * @f]
 * with the predicted values \f$ \hat{y}_i \f$ and the target values \f$ y_i \f$.
 * \f$ N \f$ is the number of elements of the \f$ y \f$ tensor.
 *
 * To get the "mean" normalization, you have to modify the learning rate to \f$ lr = \frac {1}{o \cdot n} \cdot lr \f$
 * with the number of outputs \f$ o \f$ and the batch size \f$ n \f$.
 *
 * The loss can be calculated with ailoss_crossentropy_calc_loss(). For training the deltas /errors on the target data are
 * calculated with ailoss_crossentropy_calc_delta() and written to the deltas tensor of the connection layer.
 */

#ifndef CROSSENTROPY_LOSS
#define CROSSENTROPY_LOSS

#include "core/aifes_core.h"
#include "basic/base/ailayer/ailayer_softmax.h"
#include "basic/base/ailayer/ailayer_sigmoid.h"

typedef struct ailoss_crossentropy 	ailoss_crossentropy_t; /**< New data type name for code reduction. */

/** @brief General \link ailoss_crossentropy.h Cross-Entropy loss \endlink struct
*
*/
struct ailoss_crossentropy {
	ailoss_t base; /**< Inherited field members from general ailoss struct. */
	const aimath_dtype_t *dtype; /**< Main data type of the loss. */

	/** @name Math functions
	 * @brief Required data type specific math functions
	 */
	///@{

	/** @brief Required math function: Element wise tensor subtraction
	 *
	 * Requires a math function that subtracts two tensors element wise:\n
     * @f[
     *  result = a - b
     * @f]
	 */
	void (*tensor_sub)(const aitensor_t *a, const aitensor_t *b, aitensor_t *result);

	/** @brief Required math function: Cross-Entropy between two tensors
	 *
	 * Requires a math function that calculates the Cross-Entropy between two tensors.\n
	 * For a Sigmoid output layer it should be the binary Cross-Entropy:
     * @f[
     *  result =  -\sum (target \cdot \log(predicted) + (1 - target) \log(1 - predicted))
     * @f]
     *
	 * For a Softmax output layer it should be the categorical Cross-Entropy:
     * @f[
     *  result =  -\sum target \cdot \log(predicted)
     * @f]
     *
     * @param result    A Scalar of the defined data type
	 */
	void (*crossentropy)(const aitensor_t *predicted_data, const aitensor_t *target_data, void *result);

	/** @brief Required math function: Scaling of the gradient tensor
	 *
	 * Requires a math function that scales a tensor by its batch size:\n
	 *
	 */
	void (*scale_by_batch_size)(const aitensor_t *predicted_data, aitensor_t *result);

	///@}
};

/** @brief Cross-Entropy loss type
 *
 * Defines the type of the loss (for example for type checks and debug prints).
 * See aicore_losstype for more information about the loss type.
 */
extern const aicore_losstype_t *ailoss_crossentropy_type;

/** @brief Initialize and connect the given Cross-Entropy loss
 *
 * This function represents the "constructor" of the abstract Cross-Entropy loss. It initializes the loss structure
 * and connects it to the output layer of the AIfES model.\n
 * This function is not intended to call it directly. Instead use one of the data type specific implementations
 * (like for example ailoss_crossentropy_f32_default()).
 *
 * @param *loss         The loss to initialize.
 * @param *input_layer  The output layer of the model that provides the inputs to the loss.
 * @return  Pointer to the (successfully) initialized loss structure.
 */
ailoss_t *ailoss_crossentropy(ailoss_crossentropy_t *loss, ailayer_t *input_layer);

/** @brief Calculate the combined derivative of the given Cross-Entropy loss and the output layer for error backpropagation
 *
 * *Implementation of ailoss.calc_delta.*
 *
 * It uses the result tensor of the output layer and the target data as input and writes the result
 * to the deltas tensor (ailayer.deltas) of the output layer of the model.
 *
 * By combining the Cross-Entropy loss with a Sigmoid or Softmax output layer, the combined deltas can be
 * calculated very efficiently. The backward function of the Sigmoid / Softmax layer is not used anymore.
 *
 * Calculation of the deltas:
 * @f[
 *   \delta_{in} \leftarrow p - y
 * @f]
 *
 * \f$ \delta_{in} \f$:	 Result of the delta calculation of this loss (written to ailayer.deltas of the output layer of the model)\n
 * \f$ p \f$:	 Result of the forward pass of the output layer of the model (predicted values)\n
 * \f$ y \f$:	 Target data / True values / Labels\n\n
 *
 * Used math functions:
 * * ailoss_crossentropy.tensor_sub
 *
 * @param *self         Loss to calculate the deltas for
 * @param *target_data  Target data / True values / Labels
 */
void ailoss_crossentropy_calc_delta(ailoss_t *self, const aitensor_t *target_data);

/** @brief Calculate the Cross-Entropy loss on the given target data
 *
 * *Implementation of ailoss.calc_loss.*
 *
 * It uses the result tensor of the output layer and the target data as input and writes the result
 * to the given result scalar.
 *
 * Calculation of the loss with Sigmoid output layer:
 * @f[
 *   result \leftarrow - \sum_i (y_i \log(p_i) + (1 - y_i) \log(1 - p_i))
 * @f]
 *
 * Calculation of the loss with Softmax output layer:
 * @f[
 *   result \leftarrow - \sum_i y_i \log(p_i)
 * @f]
 *
 *
 * \f$ result \f$:	 Result of the loss calculation\n
 * \f$ p \f$:	 Result of the forward pass of the output layer of the model (predicted values)\n
 * \f$ y \f$:	 Target data / True values / Labels\n\n
 *
 * Used math functions:
 * * ailoss_crossentropy.tensor_sub
 * * ailoss_crossentropy.crossentropy
 *
 * @param *self         Loss to calculate the deltas for
 * @param *target_data  Target data / True values / Labels
 * @param *result       Result scalar (the data type is specified by the data type specific implementations)
 */
void ailoss_crossentropy_calc_loss(ailoss_t *self, const aitensor_t *target_data, void *result);

/** @brief Dummy backward-function for the output layer of the model
 *
 * *Implementation of ailayer.backward.*
*
* The ailoss_crossentropy_calc_delta() function calculates the combined delta for a Sigmoid or Softmax layer
* with Cross-Entropy loss. Therefor the backward function of the Sigmoid / Softmax layer is not needed anymore
* and gets exchanged by this dummy function.
*
* The dummy function does nothing.
*
* @param *self The output layer of the model
*/
void ailoss_crossentropy_dummy_backward(ailayer_t *self);

#ifdef AIDEBUG_PRINT_MODULE_SPECS
/** @brief Print the loss specification
 *
 * @param *self The loss to print the specification for
 */
void ailoss_crossentropy_print_specs(const ailoss_t *self);
#endif // AIDEBUG_PRINT_MODULE_SPECS

#endif // CROSSENTROPY_LOSS
