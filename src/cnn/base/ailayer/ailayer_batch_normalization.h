/**
 * \file cnn/base/ailayer/ailayer_batch_normalization.h
 * \internal
 * \date 17.01.2022
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
 * \brief Base \link ailayer layer \endlink implementation of the Batch Normalization layer.
 *
 * This is an "abstract" data-type independent implementation. To use the layer use one of the provided
 * implementations for a specific hardware and data-type (for example from ailayer_batch_normalization_default.h) or set
 * the required math functions on your own.
 *
 * The [Batch Normalization layer](https://arxiv.org/abs/1502.03167) can increase the training speed in deep neural networks
 * by normalizing intermediate activations. For every element \f$ j \f$ neuron / channel \f$ i \f$ in the batch,
 * the transformation is defined as
 * @f[
 *  y_{i,j} = \mathit{BN}(x_{i,j}) = \gamma_i \cdot \frac{x_{i,j} - \mu_{i}}{\sqrt{\sigma_{i}^2+\epsilon}} + \beta_i
 * @f]
 * @f[
 *  \mu_i = \frac{1}{m} \sum_{j=1}^{m} x_{i,j}
 * @f]
 * @f[
 *  \sigma_i^2 = \frac{1}{m} \sum_{j=1}^{m} (x_{i,j} - \mu_i)^2
 * @f]
 * \f$ \beta_i \f$ and \f$ \gamma_i \f$ are trainable parameters of the layer.
 *
 * Batch Normalization behaves different during training and during inference.
 *
 * When in training mode (```ailayer.settings[AILAYER_SETTINGS_TRAINING_MODE] = TRUE```), the means and variances
 * (\f$ \mu_i \f$ and \f$ \sigma_i^2 \f$) are calculated
 * for the whole batch during forward pass. Additionally, exponential moving averages
 * of \f$ \mu_i \f$ and \f$ \sigma_i^2 \f$ are calculated to estimate these values for the inference mode.
 *
 * In inference mode (```ailayer.settings[AILAYER_SETTINGS_TRAINING_MODE] = FALSE```), \f$ \mu_i \f$ and \f$ \sigma_i^2 \f$
 * are taken as fixed parameters from the averages, which were collected during training.
 *
 * Batch Normalization works best if a whole batch is processed at once in a forward pass, i.e. the first dimension of the input shape
 * of the model equals the batch size. If this is the case, the layer will run in batch mode
 * (```ailayer.settings[AILAYER_SETTINGS_BATCH_MODE] = TRUE```) and calculates the means and variances for the batch as described above.
 * Otherwise ( if the first input shape dimension is smaller than the batch size and therefore ```ailayer.settings[AILAYER_SETTINGS_BATCH_MODE] = FALSE```),
 * the layer uses the exponential moving averages also for
 * \f$ \mu_i \f$ and \f$ \sigma_i^2 \f$ during training. This reduces the memory required for intermediate activations,
 * but it may decrease the training speed.
 *
 * The results of the forward pass of this layer are written to the result tensor of the base ailayer_t struct.
 */

#ifndef AILAYER_BATCH_NORM
#define AILAYER_BATCH_NORM

#define AIFES_CHANNELS_FIRST    1
#define AIFES_CHANNELS_LAST     -1

#include "core/aifes_core.h"
#include "basic/default/aimath/aimath_f32_default.h"

typedef struct ailayer_batch_norm 	ailayer_batch_norm_t;

/** @brief General \link ailayer_batch_normalization.h Batch Normalization layer \endlink structure
*
*/
struct ailayer_batch_norm {
	ailayer_t base; /**< Inherited field members from general ailayer struct. */

    /** @name Layer configuration
	 * @brief Required configuration parameters for the layer
	 *
	 * These fields have to be configured by the user before calling the initializer function.
	 */
	///@{
	int8_t channel_axis; /**< Index of the channel axis (1 for channels first, -1 for channels last) */
	void *momentum; /**< Momentum for the exponential moving average of means and variances. */
    void *eps; /**< Small constant for numeric stability. */
	///@}

	/** @name Trainable parameters
	 * @brief Data fields for the trainable parameters (beta, gamma) of the layer
	 */
	///@{
	aitensor_t betas; /**< Vector of the shift parameters (\f$ \beta_i \f$). */
	aitensor_t gammas; /**< Vector of the scale parameters (\f$ \gamma_i \f$). */

	aitensor_t moving_means; /**< Vector of the moving averages of the means (required for inference). */
	aitensor_t moving_variances; /**< Vector of the moving averages of the variance (required for inference). */

	aitensor_t *trainable_params[2]; /**< Pointer to \f$ \beta \f$ and \f$ \gamma \f$ (which are the trainable parameters). */
	aitensor_t *gradients[2]; /**< Gradients of \f$ \beta \f$ and \f$ \gamma \f$ for the back propagation algorithm. */
	void *optimem[2]; /**< Memory field used by the trainings optimizer. */
	///@}

	/** @name Variables for internal computation
     *
     * These fields are automatically configured in the initializer function.
	 */
	///@{
	uint16_t parameter_shape[1]; /**< Shape of the parameter vectors ( \f$ \beta, \gamma, \mu, \sigma^2 \f$). */

	aitensor_t *means; /**< Vector of the means (\f$ \mu_i \f$). */
	aitensor_t *variances; /**< Vector of the variances (\f$ \sigma^2_i \f$). */
	///@}

    /** @name Math functions
	 * @brief Required data type specific math functions
	 */
	///@{

	/** @brief Required math function: Channel-wise empirical mean calculation
	 *
	 * Requires a math function that calculates the empirical mean for each channel of the given axis:\n
     * @f[
     *  means_i = \frac{1}{m} \sum_{j=1}^{m} x_{i,j}
     * @f]
     *
     * @param x             Input tensor
     * @param channel_axis  Axis of the input tensor that stores the channel dimension.
     * @param means         Resulting mean vector (1D)
	 */
	void (*empirical_mean_channelwise)(const aitensor_t *x, int8_t channel_axis, aitensor_t *means);

	/** @brief Required math function: Channel-wise empirical variance calculation
	 *
	 * Requires a math function that calculates the empirical variance for each channel of the given axis:\n
     * @f[
     *  variances_i = \frac{1}{m} \sum_{j=1}^{m} (x_{i,j} - \mu_i)^2
     * @f]
     *
     * @param x             Input tensor
     * @param channel_axis  Axis of the input tensor that stores the channel dimension.
     * @param means         Channel-wise mean values (1D)
     * @param variances     Resulting variance vector (1D)
	 */
	void (*empirical_variance_channelwise)(const aitensor_t *x, int8_t channel_axis, const aitensor_t *means, aitensor_t *variances);

	/** @brief Required math function: Exponential moving average
	 *
	 * Requires a math function that updates the moving average with a new data point:\n
     * @f[
     *  average \leftarrow momentum \cdot average + (1 - momentum) \cdot newdata
     * @f]
     *
     * @param new_data  Input tensor with the new data point.
     * @param momentum  aiscalar_t which controls the momentum of the average (range [0, 1]).
     * @param average   The average that is modified (input and output value),
	 */
	void (*exponential_moving_average)(const aitensor_t *new_data, const void *momentum, aitensor_t *average);

	/** @brief Required math function: Batch Normalization
	 *
	 * Requires a math function that performs Batch Normalization:\n
     * @f[
     *  y_{i,j} = \mathit{BN}(x_{i,j}) = \gamma_i \cdot \frac{x_{i,j} - \mu_{i}}{\sqrt{\sigma_{i}^2+\epsilon}} + \beta_i
     * @f]
     *
     * @param x             Input tensor.
     * @param channel_axis  Axis of the input tensor that stores the channel dimension.
     * @param means         Vector with the means (\f$ \mu_i \f$) of every channel.
     * @param variances     Vector with the variances (\f$ \sigma^2_i \f$) of every channel.
     * @param offsets       Vector with the offset parameters (\f$ \beta_i \f$) of every channel.
     * @param scales        Vector with the scaling parameters (\f$ \gamma_i \f$) of every channel.
     * @param eps           Small constant for numerical stability.
     * @param result        The resulting normalized tensor.
	 */
	void (*batch_norm)(const aitensor_t *x,
                       int8_t channel_axis,
                       const aitensor_t *means,
                       const aitensor_t *variances,
                       const aitensor_t *offsets,
                       const aitensor_t *scales,
                       const void *eps,
                       aitensor_t *result);

	/** @brief Required math function: Gradients of Batch Normalization
	 *
	 * Requires a math function that calculates the derivative of the Batch Normalization with respect to the input
	 * and the trainable parameters (\f$ \beta \f$ and \f$ \gamma \f$).\n
     *
     * Please refer to the paper by Ioffe and Szegedy (https://arxiv.org/abs/1502.03167) for the equations of the gradients.
     *
     * @param x             Input tensor.
     * @param channel_axis  Axis of the input tensor that stores the channel dimension.
     * @param means         Vector with the means (\f$ \mu_i \f$) of every channel.
     * @param variances     Vector with the variances (\f$ \sigma^2_i \f$) of every channel.
     * @param betas         Vector with the offset parameters (\f$ \beta_i \f$) of every channel.
     * @param gammas        Vector with the scaling parameters (\f$ \gamma_i \f$) of every channel.
     * @param delta_out     Gradient calculated by the output layer for gradient backpropagation.
     * @param eps           Small constant for numerical stability.
     * @param delta_in      The resulting gradients of the input (\f$ \mathrm{d}\mathcal{L} / \mathrm{d}x \f$).
     * @param d_betas       The resulting gradients of the \f$ \beta \f$ parameter (\f$ \mathrm{d}\mathcal{L} / \mathrm{d}\beta \f$).
     * @param d_gammas      The resulting gradients of the \f$ \gamma \f$ parameter (\f$ \mathrm{d}\mathcal{L}  / \mathrm{d}\gamma \f$).
	 */
	void (*d_batch_norm)(const aitensor_t *x_in,
                         int8_t axis,
                         const aitensor_t *means,
                         const aitensor_t *vars,
                         const aitensor_t *betas,
                         const aitensor_t *gammas,
                         const aitensor_t *delta_out,
                         const void *eps,
                         aitensor_t *delta_in,
                         aitensor_t *d_betas,
                         aitensor_t *d_gammas);

	///@}
};

/** @brief Batch Normalization layer type
 *
 * Defines the type of the layer (for example for type checks and debug prints).
 * See aicore_layertype for more information about the layer type.
 */
extern const aicore_layertype_t *ailayer_batch_norm_type;

/** @brief Initialize and connect the given Batch Normalization layer
 *
 * This function represents the "constructor" of the abstract Batch Normalization layer. It initializes the layer structure
 * and connects it to the previous layer.\n
 * This function is not intended to call it directly. Instead use one of the data type specific implementations
 * (like for example ailayer_batch_norm_f32_default()).
 *
 * @param *layer        The layer to initialize.
 * @param *input_layer  The previous layer that provides the inputs to the layer.
 * @return  Pointer to the (successfully) initialized general layer structure (ailayer_batch_norm.base)
 */
ailayer_t *ailayer_batch_norm(ailayer_batch_norm_t *layer, ailayer_t *input_layer);

/** @brief Calculate the forward pass for given Batch Normalization layer
 *
 * *Implementation of ailayer.forward.*
 *
 * It uses the result tensor of the previous layer as input and writes the result of the forward pass
 * to the result tensor (ailayer.result) of the given layer.
 *
 * Calculation of the forward pass result:<br>
 * For every element \f$ j \f$ neuron / channel \f$ i \f$ in the batch,
 * the transformation is defined as
 * @f[
 *  x_{out;i,j} = \mathit{BN}(x_{in;i,j}) = \gamma_i \cdot \frac{x_{out;i,j} - \mu_{i}}{\sqrt{\sigma_{i}^2+\epsilon}} + \beta_i
 * @f]
 * @f[
 *  \mu_i = \frac{1}{m} \sum_{j=1}^{m} x_{in;i,j}
 * @f]
 * @f[
 *  \sigma_i^2 = \frac{1}{m} \sum_{j=1}^{m} (x_{in;i,j} - \mu_i)^2
 * @f]
 *
 * \f$ \gamma \f$:	    Scaling vector\n
 * \f$ \beta \f$:	    Offset vector\n
 * \f$ \epsilon \f$:    Small constant for numerical stability\n
 * \f$ x_{in} \f$:	    Result of the forward pass of the previous layer\n
 * \f$ x_{out} \f$:	    Result of the forward pass of this layer\n\n
 *
 * Used math functions:
 * * ailayer_batch_norm.batch_norm
 * * ailayer_batch_norm.empirical_mean_channelwise
 * * ailayer_batch_norm.empirical_variance_channelwise
 * * ailayer_batch_norm.exponential_moving_average
 *
 * @param *self Layer to calculate the forward path for.
 */
void ailayer_batch_norm_forward(ailayer_t *self);

/** @brief Calculate the backward pass for the given Batch Normalization layer
 *
 * *Implementation of ailayer.backward.*
 *
 * It uses the deltas tensor of the next layer as input and writes the result of the backward pass
 * to the deltas tensor (ailayer.deltas) of the given layer.
 *
 * Calculates the gradients of \f$ \beta \f$ and \f$ \gamma \f$ and adds them to the corresponding gradients tensor.<br>
 * Calculates the gradients for backpropagation to the previous layer and writes them to \f$ \delta_{in} \f$.
 *
 * Please refer to the paper by Ioffe and Szegedy (https://arxiv.org/abs/1502.03167) for the equations of the gradients.
 *
 * Used math functions:
 * * ailayer_batch_norm.d_batch_norm
 *
 * @param *self Layer to calculate the backward path for.
 */
void ailayer_batch_norm_backward(ailayer_t *self);

/** @brief Calculate the shape of the result tensor (ailayer.result)
 *
 * *Implementation of ailayer.calc_result_shape.*
 *
 * Resulting shape equals input shape.
 *
 * @param *self Layer to calculate the resulting shape for.
 */
void ailayer_batch_norm_calc_result_shape(ailayer_t *self);


/** @brief Calculate and return the parameter memory size needed for this layer
 *
 * *Implementation of ailayer.sizeof_paramem.*
 *
 * The parameter size is calculated for the \link ailayer_batch_norm.gammas gammas \endlink,
 * \link ailayer_batch_norm.betas betas \endlink, \link ailayer_batch_norm.moving_means moving means \endlink
 * and \link ailayer_batch_norm.moving_variances moving variances \endlink tensors.
 *
 * @param *self The layer to calculate the parameter memory size for
 * @return  Calculated parameter memory size in bytes.
 */
uint32_t ailayer_batch_norm_sizeof_paramem(const ailayer_t *self);

/** @brief Distribute provided memory to the parameter pointers
 *
 * *Implementation of ailayer.set_paramem.*
 *
 * Distributes the given buffer to the parameter pointers and sets
 * the tensor parameters.\n
 * The required parameter size can be calculated with ailayer_batch_norm_sizeof_paramem()
 *
 * @param *self         The layer to set the memory fields for.
 * @param *memory_ptr   The memory that can be used for the parameters
 */
void ailayer_batch_norm_set_paramem(ailayer_t *self, void *memory_ptr);

/** @brief Calculate and return the memory size needed by this layer for training
 *
 * *Implementation of ailayer.sizeof_trainmem.*
 *
 * The memory size is calculated for the \link ailayer_batch_norm.means means \endlink
 * and \link ailayer_batch_norm.variances variances \endlink and for the gradient tensors
 * of \link ailayer_batch_norm.gammas gammas \endlink and \link ailayer_batch_norm.betas betas \endlink.
 *
 * @param *self The layer to calculate the gradient memory size for.
 * @return  Calculated gradient memory size in bytes.
 */
uint32_t ailayer_batch_norm_sizeof_trainmem(const ailayer_t *self);

/** @brief Distribute provided memory to the gradients pointers
 *
 * *Implementation of ailayer.set_trainmem.*
 *
 * The required memory size can be calculated with ailayer_batch_norm_sizeof_trainmem().
 *
 * @param *self         The layer to set the memory fields for.
 * @param *memory_ptr   The memory that can be used for the gradients
 */
void ailayer_batch_norm_set_trainmem(ailayer_t *self, void *memory_ptr);

#ifdef AIDEBUG_PRINT_MODULE_SPECS
/** @brief Print the layer specification
 *
 * @param *self     The layer to print the specification for
 */
void ailayer_batch_norm_print_specs(const ailayer_t *self);
#endif // AIDEBUG_PRINT_MODULE_SPECS

#endif // AILAYER_BATCH_NORM
