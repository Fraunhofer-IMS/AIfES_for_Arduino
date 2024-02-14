/**
 * \file basic/base/ailoss/ailoss_mse.h
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
 * \brief Base \link ailoss loss \endlink implementation of the Mean Squared Error (MSE) loss
 *
 * This is an "abstract" data-type independent implementation. To use the loss, use one of the provided
 * implementations for a specific hardware and data-type (for example from ailoss_mse_default.h) or set
 * the required math functions on your own.
 *
 * The Mean Squared Error (MSE) loss ist best suitable for regression tasks. The loss / cost is calculated as
 * @f[
 *  L(y, \hat{y}) = \sum_{i=0}^{N} (y_i - \hat{y}_i)^2
 * @f]
 * with the predicted values \f$ \hat{y}_i \f$ and the target values \f$ y_i \f$.
 * \f$ N \f$ is the number of elements of the \f$ y \f$ tensor.
 *
 * To get the "mean" normalization, you have to modify the learning rate to \f$ lr = \frac {1}{o \cdot n} \cdot lr \f$
 * with the number of outputs \f$ o \f$ and the batch size \f$ n \f$.
 *
 * The loss can be calculated with ailoss_mse_calc_loss(). For training the deltas /errors on the target data are
 * calculated with ailoss_mse_calc_delta() and written to the deltas tensor of the connection layer.
 */

#ifndef MSE_LOSS
#define MSE_LOSS

#include "core/aifes_core.h"

typedef struct ailoss_mse 	ailoss_mse_t; /**< New data type name for code reduction. */

/** @brief General \link ailoss_mse.h Mean Squared Error (MSE) loss \endlink struct
 *
 */
struct ailoss_mse {
	ailoss_t base; /**< Inherited field members from general ailoss struct. */
	const aimath_dtype_t *dtype; /**< Main data type of the loss. */

	/** @name Math fuctions
	 * @brief Required data type specific math functions
	 */
	///@{

	/** @brief Required math function: Gradient calculation of MSE loss function
	 *
	 * Requires a math function that calculates the gradient for each output with respect to the MSE loss:\n
	 *
	 */
	void (*mse_gradients)(const aitensor_t *predicted, const aitensor_t *target, aitensor_t *result);

	/** @brief Required math function: Calculation of the MSE loss function
	 *
	 * Requires a math function that calculates loss of the prediction compared to the target:\n
	 *
	 */
	void (*mse_loss)(const aitensor_t *predicted, const aitensor_t *target, void *result);

	///@}
};

/** @brief Mean Squared Error loss type
 *
 * Defines the type of the loss (for example for type checks and debug prints).
 * See aicore_losstype for more information about the loss type.
 */
extern const aicore_losstype_t *ailoss_mse_type;

/** @brief Initialize and connect the given MSE loss
 *
 * This function represents the "constructor" of the abstract MSE loss. It initializes the loss structure
 * and connects it to the output layer of the AIfES model.\n
 * This function is not intended to call it directly. Instead use one of the data type specific implementations
 * (like for example ailoss_mse_f32_default()).
 *
 * @param *loss         The loss to initialize.
 * @param *input_layer  The output layer of the model that provides the inputs to the loss.
 * @return  Pointer to the (successfully) initialized loss structure.
 */
ailoss_t *ailoss_mse(ailoss_mse_t *loss, ailayer_t *input_layer);

/** @brief Calculate the derivative of the given MSE loss for error backpropagation
 *
 * *Implementation of ailoss.calc_delta.*
 *
 * It uses the result tensor of the output layer and the target data as input and writes the result
 * to the deltas tensor (ailayer.deltas) of the connection layer (ailoss.connection_layer).
 *
 * Calculation of the deltas:
 * @f[
 *   \delta_{in} \leftarrow p - y
 * @f]
 *
 * \f$ \delta_{in} \f$:	 Result of the delta calculation of this loss (written to ailayer.deltas of the ailoss.connection_layer)\n
 * \f$ p \f$:	 Result of the forward pass of the output layer of the model (predicted values)\n
 * \f$ y \f$:	 Target data / True values / Labels\n\n
 *
 * Used math functions:
 * * ailoss_mse.tensor_sub
 *
 * @param *self         Loss to calculate the deltas for
 * @param *target_data  Target data / True values / Labels
 */
void ailoss_mse_calc_delta(ailoss_t *self, const aitensor_t *target_data);

/** @brief Calculate the MSE loss on the given target data
 *
 * *Implementation of ailoss.calc_loss.*
 *
 * It uses the result tensor of the output layer and the target data as input and writes the result
 * to the given result scalar.
 *
 * Calculation of the loss:
 * @f[
 *   result \leftarrow \sum_i (y_i - p_i)^2
 * @f]
 *
 * \f$ result \f$:	 Result of the loss calculation\n
 * \f$ p \f$:	 Result of the forward pass of the output layer of the model (predicted values)\n
 * \f$ y \f$:	 Target data / True values / Labels\n\n
 *
 * Used math functions:
 * * ailoss_mse.tensor_sub
 * * ailoss_mse.norm_squared
 *
 * @param *self         Loss to calculate the deltas for
 * @param *target_data  Target data / True values / Labels
 * @param *result       Result scalar (the data type is specified by the data type specific implementations)
 */
void ailoss_mse_calc_loss(ailoss_t *self, const aitensor_t *target_data, void *result);

#ifdef AIDEBUG_PRINT_MODULE_SPECS
/** @brief Print the loss specification
 *
 * @param *self The loss to print the specification for
 */
void ailoss_mse_print_specs(const ailoss_t *self);
#endif // AIDEBUG_PRINT_MODULE_SPECS

#endif // MSE_LOSS
