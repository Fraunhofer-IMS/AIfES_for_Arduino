/**
 * \file basic/base/aialgo/aialgo_sequential_inference.h
 * \internal
 * \date 20.10.2020
 * \endinternal
 * \version 2.0alpha
 * \copyright  Copyright (C) 2020-2021  Fraunhofer Institute for Microelectronic Circuits and Systems.
    All rights reserved.

    AIfES is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * \brief Functions required for inference of models
 * \details The functions target memory allocation/scheduling, the calculation of the forward pass and quantization for model inference
 */

#ifndef AIALGO_SEQUENTIAL_INFERENCE
#define AIALGO_SEQUENTIAL_INFERENCE

#include "core/aifes_core.h"
#include "core/aifes_math.h"
#include "basic/base/aimath/aimath_basic.h"
#include "basic/default/ailayer/ailayer_dense_default.h"

/** @brief Calculate the memory requirements for intermediate results of an inference
 *
 * This memory is mainly for the result buffers of the layers.
 *
 * Use aialgo_schedule_inference_memory() to set the memory to the model.
 *
 * @param *model The model
 * @return       Required memory size in bytes
 */
uint32_t aialgo_sizeof_inference_memory(aimodel_t *model);

/** @brief Calculate the memory requirements for the trainable parameters (like weights, bias, ...) of the model
 *
 * Use aialgo_distribute_parameter_memory() to set the memory to the model.
 *
 * @param *model The model
 * @return       Required memory size in bytes
 */
uint32_t aialgo_sizeof_parameter_memory(aimodel_t *model);

/** @brief Assign the memory for intermediate results of an inference to the model
 *
 * The required memory size can be calculated with aialgo_sizeof_inference_memory()
 *
 * @param *model         The model
 * @param *memory_ptr    Pointer to the memory block
 * @param memory_size    Size of the memory block (for error checking)
 * @return               0 if successful
 */
uint8_t aialgo_schedule_inference_memory(aimodel_t *model, void *memory_ptr, uint32_t memory_size);

/** @brief Assign the memory for the trainable parameters (like weights, bias, ...) of the model
 *
 * Only use this function if the parameters are not pre-trained or manually configured.
 * Afterwards an initialization of the memory (for example by initializing the weights) has to be performed.
 *
 * The required memory size can be calculated with aialgo_sizeof_parameter_memory()
 *
 * @param *model         The model
 * @param *memory_ptr    Pointer to the memory block
 * @param memory_size    Size of the memory block (for error checking)
 * @return 0 if successful
 */
void aialgo_distribute_parameter_memory(aimodel_t *model, void *memory_ptr, uint32_t memory_size);

/** @brief Perform a forward pass on the model
 *
 * The result is stored in the result tensor of the output layer and a pointer to this is returned.
 * This output result is stored in the inference memory and is only valid as long as the inference memory is valid.
 * To get the output as a separate tensor, use aialgo_inference_model() instead.
 *
 * @param *model         The model
 * @param *input_data    Input data tensor of the same shape as the input_layer shape
 * @return               Pointer to the output data of the forward pass (points to the result tensor of the output layer)
 */
aitensor_t *aialgo_forward_model(aimodel_t *model, aitensor_t *input_data);

/** @brief Perform an inference on the model / Run the model
 *
 * Make shure to initialize the model (aialgo_compile_model()) and schedule the inference memory
 * (for example with aialgo_schedule_inference_memory() or aialgo_schedule_training_memory()) before
 * calling this function.
 *
 * Example:
 * \code{.c}
 * float input_data[] = {0.0f, 1.0f};
 * uint16_t input_shape[] = {1, 2}
 * aitensor_t input_tensor = {
 *     .dtype = aif32,
 *     .dim = 2,
 *     .shape = input_shape,
 *     .data = input_data
 * };
 *
 * float output_data[1];
 * uint16_t output_shape[] = {1, 1}
 * aitensor_t output_tensor = {
 *     .dtype = aif32,
 *     .dim = 2,
 *     .shape = output_shape,
 *     .data = output_data
 * };
 *
 * aialgo_inference_model(&model, &input_tensor, &output_tensor);
 *
 * // The results are now in the output_tensor
 * \endcode
 *
 * @param *model         The model
 * @param *input_data    Input data tensor of the same shape as the input_layer shape
 * @param *output_data   Empty tensor for the results of the inference with the size of your outputs
 * @return               Pointer to the output_data tensor with the results
 */
aitensor_t *aialgo_inference_model(aimodel_t *model, aitensor_t *input_data, aitensor_t *output_data);

/** @brief Initialize the model structure
*
* Counts the number of layers and trainable parameters in a model as preparation for inference or training.
*
* @param *model The model
* @return       0 if successful
*/
uint8_t aialgo_compile_model(aimodel_t *model);

/** @brief Print the layer structure of the model with the configured parameters
*
* @param *model The model
*/
void aialgo_print_model_structure(aimodel_t *model);

#endif // AIALGO_SEQUENTIAL_INFERENCE
