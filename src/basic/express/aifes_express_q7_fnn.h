/**
 * \file basic/express/aifes_express_q7_fnn.h
 * \internal
 * \date 15.11.2021
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
 * \brief AIfES Express functions for weights with Q7 (int8) data type
 *
 * AIfES Express is a beginner friendly high-level API of AIfES.
 * This file contains all necessary functions for neural networks with int8 weights.
 */

#ifndef EXPRESS_FNN_Q7
#define EXPRESS_FNN_Q7

#include "basic/express/aifes_express_f32_fnn.h"

/** @brief Calculates the required length of the uint8_t array for the FNN
 *
 * Contains the number of weights and additionally the necessary parameters for the fixpoint shifting
 *
 * @param       *fnn_structure      The FNN structure
 * @param       layer_count         Number of layers
 * @return      Required length of the uint8_t array
 */
uint32_t AIFES_E_flat_weights_number_fnn_q7(uint32_t *fnn_structure, uint32_t layer_count);


/** @brief Quantizes the weights of an F32 FNN into a Q7 FNN
 *
 * The representative dataset should cover the min max values of the inputs.
 * The dataset is needed for the calculation of the shift and the zero point of each layer.
 *
 * ** The Q7 quantized buffer can only be used with architectures with the same AIFES_MEMORY_ALIGNMENT.
 * The inference might crash if you export the buffer to architectures with different AIFES_MEMORY_ALIGNMENT. **
 *
 * Possible returns:
 * * 0 = success
 * * 1 = ERROR! Tensor dtype
 * * 2 = ERROR! Input tensor shape does not correspond to ANN inputs
 * * 3 = ERROR! Not enough memory
 * * 4 = ERROR! Unknown activation function
 *
 * **Example:**
 * \code{.c}
 * uint32_t nn_structure[3] = {2,3,1};
 *
 * AIFES_E_activations nn_activations[2];
 * nn_activations[0] = AIfES_E_sigmoid;
 * nn_activations[1] = AIfES_E_sigmoid;
 *
 * float weights[] = {-10.1164f, -8.4212f, 5.4396f, 7.297f, -7.6482f, -9.0155f, -2.9653f,
 *                     2.3677f, -1.5968f, 12.0305f, -6.5858f, 11.9371f,-5.4247f};
 *
 * AIFES_E_model_parameter_fnn_f32 nn;
 * nn.layer_count = 3;
 * nn.fnn_structure = nn_structure;
 * nn.fnn_activations = nn_activations;
 * nn.flat_weights = weights;
 *
 * uint32_t q7_parameter_memory_size = AIFES_E_flat_weights_number_fnn_q7(nn_structure, 3);
 * uint8_t *weights_q7 = (uint8_t *) malloc(q7_parameter_memory_size);
 *
 * // AIfES Express function: do the quantisation
 * AIFES_E_quantisation_fnn_f32_to_q7(&input_tensor,
 *                                    &nn,
 *                                    weights_q7);
 *
 * // Change the flat weights for q7 calculation
 * nn.flat_weights = weights_q7;
 * \endcode
 *
 * @param       *representative_f32_dataset     Tensor with the inputs
 * @param       *AIFES_E_fnn                    The FNN model parameters
 * @param       *q7_parameter_dataset           Tensor for the results
 * @return      Error output
 */
int8_t AIFES_E_quantisation_fnn_f32_to_q7(aitensor_t *representative_f32_dataset,
                                          AIFES_E_model_parameter_fnn_f32 *AIFES_E_fnn,
                                          uint8_t *q7_parameter_dataset);


/** @brief Executes the inference of a Q7 FNN
 *
 * Requires the input tensor, the FNN model parameters and an output tensor for the results.
 * Use here as flat_weights the q7_parameter_dataset calculated from quantization
 * The function takes float data as input tensor and converts it to Q7 format.
 * The inference is performed with the Q7 FNN.
 * For the output, the results are again converted as float and output via the output tensor.
 *
 * Possible returns:
 * * 0 = success
 * * 1 = ERROR! Tensor dtype
 * * 2 = ERROR! Tensor shape: Data Number
 * * 3 = ERROR! Input tensor shape does not correspond to ANN inputs
 * * 4 = ERROR! Output tensor shape does not correspond to ANN outputs
 * * 5 = ERROR! Unknown activation function
 * * 6 = ERROR! Not enough memory
 *
 * **Example:**
 * \code{.c}
 * uint32_t nn_structure[3] = {2,3,1};
 *
 * AIFES_E_activations nn_activations[2];
 * nn_activations[0] = AIfES_E_sigmoid;
 * nn_activations[1] = AIfES_E_sigmoid;
 *
 * // Q7 weights (4 byte aligned: AIFES_MEMORY_ALIGNMENT = 4)
 * uint32_t weights_q7[ 16 ] = {
 *     0x00BA0007, 0x00290003, 0x000B0003, 0x00000003, 0x3A2CBDAF, 0x0000B8C3, 0x0000000A, 0x00000000, 0xFFFFF424, 0x00000979,
 *     0xFFFFF99D, 0x00000003, 0x005FCB60, 0x0000000B, 0x00000000, 0xFFFFD49A
 * };
 *
 * // Q7 weights (2 byte aligned: AIFES_MEMORY_ALIGNMENT = 2)
 * //uint16_t weights_q7[ 29 ] = {
 * //    0x0007, 0x00BA, 0x0003, 0x0029, 0x0003, 0x000B, 0x0003, 0x0000, 0xBDAF, 0x3A2C, 0xB8C3, 0x000A, 0x0000, 0x0000, 0xF424, 0xFFFF,
 * //    0x0979, 0x0000, 0xF99D, 0xFFFF, 0x0003, 0x0000, 0xCB60, 0x005F, 0x000B, 0x0000, 0x0000, 0xD49A, 0xFFFF
 * //};
 *
 * AIFES_E_model_parameter_fnn_f32 nn;
 * nn.layer_count = 3;
 * nn.fnn_structure = nn_structure;
 * nn.fnn_activations = nn_activations;
 * nn.flat_weights = weights_q7;
 *
 * float input_data[4][2] = {
 *     {0.0f, 0.0f},
 *     {0.0f, 1.0f},
 *     {1.0f, 0.0f},
 *     {1.0f, 1.0f}
 * };
 * uint16_t input_shape[4*2] = {4, 2};                     // Definition of the input shape
 * aitensor_t input_tensor = AITENSOR_2D_F32(input_shape, input_data);                 // Macro for the simple creation of a float32 tensor. Also usable in the normal AIfES version
 *
 * float output_data[4*1];                                                        // Output data
 * uint16_t output_shape[] = {4, 1};
 * aitensor_t output_tensor = AITENSOR_2D_F32(output_shape, output_data);
 *
 * int8_t error;
 * error = AIFES_E_inference_fnn_q7(&input_tensor,
 *                                  &nn,
 *                                  &output_tensor);
 * \endcode
 *
 * @param       *input_tensor       Tensor with the inputs
 * @param       *AIFES_E_fnn        The FNN model parameters
 * @param       *output_tensor      Tensor for the results
 * @return      Error output
 */
int8_t AIFES_E_inference_fnn_q7(aitensor_t *input_tensor,
                                AIFES_E_model_parameter_fnn_f32 *AIFES_E_fnn,
                                aitensor_t *output_tensor);

#endif // EXPRESS_FNN_Q7
