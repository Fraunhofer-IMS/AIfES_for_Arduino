/**
 * \file basic/express/aifes_express_f32_fnn.h
 * \internal
 * \date 04.08.2021
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
 * \brief AIfES Express functions for weights with F32 (float32) data type
 *
 * AIfES Express is a beginner friendly high-level API of AIfES.
 * This file contains all necessary functions for neural networks with float32 weights.
 */

#ifndef EXPRESS_FNN_F32
#define EXPRESS_FNN_F32

#include "core/aifes_math.h"

/** @brief Possible activation functions in AIfES-Express
 */
typedef enum
{
    AIfES_E_relu,           /**< Relu */
    AIfES_E_sigmoid,        /**< Sigmoid */
    AIfES_E_softmax,        /**< Softmax */
    AIfES_E_leaky_relu,     /**< Leaky_relu */
    AIfES_E_elu,            /**< Elu */
    AIfES_E_tanh,           /**< Tanh */
    AIfES_E_softsign,       /**< Softsign */
    AIfES_E_linear          /**< Linear */

}AIFES_E_activations;

/** @brief Possible loss functions in AIfES-Express
 */
typedef enum
{
    AIfES_E_mse,            /**< Mean squared error (MSE) */
    AIfES_E_crossentropy    /**< Crossentropy*/

}AIFES_E_loss;

/** @brief Possible optimizers in AIfES-Express
 */
typedef enum
{
    AIfES_E_adam,           /**< ADAM */
    AIfES_E_sgd             /**< SGD */

}AIFES_E_optimizer;

/** @brief Possible weight initialization methods in AIfES-Express
 */
typedef enum
{
    AIfES_E_init_uniform,           /**< Dices the weights in a range of values you specify */
    AIfES_E_init_glorot_uniform,    /**< Random numbers are uniformly diced within a certain range */
    AIfES_E_init_no_init            /**< No weight init. If starting weights are already available or if you want to continue training */

}AIFES_E_init_weights_method;

/** @brief Turn early stopping on or off
 */
typedef enum
{
    AIfES_E_early_stopping_off,
    AIfES_E_early_stopping_on

}AIFES_E_early_stopping;

/** @brief Parameters for the FNN model
 *
 * This structure contains all parameters for the creation of the model.
 *
 * **Example:**\n
 * In this example, a 2-3-1 FNN is generated.
 * The hidden layer and the output layer use a sigmoid activation function
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
 * \endcode
 */
struct AIFES_E_model_parameter_fnn_f32{
   uint32_t             layer_count;        /**< Count of all layers, including input and output layers */
   uint32_t             *fnn_structure;     /**< Pointer to the network structure */
   AIFES_E_activations  *fnn_activations;   /**< Pointer to the activation function list (AIFES_E_activations) */
   void *               flat_weights;       /**< Pointer to the array with the weights */
};

/** @brief Parameters for Training
 *
 * The structure includes all parameters for training.
 *
 * **Example:**
 * \code{.c}
 * AIFES_E_training_parameter_fnn_f32  nn_train_config;
 * nn_train_config.optimizer = AIfES_E_adam;
 * nn_train_config.sgd_momentum = 0.0f;         // Only for SGD optimizer
 * nn_train_config.loss = AIfES_E_mse;
 * nn_train_config.learn_rate = 0.05f;
 * nn_train_config.batch_size = 4;
 * nn_train_config.epochs = 100;
 * nn_train_config.epochs_loss_print_interval = 10;
 *
 * // Your individual print function
 * nn_train_config.loss_print_function = my_print_function;
 *
 * //You can enable early stopping, so that learning is automatically stopped when a learning target is reached
 * nn_train_config.early_stopping = AIfES_E_early_stopping_on;
 * //Define your target loss
 * nn_train_config.early_stopping_target_loss = 0.004;
 * \endcode
 */
struct AIFES_E_training_parameter_fnn_f32{
   AIFES_E_loss                 loss;   /**< Loss selection (AIFES_E_loss) */
   AIFES_E_optimizer            optimizer;  /**< Optimizer selection (AIFES_E_optimizer) */
   float                        learn_rate; /**< Learning rate for training (For all optimizers) */
   float                        sgd_momentum; /**< Optional momentum for SGD (Value 0.0f means Momentum off) */
   uint32_t                     batch_size; /**< Batch size (min: 1 -> max: Entire data set) */
   uint32_t                     epochs; /**< Number of desired epochs (If early stopping is on, can be stopped before) */
   uint32_t                     epochs_loss_print_interval; /**< Selection of the interval in which the loss is to be calculated and output via the print function */

    /** @brief Individual print function for the loss
    *
    * The loss is calculated only in the previously selected interval and output via the function to which the pointer points
    * The function must have the following structure
    * \code{.c}
    * void my_print_function(float loss)
    * {
    *    //E.g. a normal print output
    *    printf("Loss: %f\n", loss);
    * }
    * \endcode
    *
    * @param tensor           The tensor to print
    */
   void                         (*loss_print_function)(float);
   AIFES_E_early_stopping       early_stopping; /**< Switch early stopping on or off */
   float                        early_stopping_target_loss; /**< If early stopping is switched on, the target loss can be specified here. It is only checked in the print interval */
};

/** @brief Parameters for weight initialization
 *
 * Initialization method of weights and its parameterization
 *
 * **Example:** Set a uniform distributed weights initializer with automatic value range:\n
 * \code{.c}
 * AIFES_E_init_weights_parameter_fnn_f32  nn_weights_init;
 * nn_weights_init.init_weights_method = AIfES_E_init_glorot_uniform ;
 * \endcode
 *
 * **Example:** Set a uniform distributed weights initializer with custom value range:\n
 * \code{.c}
 * AIFES_E_init_weights_parameter_fnn_f32  nn_weights_init;
 * nn_weights_init.init_weights_method = AIfES_E_init_uniform;
 * nn_weights_init.min_init_uniform = -2; // only for the AIfES_E_init_uniform
 * nn_weights_init.max_init_uniform = 2;  // only for the AIfES_E_init_uniform
 * \endcode
 */
struct AIFES_E_init_weights_parameter_fnn_f32{
   AIFES_E_init_weights_method  init_weights_method;    /**< Weight initialization method (AIFES_E_init_weights_method) */
   float                        min_init_uniform;       /**< min value only for(AIfES_E_init_uniform) */
   float                        max_init_uniform;       /**< max value only for(AIfES_E_init_uniform) */
};

typedef struct AIFES_E_model_parameter_fnn_f32 	AIFES_E_model_parameter_fnn_f32;

typedef struct AIFES_E_training_parameter_fnn_f32 	AIFES_E_training_parameter_fnn_f32;

typedef struct AIFES_E_init_weights_parameter_fnn_f32 	AIFES_E_init_weights_parameter_fnn_f32;


/** @brief Calculates the total required float weights for the selected network structure
 *
 *
 * @param       *fnn_structure      The FNN structure
 * @param       layer_count         Number of layers
 * @return      Number of float weights required
 */
uint32_t AIFES_E_flat_weights_number_fnn_f32(uint32_t *fnn_structure,uint32_t layer_count);



/** @brief Executes the inference
 *
 * Requires the input tensor, the FNN model parameters and an output tensor for the results.
 * All data sets of the input tensor are calculated
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
 * float weights[] = {-10.1164f, -8.4212f, 5.4396f, 7.297f, -7.6482f, -9.0155f, -2.9653f,
 *                     2.3677f, -1.5968f, 12.0305f, -6.5858f, 11.9371f,-5.4247f};
 *
 * AIFES_E_model_parameter_fnn_f32 nn;
 * nn.layer_count = 3;
 * nn.fnn_structure = nn_structure;
 * nn.fnn_activations = nn_activations;
 * nn.flat_weights = weights;
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
 * error = AIFES_E_inference_fnn_f32(&input_tensor,
 *                                   &nn,
 *                                   &output_tensor);
 * \endcode
 *
 * @param       *input_tensor       Tensor with the inputs
 * @param       *AIFES_E_fnn        The FNN model parameters
 * @param       *output_tensor      Tensor for the results
 * @return      Error output
 */
int8_t AIFES_E_inference_fnn_f32(aitensor_t *input_tensor,
                                 AIFES_E_model_parameter_fnn_f32 *AIFES_E_fnn,
                                 aitensor_t *output_tensor);


/** @brief Executes the training
 *
 * Requires the input tensor, the target tensor, FNN model parameters, training parameters, weight initialization method and an output tensor for the results.
 * All data sets of the input tensor are used for the training
 *
 * Possible returns:
 * * 0 = success
 * * 1 = ERROR! Tensor dtype
 * * 2 = ERROR! Tensor shape: Data Number
 * * 3 = ERROR! Input tensor shape does not correspond to ANN inputs
 * * 4 = ERROR! Output tensor shape does not correspond to ANN outputs
 * * 5 = ERROR! Use the crossentropy as loss for softmax
 * * 6 = ERROR! learn_rate or sgd_momentum negative
 * * 7 = ERROR! Init uniform weights min - max wrong
 * * 8 = ERROR! batch_size: min = 1 / max = Number of training data
 * * 9 = ERROR! Unknown activation function
 * * 10 = ERROR! Unknown loss function
 * * 11 = ERROR! Unknown init weights method
 * * 12 = ERROR! Unknown optimizer
 * * 13 = ERROR! Not enough memory
 *
 * **Example:**
 * \code{.c}
 * void my_print_function(float loss)
 * {
 *    //E.g. a normal print output
 *    printf("Loss: %f\n", loss);
 * }
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
 * AIFES_E_training_parameter_fnn_f32  nn_train_config;
 * nn_train_config.optimizer = AIfES_E_adam;
 * nn_train_config.sgd_momentum = 0.0f;         // Only for SGD optimizer
 * nn_train_config.loss = AIfES_E_mse;
 * nn_train_config.learn_rate = 0.05f;
 * nn_train_config.batch_size = 4;
 * nn_train_config.epochs = 100;
 * nn_train_config.epochs_loss_print_interval = 10;
 *
 * // Your individual print function
 * nn_train_config.loss_print_function = my_print_function;
 *
 * //You can enable early stopping, so that learning is automatically stopped when a learning target is reached
 * nn_train_config.early_stopping = AIfES_E_early_stopping_on;
 * //Define your target loss
 * nn_train_config.early_stopping_target_loss = 0.004;
 *
 * AIFES_E_init_weights_parameter_fnn_f32  nn_weights_init;
 * nn_weights_init.init_weights_method = AIfES_E_init_glorot_uniform ;
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
 * float target_data[4*1] = {0.0f, 1.0f, 1.0f, 0.0f};                                     // Target Data
 * uint16_t target_shape[] = {4, 1};
 * aitensor_t target_tensor = AITENSOR_2D_F32(target_shape, target_data);              // Macro for the simple creation of a float32 tensor. Also usable in the normal AIfES version
 *
 * float output_data[4*1];                                                        // Output data
 * uint16_t output_shape[] = {4, 1};
 * aitensor_t output_tensor = AITENSOR_2D_F32(output_shape, output_data);
 *
 * int8_t error;
 * error = AIFES_E_training_fnn_f32(&input_tensor,
 *                                  &target_tensor,
 *                                  &nn,
 *                                  &nn_train_config,
 *                                  &nn_weights_init,
 *                                  &output_tensor);
 * \endcode
 *
 * @param       *input_tensor               Tensor with the input training data
 * @param       *target_tensor              Tensor with the training target data / labels
 * @param       *AIFES_E_fnn                The FNN model parameters
 * @param       *AIFES_E_fnn_training       The training parameters
 * @param       *AIFES_E_fnn_init_weights   The weight init parameters
 * @param       *output_tensor              Tensor for the results
 * @return      Error output
 */
int8_t AIFES_E_training_fnn_f32(aitensor_t *input_tensor,
                                aitensor_t *target_tensor,
                                AIFES_E_model_parameter_fnn_f32 *AIFES_E_fnn,
                                AIFES_E_training_parameter_fnn_f32 *AIFES_E_fnn_training,
                                AIFES_E_init_weights_parameter_fnn_f32 *AIFES_E_fnn_init_weights,
                                aitensor_t *output_tensor);



#endif // EXPRESS_FNN_F32
