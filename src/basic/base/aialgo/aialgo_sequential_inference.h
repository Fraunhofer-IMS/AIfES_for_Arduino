/**
 * \file basic/base/aialgo/aialgo_sequential_inference.h
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
 * float input_data[] = {0.0f, 1.0f, 0.0f};
 * uint16_t input_shape[] = {1, 3}
 * aitensor_t input_tensor = AITENSOR_2D_F32(input_shape, input_data);
 *
 * float output_data[2];
 * uint16_t output_shape[] = {1, 2}
 * aitensor_t output_tensor = AITENSOR_2D_F32(output_shape, output_data);
 *
 * aialgo_inference_model(&model, &input_tensor, &output_tensor);
 *
 * // The results are now in the output_tensor
 * \endcode
 *
 * @param *model         The model
 * @param *input_data    Input data tensor of the same shape as the input_layer shape
 * @param *output_data   Empty tensor for the results of the inference with the size of your outputs
 * @return               0 if successful
 */
uint8_t aialgo_inference_model(aimodel_t *model, aitensor_t *input_data, aitensor_t *output_data);

/** @brief Initialize the model structure
*
* Counts the number of layers and trainable parameters in a model as preparation for inference or training.
*
* @param *model The model
* @return       0 if successful
*/
uint8_t aialgo_compile_model(aimodel_t *model);

/** @brief Quantize model parameters (weights and bias)
*
* @param *model_f32 Pointer to model with single-precision floating point parameters that should be quantized
* @param *model_q7 Pointer to model with quantized, fixed-point parameters in q7 format
* @param *representative_dataset Pointer to a dataset that represents real model inputs to determine fixed-point quantization parameters
*/
void aialgo_quantize_model_f32_to_q7(aimodel_t *model_f32, aimodel_t *model_q7, aitensor_t *representative_dataset);

/** @brief Initialize the quantization parameters of the layer results for \link aimath_q31.h Q31 \endlink data type
 *
 * Initializes the quantization parameters of the layer output (ailayer.result; output of the forward function)
 * to the given shift and zero_point = 0.\n
 * Use this function for example when you train a model in Q31 data-type.
 *
 * @param *model The model
 * @param shift  Number of decimal places (shift in Q31) of the layer results
 */
void aialgo_set_model_result_precision_q31(aimodel_t *model, uint16_t shift);

/** @brief Initialize the quantization parameters of the layer deltas for \link aimath_q31.h Q31 \endlink data type
 *
 * Initializes the quantization parameters of the layer deltas tensor (ailayer.deltas; output of the backward function)
 * to the given shift and zero_point = 0.\n
 * Use this function when you train a model in Q31 data-type.
 *
 * @param *model The model
 * @param shift  Number of decimal places (shift in Q31) of the layer deltas
 */
void aialgo_set_model_delta_precision_q31(aimodel_t *model, uint16_t shift);

/** @brief Initialize the quantization parameters of the gradients for \link aimath_q31.h Q31 \endlink data type
 *
 * Initializes the quantization parameters of the gradients tensors to the given shift and zero_point = 0.\n
 * Use this function when you train a model in Q31 data-type.
 *
 * @param *model The model
 * @param shift  Number of decimal places (shift in Q31) of the gradients
 */
void aialgo_set_model_gradient_precision_q31(aimodel_t *model, uint16_t shift);

/** @brief Print the layer structure of the model with the configured parameters
*
* @param *model The model
*/
void aialgo_print_model_structure(aimodel_t *model);

/** @brief Apply the specified setting to all layers in the model
 *
 * @param *model    The model
 * @param bitmask   32-bit mask to address the setting value (usually set to 0b1)
 * @param shift     The amount of bits to shift the bitmask to address the desired setting value (e.g. AILAYER_SETTINGS_TRAINING_MODE)
 * @param value     The target value
 */
void aialgo_set_layer_settings_model(aimodel_t *model, uint32_t bitmask, uint8_t shift, uint32_t value);

/** @brief Enables / disables the training mode of the model
*
* Some layers may behave differently during training than during inference (e.g. the Batch Normalization layer).\n
* The aialgo_train_model() function calls this function internally.
*
* The setting value can be accessed with:
* \code{c}
* if(AILAYER_SETTINGS_IS(layer->settings, 0b1, AILAYER_SETTINGS_TRAINING_MODE)){
*   ...
* }
* \endcode
*
* @param *model     The model
* @param value      Target value (TRUE or FALSE)
*/
void aialgo_set_training_mode_model(aimodel_t *model, uint8_t value);

/** @brief Enables / disables the batch mode of the model
*
* When executed in batch mode, a whole batch is processed by one forward pass.\n
* Some layers may behave differently during training than during inference (e.g. the Batch Normalization layer).\n
* The aialgo_train_model() function calls this function internally (Batch mode gets enabled if the batch_size equals the first element of
* the ailayer_input.input_shape and gets disabled otherwise).
*
* The setting value can be accessed with:
* \code{c}
* if(AILAYER_SETTINGS_IS(layer->settings, 0b1, AILAYER_SETTINGS_BATCH_MODE)){
*   ...
* }
* \endcode
*
* @param *model     The model
* @param value      Target value (TRUE or FALSE)
*/
void aialgo_set_batch_mode_model(aimodel_t *model, uint8_t value);

/** @brief Freeze / Unfreeze trainable parameters of the model
*
* When a layer is set to trainable = FALSE, the corresponding parameters are not modified by aialgo_train_model().
*
* The setting value can be accessed with:
* \code{c}
* if(AILAYER_SETTINGS_IS(layer->settings, 0b1, AILAYER_SETTINGS_TRAINABLE)){
*   ...
* }
* \endcode
*
* @param *model     The model
* @param value      Target value (TRUE or FALSE)
*/
void aialgo_set_trainable_model(aimodel_t *model, uint8_t value);

#endif // AIALGO_SEQUENTIAL_INFERENCE
