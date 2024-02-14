/**
 * \file basic/express/aifes_express_f32_fnn.c
 * \version 2.2.0
 * \date 04.08.2021
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
 * \brief
 * \details
 */

#include "basic/express/aifes_express_f32_fnn.h"


// Include the layers in default implementation
#include "basic/default/ailayer/ailayer_dense_default.h"
#include "basic/default/ailayer/ailayer_input_default.h"
#include "basic/default/ailayer/ailayer_relu_default.h"
#include "basic/default/ailayer/ailayer_leaky_relu_default.h"
#include "basic/default/ailayer/ailayer_elu_default.h"
#include "basic/default/ailayer/ailayer_sigmoid_default.h"
#include "basic/default/ailayer/ailayer_tanh_default.h"
#include "basic/default/ailayer/ailayer_softmax_default.h"
#include "basic/default/ailayer/ailayer_softsign_default.h"

// Include the losses in default implementation
#include "basic/default/ailoss/ailoss_mse_default.h"
#include "basic/default/ailoss/ailoss_crossentropy_default.h"

// Include the optimizers in default implementation
#include "basic/default/aiopti/aiopti_sgd_default.h"
#include "basic/default/aiopti/aiopti_adam_default.h"

// Include the algorithmic
#include "basic/base/aialgo/aialgo_sequential_inference.h"
#include "basic/base/aialgo/aialgo_sequential_training.h"


uint32_t AIFES_E_flat_weights_number_fnn_f32(uint32_t *fnn_structure,uint32_t layer_count)
 {
    uint32_t i = 0;
    uint32_t sum = 0;


    for (i = 1; i < layer_count; i++)
    {
        sum = sum + fnn_structure[i-1] * fnn_structure[i] + fnn_structure[i];
    }

    return sum;
 }


int8_t AIFES_E_inference_fnn_f32(aitensor_t *input_tensor,
                                 AIFES_E_model_parameter_fnn_f32 *AIFES_E_fnn,
                                 aitensor_t *output_tensor)
{
    uint32_t i = 0;

    if(input_tensor->dtype != aif32 || output_tensor->dtype != aif32)
    {
        //printf("ERROR! Tensor dtype\n");
        return(-1);
    }

    if(input_tensor->shape[0] != output_tensor->shape[0])
    {
        //printf("ERROR! Tensor shape: Data Number\n");
        return(-2);
    }

    if(input_tensor->shape[1] != AIFES_E_fnn->fnn_structure[0])
    {
        //printf("ERROR! Input tensor shape does not correspond to ANN inputs\n");
        return(-3);
    }

    if(output_tensor->shape[1] != AIFES_E_fnn->fnn_structure[AIFES_E_fnn->layer_count - 1])
    {
        //printf("ERROR! Output tensor shape does not correspond to ANN outputs\n");
        return(-4);
    }

    // Counter for the activation functions
    uint32_t relu_count = 0;
    uint32_t sigmoid_count = 0;
    uint32_t softmax_count = 0;
    uint32_t leaky_relu_count = 0;
    uint32_t elu_count = 0;
    uint32_t tanh_count = 0;
    uint32_t softsign_count = 0;
    uint32_t linear_count = 0;

    // Count for the activation functions in the FNN structure
    for (i = 0; i < AIFES_E_fnn->layer_count - 1; i++) {
        switch(AIFES_E_fnn->fnn_activations[i]) {
            case AIfES_E_relu:
                relu_count ++;
                break;
            case AIfES_E_sigmoid:
                sigmoid_count ++;
                break;
            case AIfES_E_softmax:
                softmax_count ++;
                break;
            case AIfES_E_leaky_relu:
                leaky_relu_count ++;
                break;
            case AIfES_E_elu:
                elu_count ++;
                break;
            case AIfES_E_tanh:
                tanh_count ++;
                break;
            case AIfES_E_softsign:
                softsign_count ++;
                break;
            case AIfES_E_linear:
                linear_count ++;
                break;
            default :
                //printf("ERROR! Unknown activation function\n" );
                return(-5);
       }
    }

    // Define the activation functions
    ailayer_relu_f32_t          relu_layer[relu_count];
    ailayer_sigmoid_f32_t       sigmoid_layer[sigmoid_count];
    ailayer_softmax_f32_t       softmax_layer[softmax_count];
    ailayer_leaky_relu_f32_t    leaky_relu_layer[leaky_relu_count]; // alpha=0.01
    ailayer_elu_f32_t           elu_layer[elu_count];               // alpha=1.0
    ailayer_tanh_f32_t          tanh_layer[tanh_count];
    ailayer_softsign_f32_t      softsign_layer[softsign_count];

    // Init leaky relu with default alpha
    for (i = 0; i < leaky_relu_count; i++) {
     leaky_relu_layer[i].alpha = 0.01f;
    }
    // Init elu with default alpha
    for (i = 0; i < elu_count; i++) {
     elu_layer[i].alpha = 1.0f;
    }

    // Define the dense layer
    ailayer_dense_t         dense_layer[AIFES_E_fnn->layer_count - 1];

    // Use the variables as counter
    relu_count = 0;
    sigmoid_count = 0;
    softmax_count = 0;
    leaky_relu_count = 0;
    elu_count = 0;
    tanh_count = 0;
    softsign_count = 0;
    linear_count = 0;


    // Input layer
    uint16_t input_layer_shape[] = {1, AIFES_E_fnn->fnn_structure[0]};      // Definition of the input layer shape (Must fit to the input tensor)
    ailayer_input_t input_layer;                // Creation of the AIfES input layer
    input_layer.input_dim = 2;                  // Definition of the input dimension (Must fit to the input tensor)
    input_layer.input_shape = input_layer_shape;// Handover of the input layer shape

    aimodel_t model;  // AIfES model
    ailayer_t *x;     // Layer object from AIfES, contains the layers

    // Generate the dense layer and the output layer
    for (i = 0; i < AIFES_E_fnn->layer_count - 1; i++) {

        if(i==0)
        {
             // Input layer
             model.input_layer = ailayer_input_f32_default(&input_layer);
             // first dense layer
             dense_layer[i].neurons =  AIFES_E_fnn->fnn_structure[i+1];
             // Create first dense layer
             x = ailayer_dense_f32_default(&dense_layer[i], model.input_layer);
        }
        else
        {
            dense_layer[i].neurons =  AIFES_E_fnn->fnn_structure[i+1];  // Don't use the input neurons
            x = ailayer_dense_f32_default(&dense_layer[i], x);
        }

        switch(AIFES_E_fnn->fnn_activations[i]) {
            case AIfES_E_relu:
                if(i == AIFES_E_fnn->layer_count - 2){
                    model.output_layer = ailayer_relu_f32_default(&relu_layer[relu_count], x);  // Output layer
                }else{
                    x = ailayer_relu_f32_default(&relu_layer[relu_count], x);
                }
                relu_count ++;
                break;
            case AIfES_E_sigmoid:
                if(i == AIFES_E_fnn->layer_count - 2){
                    model.output_layer = ailayer_sigmoid_f32_default(&sigmoid_layer[sigmoid_count], x);
                }else{
                    x = ailayer_sigmoid_f32_default(&sigmoid_layer[sigmoid_count], x);
                }
                sigmoid_count ++;
                break;
            case AIfES_E_softmax:
                if(i == AIFES_E_fnn->layer_count - 2){
                    model.output_layer = ailayer_softmax_f32_default(&softmax_layer[softmax_count], x);
                }else{
                    x = ailayer_softmax_f32_default(&softmax_layer[softmax_count], x);
                }
                softmax_count ++;
                break;
            case AIfES_E_leaky_relu:
                if(i == AIFES_E_fnn->layer_count - 2){
                    model.output_layer = ailayer_leaky_relu_f32_default(&leaky_relu_layer[leaky_relu_count], x);
                }else{
                    x = ailayer_leaky_relu_f32_default(&leaky_relu_layer[leaky_relu_count], x);
                }
                leaky_relu_count ++;
                break;
            case AIfES_E_elu:
                if(i == AIFES_E_fnn->layer_count - 2){
                    model.output_layer = ailayer_elu_f32_default(&elu_layer[elu_count], x);
                }else{
                    x = ailayer_elu_f32_default(&elu_layer[elu_count], x);
                }
                elu_count ++;
                break;
            case AIfES_E_tanh:
                if(i == AIFES_E_fnn->layer_count - 2){
                    model.output_layer = ailayer_tanh_f32_default(&tanh_layer[tanh_count], x);
                }else{
                    x = ailayer_tanh_f32_default(&tanh_layer[tanh_count], x);
                }
                tanh_count ++;
                break;
            case AIfES_E_softsign:
                if(i == AIFES_E_fnn->layer_count - 2){
                    model.output_layer = ailayer_softsign_f32_default(&softsign_layer[softsign_count], x);
                }else{
                    x = ailayer_softsign_f32_default(&softsign_layer[softsign_count], x);
                }
                softsign_count ++;
                break;
            case AIfES_E_linear:
                if(i == AIFES_E_fnn->layer_count - 2){
                    model.output_layer = x;
                }else{
                    //no activation needed
                }
                linear_count ++;
                break;

            default :
                //printf("ERROR! Unknown activation function\n" );
                return(-5);
        }

    }

    aialgo_compile_model(&model); // Compile the AIfES model

    // -------------------------------- Pass the weights to the model ----------------------------------
    //Calculate required memory for the weights in Byte
    uint32_t parameter_memory_size = aialgo_sizeof_parameter_memory(&model);

    // Distribute the weights over the model
    aialgo_distribute_parameter_memory(&model, (void *) AIFES_E_fnn->flat_weights, parameter_memory_size);

    // -------------------------------- Allocate and schedule the working memory for inference ---------

    // Allocate memory for result and temporal data
    uint32_t memory_size = aialgo_sizeof_inference_memory(&model);

    void *memory_ptr = malloc(memory_size);

    if(memory_ptr == NULL)
    {
        //printf("ERROR! Not enough memory\n" );
        return(-6);
    }

    // Schedule the memory over the model
    aialgo_schedule_inference_memory(&model, memory_ptr, memory_size);

    //Do the inference and pass the result to the output_tensor
    aialgo_inference_model(&model, input_tensor, output_tensor);

    //Free the inference memory
    free(memory_ptr);

    return(0);
}

int8_t AIFES_E_training_fnn_f32(aitensor_t *input_tensor,
                                aitensor_t *target_tensor,
                                AIFES_E_model_parameter_fnn_f32 *AIFES_E_fnn,
                                AIFES_E_training_parameter_fnn_f32 *AIFES_E_fnn_training,
                                AIFES_E_init_weights_parameter_fnn_f32 *AIFES_E_fnn_init_weights,
                                aitensor_t *output_tensor)
{
    uint32_t i = 0;

    if(input_tensor->dtype != aif32 || output_tensor->dtype != aif32 || target_tensor->dtype != aif32)
    {
        //printf("ERROR! Tensor dtype\n");
        return(-1);
    }

    if(input_tensor->shape[0] != output_tensor->shape[0] || input_tensor->shape[0] != target_tensor->shape[0])
    {
        //printf("ERROR! Tensor shape: Data Number\n");
        return(-2);
    }

    if(input_tensor->shape[1] != AIFES_E_fnn->fnn_structure[0])
    {
        //printf("ERROR! Input tensor shape does not correspond to ANN inputs\n");
        return(-3);
    }

    if(output_tensor->shape[1] != AIFES_E_fnn->fnn_structure[AIFES_E_fnn->layer_count - 1] || target_tensor->shape[1] != AIFES_E_fnn->fnn_structure[AIFES_E_fnn->layer_count - 1])
    {
        //printf("ERROR! Output or target tensor shape does not correspond to ANN outputs\n");
        return(-4);
    }

    if(AIFES_E_fnn->fnn_activations[AIFES_E_fnn->layer_count - 2] == AIfES_E_softmax && AIFES_E_fnn_training->loss != AIfES_E_crossentropy)
    {
        //printf("ERROR! Use the crossentropy as loss for softmax\n");
        return(-5);
    }

    if(AIFES_E_fnn_training->sgd_momentum < 0 || AIFES_E_fnn_training->learn_rate < 0)
    {
        //printf("ERROR! learn_rate or sgd_momentum negative\n");
        return(-6);
    }

    if(AIFES_E_fnn_init_weights->init_weights_method == AIfES_E_init_uniform)
    {
        if(AIFES_E_fnn_init_weights->min_init_uniform >= AIFES_E_fnn_init_weights->max_init_uniform)
        {
            //printf("ERROR! Init uniform weights min - max wrong\n");
            return(-7);
        }
    }

    if(AIFES_E_fnn_training->batch_size > input_tensor->shape[0] || AIFES_E_fnn_training->batch_size <= 0)
    {
        //printf("ERROR! batch_size: min = 1 / max = Number of training data\n");
        return(-8);
    }


    // Counter for the activation functions
    uint32_t relu_count = 0;
    uint32_t sigmoid_count = 0;
    uint32_t softmax_count = 0;
    uint32_t leaky_relu_count = 0;
    uint32_t elu_count = 0;
    uint32_t tanh_count = 0;
    uint32_t softsign_count = 0;
    uint32_t linear_count = 0;

    // Count for the activation functions in the FNN structure
    for (i = 0; i < AIFES_E_fnn->layer_count - 1; i++) {
        switch(AIFES_E_fnn->fnn_activations[i]) {
            case AIfES_E_relu:
                relu_count ++;
                break;
            case AIfES_E_sigmoid:
                sigmoid_count ++;
                break;
            case AIfES_E_softmax:
                softmax_count ++;
                break;
            case AIfES_E_leaky_relu:
                leaky_relu_count ++;
                break;
            case AIfES_E_elu:
                elu_count ++;
                break;
            case AIfES_E_tanh:
                tanh_count ++;
                break;
            case AIfES_E_softsign:
                softsign_count ++;
                break;
            case AIfES_E_linear:
                linear_count ++;
                break;
            default :
                //printf("ERROR! Unknown activation function\n" );
                return(-9);
       }
    }

    // Define the activation functions
    ailayer_relu_f32_t          relu_layer[relu_count];
    ailayer_sigmoid_f32_t       sigmoid_layer[sigmoid_count];
    ailayer_softmax_f32_t       softmax_layer[softmax_count];
    ailayer_leaky_relu_f32_t    leaky_relu_layer[leaky_relu_count]; // alpha=0.01
    ailayer_elu_f32_t           elu_layer[elu_count];               // alpha=1.0
    ailayer_tanh_f32_t          tanh_layer[tanh_count];
    ailayer_softsign_f32_t      softsign_layer[softsign_count];

    // Init leaky relu with default alpha
    for (i = 0; i < leaky_relu_count; i++) {
     leaky_relu_layer[i].alpha = 0.01f;
    }
    // Init elu with default alpha
    for (i = 0; i < elu_count; i++) {
     elu_layer[i].alpha = 1.0f;
    }

    // Define the dense layer
    ailayer_dense_t         dense_layer[AIFES_E_fnn->layer_count - 1];

    // Use the variables as counter
    relu_count = 0;
    sigmoid_count = 0;
    softmax_count = 0;
    leaky_relu_count = 0;
    elu_count = 0;
    tanh_count = 0;
    softsign_count = 0;
    linear_count = 0;


    // Input layer
    uint16_t input_layer_shape[] = {1, AIFES_E_fnn->fnn_structure[0]};      // Definition of the input layer shape (Must fit to the input tensor)
    ailayer_input_t input_layer;                // Creation of the AIfES input layer
    input_layer.input_dim = 2;                  // Definition of the input dimension (Must fit to the input tensor)
    input_layer.input_shape = input_layer_shape;// Handover of the input layer shape

    aimodel_t model;  // AIfES model
    ailayer_t *x;     // Layer object from AIfES, contains the layers

    // Generate the dense layer and the output layer
    for (i = 0; i < AIFES_E_fnn->layer_count - 1; i++) {

        if(i==0)
        {
             // Input layer
             model.input_layer = ailayer_input_f32_default(&input_layer);
             // first dense layer
             dense_layer[i].neurons =  AIFES_E_fnn->fnn_structure[i+1];
             // Create first dense layer
             x = ailayer_dense_f32_default(&dense_layer[i], model.input_layer);
        }
        else
        {
            dense_layer[i].neurons =  AIFES_E_fnn->fnn_structure[i+1];  // Don't use the input neurons
            x = ailayer_dense_f32_default(&dense_layer[i], x);
        }

        switch(AIFES_E_fnn->fnn_activations[i]) {
            case AIfES_E_relu:
                if(i == AIFES_E_fnn->layer_count - 2){
                    model.output_layer = ailayer_relu_f32_default(&relu_layer[relu_count], x);  // Output layer
                }else{
                    x = ailayer_relu_f32_default(&relu_layer[relu_count], x);
                }
                relu_count ++;
                break;
            case AIfES_E_sigmoid:
                if(i == AIFES_E_fnn->layer_count - 2){
                    model.output_layer = ailayer_sigmoid_f32_default(&sigmoid_layer[sigmoid_count], x);
                }else{
                    x = ailayer_sigmoid_f32_default(&sigmoid_layer[sigmoid_count], x);
                }
                sigmoid_count ++;
                break;
            case AIfES_E_softmax:
                if(i == AIFES_E_fnn->layer_count - 2){
                    model.output_layer = ailayer_softmax_f32_default(&softmax_layer[softmax_count], x);
                }else{
                    x = ailayer_softmax_f32_default(&softmax_layer[softmax_count], x);
                }
                softmax_count ++;
                break;
            case AIfES_E_leaky_relu:
                if(i == AIFES_E_fnn->layer_count - 2){
                    model.output_layer = ailayer_leaky_relu_f32_default(&leaky_relu_layer[leaky_relu_count], x);
                }else{
                    x = ailayer_leaky_relu_f32_default(&leaky_relu_layer[leaky_relu_count], x);
                }
                leaky_relu_count ++;
                break;
            case AIfES_E_elu:
                if(i == AIFES_E_fnn->layer_count - 2){
                    model.output_layer = ailayer_elu_f32_default(&elu_layer[elu_count], x);
                }else{
                    x = ailayer_elu_f32_default(&elu_layer[elu_count], x);
                }
                elu_count ++;
                break;
            case AIfES_E_tanh:
                if(i == AIFES_E_fnn->layer_count - 2){
                    model.output_layer = ailayer_tanh_f32_default(&tanh_layer[tanh_count], x);
                }else{
                    x = ailayer_tanh_f32_default(&tanh_layer[tanh_count], x);
                }
                tanh_count ++;
                break;
            case AIfES_E_softsign:
                if(i == AIFES_E_fnn->layer_count - 2){
                    model.output_layer = ailayer_softsign_f32_default(&softsign_layer[softsign_count], x);
                }else{
                    x = ailayer_softsign_f32_default(&softsign_layer[softsign_count], x);
                }
                softsign_count ++;
                break;
            case AIfES_E_linear:
                if(i == AIFES_E_fnn->layer_count - 2){
                    model.output_layer = x;
                }else{
                    //no activation needed
                }
                linear_count ++;
                break;

            default :
                //printf("ERROR! Unknown activation function\n" );
                return(-5);
        }

    }

    ailoss_mse_t mse_loss; //Loss: mean squared error
    ailoss_crossentropy_t crossentropy_loss; //Loss: crossentropy

    switch(AIFES_E_fnn_training->loss){
        case AIfES_E_mse:
            model.loss = ailoss_mse_f32_default(&mse_loss, model.output_layer);
            break;
        case AIfES_E_crossentropy:
            model.loss = ailoss_crossentropy_f32_default(&crossentropy_loss, model.output_layer);
            break;
        default :
            //printf("ERROR! Unknown loss function\n" );
            return(-10);
    }


    aialgo_compile_model(&model); // Compile the AIfES model

    // -------------------------------- Pass the weights to the model ----------------------------------
    //Calculate required memory for the weights in Byte
    uint32_t parameter_memory_size = aialgo_sizeof_parameter_memory(&model);

    // Distribute the weights over the model
    aialgo_distribute_parameter_memory(&model, (void *) AIFES_E_fnn->flat_weights, parameter_memory_size);

    // ------------------------------- Initialize the parameters ------------------------------

    // Init Weights
    switch(AIFES_E_fnn_init_weights->init_weights_method){
        case AIfES_E_init_uniform:
            for (i = 0; i < AIFES_E_fnn->layer_count - 1; i++) {
                aimath_f32_default_tensor_init_uniform(&dense_layer[i].weights,AIFES_E_fnn_init_weights->max_init_uniform,AIFES_E_fnn_init_weights->min_init_uniform);
                aimath_f32_default_tensor_init_uniform(&dense_layer[i].bias,AIFES_E_fnn_init_weights->max_init_uniform,AIFES_E_fnn_init_weights->min_init_uniform);
            }
            break;
        case AIfES_E_init_glorot_uniform:
            for (i = 0; i < AIFES_E_fnn->layer_count - 1; i++) {
                aimath_f32_default_init_glorot_uniform(&dense_layer[i].weights);
                aimath_f32_default_init_zeros(&dense_layer[i].bias);
            }
            break;
        case AIfES_E_init_no_init:
            // No Init
            break;
        default :
            //printf("ERROR! Unknown init weights method\n" );
            return(-11);
    }

    // -------------------------------- Define the optimizer for training ---------------------

    aiopti_t *optimizer; // Object for the optimizer
    aiopti_adam_f32_t adam_opti;
    aiopti_sgd_f32_t sgd_opti;

    switch(AIFES_E_fnn_training->optimizer){
        case AIfES_E_adam:
            adam_opti.learning_rate = AIFES_E_fnn_training->learn_rate;
            adam_opti.beta1 = 0.9f;
            adam_opti.beta2 = 0.999f;
            adam_opti.eps = 1e-7;
            optimizer = aiopti_adam_f32_default(&adam_opti);
            break;
        case AIfES_E_sgd:
            sgd_opti.learning_rate = AIFES_E_fnn_training->learn_rate;
            sgd_opti.momentum = AIFES_E_fnn_training->sgd_momentum;
            optimizer = aiopti_sgd_f32_default(&sgd_opti);
            break;
        default :
            //printf("ERROR! Unknown optimizer\n" );
            return(-12);
    }

    // -------------------------------- Allocate and schedule the working memory for training ---------

    // Allocate memory for training and temporal data
    uint32_t memory_size = aialgo_sizeof_training_memory(&model, optimizer);

    void *memory_ptr = malloc(memory_size);

    if(memory_ptr == NULL)
    {
        //printf("ERROR! Not enough memory\n" );
        return(-13);
    }

    // Schedule the memory over the model
    aialgo_schedule_training_memory(&model, optimizer, memory_ptr, memory_size);

    // Initialize the AIfES model
    aialgo_init_model_for_training(&model, optimizer);

    // ------------------------------------- Run the training ------------------------------------

    float loss = 0;

    for(i = 0; i < AIFES_E_fnn_training->epochs; i++)
    {
        // One epoch of training. Iterates through the whole data once
        aialgo_train_model(&model, input_tensor, target_tensor, optimizer, AIFES_E_fnn_training->batch_size);

        // Calculate and print loss every print_interval epochs
        if(i % AIFES_E_fnn_training->epochs_loss_print_interval == 0)
        {

            aialgo_calc_loss_model_f32(&model, input_tensor, target_tensor, &loss);

            //Print the loss with the user function
            switch(AIFES_E_fnn_training->loss){
            case AIfES_E_mse:
                // loss = loss / (NUMBER_OF_OUTPUT_NEURONS * NUMBER_DATASETS)
                loss = loss / (AIFES_E_fnn->fnn_structure[AIFES_E_fnn->layer_count - 1] * input_tensor->shape[0]);
                (*AIFES_E_fnn_training->loss_print_function)(loss);
                if(AIFES_E_fnn_training->early_stopping == AIfES_E_early_stopping_on)
                {
                    if(loss <= AIFES_E_fnn_training->early_stopping_target_loss)
                    {
                            free(memory_ptr);
                            return(0);
                    }
                }

                break;
            case AIfES_E_crossentropy:
                // loss = loss / NUMBER_DATASETS
                loss = loss / input_tensor->shape[0];
                (*AIFES_E_fnn_training->loss_print_function)(loss);

                if(AIFES_E_fnn_training->early_stopping == AIfES_E_early_stopping_on)
                {
                    if(loss <= AIFES_E_fnn_training->early_stopping_target_loss)
                    {
                            free(memory_ptr);
                            return(0);
                    }
                }
                break;
            default :
                //printf("ERROR! Unknown loss function\n" );
                return(-10);
            }



        }
    }

    // Do the inference after training
    //aialgo_inference_model(&model, input_tensor, output_tensor);

    //Free the inference memory
    free(memory_ptr);

    return(0);
}


