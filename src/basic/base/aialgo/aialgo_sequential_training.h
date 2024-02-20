/**
 * \file basic/base/aialgo/aialgo_sequential_training.h
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
 * \brief Functions required for the training of models
 * \details The functions target memory allocation/scheduling and the backpropagation for model training
 */

#ifndef AIALGO_SEQUENTIAL_TRAINING
#define AIALGO_SEQUENTIAL_TRAINING

#include "core/aifes_core.h"
#include "core/aifes_math.h"
#include "basic/base/aimath/aimath_basic.h"
#include "basic/base/aimath/aimath_q31.h"

/** @brief Calculate the memory requirements for model training
 *
 * This memory is used for intermediate results, gradients and momentums.
 *
 * Use aialgo_schedule_training_memory() to set the memory to the model.
 *
 * @param *model        The model
 * @param *optimizer    The optimizer that is used for training
 * @return              Required memory size in bytes
 */
uint32_t aialgo_sizeof_training_memory(aimodel_t *model, aiopti_t *optimizer);

/** @brief Assign the memory for model training
 *
 * This memory is used for intermediate results, gradients and momentums.
 *
 * The required memory size can be calculated with aialgo_sizeof_training_memory().
 *
 * @param *model        The model
 * @param *optimizer    The optimizer that is used for training
 * @param *memory_ptr   Pointer to the memory block
 * @param memory_size   Size of the memory block (for error checking)
 * @return              0 if successful
 */
uint8_t aialgo_schedule_training_memory(aimodel_t *model, aiopti_t *optimizer, void *memory_ptr, uint32_t memory_size);

/** @brief Initialize the optimization memory of the model layers
 *
 * @param *model     The model
 * @param *optimizer The optimizer that is used for training
 */
void aialgo_init_model_for_training(aimodel_t *model, aiopti_t *optimizer);

/** @brief Perform the backward pass
 *
 * @param *model         The model
 * @param *target_data   The tensor containing the target data / labels
 */
void aialgo_backward_model(aimodel_t *model, aitensor_t *target_data);

/** @brief Perform one training epoch on all data batches of the dataset using backpropagation
 *
 * Make shure to initialize the model (aialgo_compile_model()) and schedule the training memory
 * (for example with aialgo_schedule_training_memory()) and initialize the training memory
 * (aialgo_init_model_for_training()) before calling this function.
 *
 * Example: Training of an F32 model for multiple epochs
 * \code{.c}
 * int epochs = 100;
 * int batch_size = 4;
 * int print_interval = 10;
 *
 * float loss;
 * for(i = 0; i < epochs; i++)
 * {
 *     // One epoch of training. Iterates through the whole data once
 *     aialgo_train_model(&model, &input_tensor, &target_tensor, optimizer, batch_size);

 *     // Calculate and print loss every print_interval epochs
 *     if(i % print_interval == 0)
 *     {
 *         aialgo_calc_loss_model_f32(&model, &input_tensor, &target_tensor, &loss);
 *         printf("Epoch %5d: loss: %f\n", i, loss);
 *     }
 * }
 * \endcode
 *
 * @param *model            The model
 * @param *input_tensor     The tensor containing the input data
 * @param *target_tensor    The tensor containing the target data / labels
 * @param *optimizer        The optimizer that is used for training
 * @param batch_size        Size of a batch / Number of input vektors
 */
uint8_t aialgo_train_model(aimodel_t *model, aitensor_t *input_tensor, aitensor_t *target_tensor, aiopti_t *optimizer, uint32_t batch_size);

/** @brief Calculate the loss in \link aimath_f32.h F32 \endlink data type
 *
 * @param *model         The model
 * @param *input_data    Tensor containing the input data
 * @param *target_data   Tensor containing the target data / labels
 * @param *result        The calculated loss will be written here
 */
uint8_t aialgo_calc_loss_model_f32(aimodel_t *model, aitensor_t *input_data, aitensor_t *target_data, float *result);

/** @brief Calculate the loss in \link aimath_q31.h Q31 \endlink data type
 *
 * @param *model         The model
 * @param *input_data    Tensor containing the input data
 * @param *target_data   Tensor containing the target data / labels
 * @param *result        The calculated loss will be written here. The zero_point and the scale should be set to proper values.
 */
uint8_t aialgo_calc_loss_model_q31(aimodel_t *model, aitensor_t *input_data, aitensor_t *target_data, aiscalar_q31_t *result);

/** @brief Set the gradients to zero
 *
 * @param *model     The model
 * @param *optimizer The optimizer that is used for training
 */
void aialgo_zero_gradients_model(aimodel_t *model, aiopti_t *optimizer);

/** @brief Perform the optimization step on the model parameters
 *
 * @param *model     The model
 * @param *optimizer The optimizer that is used for training
 */
void aialgo_update_params_model(aimodel_t *model, aiopti_t *optimizer);

/** @brief Print the loss specs
 *
 * Prints information like type, data type and constants to the console.
 *
 * @param *loss The loss
 */
void aialgo_print_loss_specs(ailoss_t *loss);

/** @brief Print the optimizer specs
 *
 * Prints information like type, data type and constants to the console.
 *
 * @param *opti The optimizer
 */
void aialgo_print_optimizer_specs(aiopti_t *opti);

/** @brief Initialize the parameters of the given model with their default initialization method
 *
 * Initialize the parameters of all layers that have a default initialization function (ailayer.init_params) configured
 * and that are set to trainable (ailayer.settings[AILAYER_SETTINGS_TRAINABLE] = TRUE).
 *
 * @param *model The model to initialize
 */
void aialgo_initialize_parameters_model(aimodel_t *model);

#endif // AIALGO_SEQUENTIAL_TRAINING
