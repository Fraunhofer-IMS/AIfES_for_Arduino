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

#include "basic/express/aifes_express_q7_fnn.h"


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


uint32_t AIFES_E_flat_weights_number_fnn_q7(uint32_t *fnn_structure, uint32_t layer_count)
{
    int i;
    uint32_t count = 0;

    // Result tensor parameters
    count += layer_count * sizeof(aimath_q7_params_t);

    for(i = 1; i < layer_count; i++){
        AIFES_ALIGN_INTEGER(count, AIFES_MEMORY_ALIGNMENT);
        // Weights (int8)
        count += sizeof(aimath_q7_params_t); // Q params
        AIFES_ALIGN_INTEGER(count, AIFES_MEMORY_ALIGNMENT);
        count += 1 * fnn_structure[i-1] * fnn_structure[i]; // Data
        AIFES_ALIGN_INTEGER(count, AIFES_MEMORY_ALIGNMENT);
        // Bias (int32)
        count += sizeof(aimath_q31_params_t); // Q params
        AIFES_ALIGN_INTEGER(count, AIFES_MEMORY_ALIGNMENT);
        count += 4 * fnn_structure[i]; // Data
    }

    return count;
}

int8_t AIFES_E_quantisation_fnn_f32_to_q7(aitensor_t *representative_f32_dataset,
                                          AIFES_E_model_parameter_fnn_f32 *AIFES_E_fnn,
                                          unsigned char *q7_parameter_dataset)
{
    uint32_t i = 0;

    if(representative_f32_dataset->dtype != aif32)
    {
        //printf("ERROR! Tensor dtype\n");
        return(-1);
    }

    if(representative_f32_dataset->shape[1] != AIFES_E_fnn->fnn_structure[0])
    {
        //printf("ERROR! Input tensor shape does not correspond to ANN inputs\n");
        return(-2);
    }

    // Create Q7 Tensor
    // ---------------------------------------------------------
    int8_t *input_data_q7;

    input_data_q7 = (int8_t *) malloc(representative_f32_dataset->shape[0] * representative_f32_dataset->shape[1] * sizeof(int8_t));

    if(input_data_q7 == NULL)
    {
        //printf("ERROR! Not enough memory\n" );
        return(-3);
    }

	aimath_q7_params_t input_params;
	aitensor_t input_tensor_q7 = AITENSOR_2D_Q7(representative_f32_dataset->shape, &input_params, input_data_q7);

	// Get the min/max value of the representative_f32_dataset
	float min_value, max_value;
    aimath_f32_default_min(representative_f32_dataset, &min_value);
    aimath_f32_default_max(representative_f32_dataset, &max_value);
    // Calculate the q7 parameters
    aimath_q7_calc_q_params_from_f32(min_value, max_value, input_tensor_q7.tensor_params);
    // ---------------------------------------------------------

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
                return(-4);
       }
    }

    // F32 -----------------------------------------------------------------------------------------
    // Define the activation functions F32
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

    // Define the dense layer F32
    ailayer_dense_t         dense_layer[AIFES_E_fnn->layer_count - 1];


    // Q7 -----------------------------------------------------------------------------------------
    // Define the activation functions q7
    ailayer_relu_q7_t          relu_layer_q7[relu_count];
    ailayer_sigmoid_q7_t       sigmoid_layer_q7[sigmoid_count];
    ailayer_softmax_q7_t       softmax_layer_q7[softmax_count];
    ailayer_leaky_relu_q7_t    leaky_relu_layer_q7[leaky_relu_count]; // alpha=0.01
    ailayer_elu_q7_t           elu_layer_q7[elu_count];               // alpha=1.0
    ailayer_tanh_q7_t          tanh_layer_q7[tanh_count];
    ailayer_softsign_q7_t      softsign_layer_q7[softsign_count];


    // Init leaky relu with default alpha
    for (i = 0; i < leaky_relu_count; i++) {
        leaky_relu_layer_q7[i].alpha.shift = FLOAT_TO_Q7(0.01f,10,0);
        leaky_relu_layer_q7[i].alpha.value = 10;
        leaky_relu_layer_q7[i].alpha.zero_point = 0;
    }
    // Init elu with default alpha
    for (i = 0; i < elu_count; i++) {
        elu_layer_q7[i].alpha.shift = 0;
        elu_layer_q7[i].alpha.value = 1;
        elu_layer_q7[i].alpha.zero_point = 0;
    }

    // Define the dense layer q7
    ailayer_dense_t         dense_layer_q7[AIFES_E_fnn->layer_count - 1];

    // -----------------------------------------------------------------------------------------

    // Use the variables as counter
    relu_count = 0;
    sigmoid_count = 0;
    softmax_count = 0;
    leaky_relu_count = 0;
    elu_count = 0;
    tanh_count = 0;
    softsign_count = 0;
    linear_count = 0;


    // Input layer F32 and model
    uint16_t input_layer_shape[] = {1, AIFES_E_fnn->fnn_structure[0]};      // Definition of the input layer shape (Must fit to the input tensor)
    ailayer_input_t input_layer;                // Creation of the AIfES input layer
    input_layer.input_dim = 2;                  // Definition of the input dimension (Must fit to the input tensor)
    input_layer.input_shape = input_layer_shape;// Handover of the input layer shape

    aimodel_t model;  // AIfES model
    ailayer_t *x;     // Layer object from AIfES, contains the layers

    // Input layer q7 and model
    //uint16_t input_layer_shape[] = {1, AIFES_E_fnn->fnn_structure[0]};      // Definition of the input layer shape (Must fit to the input tensor)
    ailayer_input_t input_layer_q7;                // Creation of the AIfES input layer
    input_layer_q7.input_dim = 2;                  // Definition of the input dimension (Must fit to the input tensor)
    input_layer_q7.input_shape = input_layer_shape;// Handover of the input layer shape

    aimodel_t model_q7;  // AIfES model
    ailayer_t *x_q7;     // Layer object from AIfES, contains the layers

    // Generate the dense layer and the output layer
    for (i = 0; i < AIFES_E_fnn->layer_count - 1; i++) {

        if(i==0)
        {
             //F32
             // Input layer
             model.input_layer = ailayer_input_f32_default(&input_layer);
             // first dense layer
             dense_layer[i].neurons =  AIFES_E_fnn->fnn_structure[i+1];
             // Create first dense layer
             x = ailayer_dense_f32_default(&dense_layer[i], model.input_layer);
            // Q7
            // Input layer
             model_q7.input_layer = ailayer_input_q7_default(&input_layer_q7);
             // first dense layer
             dense_layer_q7[i].neurons =  AIFES_E_fnn->fnn_structure[i+1];
             // Create first dense layer
             x_q7 = ailayer_dense_q7_default(&dense_layer_q7[i], model_q7.input_layer);
        }
        else
        {
            dense_layer[i].neurons =  AIFES_E_fnn->fnn_structure[i+1];  // Don't use the input neurons
            x = ailayer_dense_f32_default(&dense_layer[i], x);

            dense_layer_q7[i].neurons =  AIFES_E_fnn->fnn_structure[i+1];  // Don't use the input neurons
            x_q7 = ailayer_dense_q7_default(&dense_layer_q7[i], x_q7);
        }

        switch(AIFES_E_fnn->fnn_activations[i]) {
            case AIfES_E_relu:
                if(i == AIFES_E_fnn->layer_count - 2){
                    model.output_layer = ailayer_relu_f32_default(&relu_layer[relu_count], x);  // Output layer
                    model_q7.output_layer = ailayer_relu_q7_default(&relu_layer_q7[relu_count], x_q7);  // Output layer
                }else{
                    x = ailayer_relu_f32_default(&relu_layer[relu_count], x);
                    x_q7 = ailayer_relu_q7_default(&relu_layer_q7[relu_count], x_q7);
                }
                relu_count ++;
                break;
            case AIfES_E_sigmoid:
                if(i == AIFES_E_fnn->layer_count - 2){
                    model.output_layer = ailayer_sigmoid_f32_default(&sigmoid_layer[sigmoid_count], x);
                    model_q7.output_layer = ailayer_sigmoid_q7_default(&sigmoid_layer_q7[sigmoid_count], x_q7);
                }else{
                    x = ailayer_sigmoid_f32_default(&sigmoid_layer[sigmoid_count], x);
                    x_q7 = ailayer_sigmoid_q7_default(&sigmoid_layer_q7[sigmoid_count], x_q7);
                }
                sigmoid_count ++;
                break;
            case AIfES_E_softmax:
                if(i == AIFES_E_fnn->layer_count - 2){
                    model.output_layer = ailayer_softmax_f32_default(&softmax_layer[softmax_count], x);
                    model_q7.output_layer = ailayer_softmax_q7_default(&softmax_layer_q7[softmax_count], x_q7);
                }else{
                    x = ailayer_softmax_f32_default(&softmax_layer[softmax_count], x);
                    x_q7 = ailayer_softmax_q7_default(&softmax_layer_q7[softmax_count], x_q7);
                }
                softmax_count ++;
                break;
            case AIfES_E_leaky_relu:
                if(i == AIFES_E_fnn->layer_count - 2){
                    model.output_layer = ailayer_leaky_relu_f32_default(&leaky_relu_layer[leaky_relu_count], x);
                    model_q7.output_layer = ailayer_leaky_relu_q7_default(&leaky_relu_layer_q7[leaky_relu_count], x_q7);
                }else{
                    x = ailayer_leaky_relu_f32_default(&leaky_relu_layer[leaky_relu_count], x);
                    x_q7 = ailayer_leaky_relu_q7_default(&leaky_relu_layer_q7[leaky_relu_count], x_q7);
                }
                leaky_relu_count ++;
                break;
            case AIfES_E_elu:
                if(i == AIFES_E_fnn->layer_count - 2){
                    model.output_layer = ailayer_elu_f32_default(&elu_layer[elu_count], x);
                    model_q7.output_layer = ailayer_elu_q7_default(&elu_layer_q7[elu_count], x_q7);
                }else{
                    x = ailayer_elu_f32_default(&elu_layer[elu_count], x);
                    x_q7 = ailayer_elu_q7_default(&elu_layer_q7[elu_count], x_q7);
                }
                elu_count ++;
                break;
            case AIfES_E_tanh:
                if(i == AIFES_E_fnn->layer_count - 2){
                    model.output_layer = ailayer_tanh_f32_default(&tanh_layer[tanh_count], x);
                    model_q7.output_layer = ailayer_tanh_q7_default(&tanh_layer_q7[tanh_count], x_q7);
                }else{
                    x = ailayer_tanh_f32_default(&tanh_layer[tanh_count], x);
                    x_q7 = ailayer_tanh_q7_default(&tanh_layer_q7[tanh_count], x_q7);
                }
                tanh_count ++;
                break;
            case AIfES_E_softsign:
                if(i == AIFES_E_fnn->layer_count - 2){
                    model.output_layer = ailayer_softsign_f32_default(&softsign_layer[softsign_count], x);
                    model_q7.output_layer = ailayer_softsign_q7_default(&softsign_layer_q7[softsign_count], x_q7);
                }else{
                    x = ailayer_softsign_f32_default(&softsign_layer[softsign_count], x);
                    x_q7 = ailayer_softsign_q7_default(&softsign_layer_q7[softsign_count], x_q7);
                }
                softsign_count ++;
                break;
            case AIfES_E_linear:
                if(i == AIFES_E_fnn->layer_count - 2){
                    model.output_layer = x;
                    model_q7.output_layer = x_q7;
                }else{
                    //no activation needed
                }
                linear_count ++;
                break;

            default :
                //printf("ERROR! Unknown activation function\n" );
                return(-4);
        }

    }

    aialgo_compile_model(&model); // Compile the AIfES model

    // -------------------------------- Pass the weights to the model ----------------------------------
    //Calculate required memory for the weights in Byte
    uint32_t parameter_memory_size = aialgo_sizeof_parameter_memory(&model);

    // Distribute the weights over the model
    aialgo_distribute_parameter_memory(&model, (void *) AIFES_E_fnn->flat_weights, parameter_memory_size);

    // -------------------------------- Allocate and schedule the working memory for inference ---------

    aialgo_compile_model(&model_q7); // Compile the AIfES q7 model

    uint32_t parameter_memory_size_q7 = aialgo_sizeof_parameter_memory(&model_q7);
	//printf("Required memory for parameter of the q7 model (Weights, Biases, quantization parameters): %d Byte\n", parameter_memory_size_q7);

	void *parameter_memory_q7 = malloc(parameter_memory_size_q7);

    aialgo_distribute_parameter_memory(&model_q7, (void *) q7_parameter_dataset, parameter_memory_size_q7);

    // Allocate memory for result and temporal data
    uint32_t memory_size = aialgo_sizeof_inference_memory(&model);

    void *memory_ptr = malloc(memory_size);

    if(memory_ptr == NULL)
    {
        //printf("ERROR! Not enough memory\n" );
        return(-3);
    }

    // Schedule the memory over the model
    aialgo_schedule_inference_memory(&model, memory_ptr, memory_size);

    memory_size = aialgo_sizeof_inference_memory(&model_q7);

	memory_ptr = malloc(memory_size);

    if(memory_ptr == NULL)
    {
        //printf("ERROR! Not enough memory\n" );
        return(-3);
    }

	// Schedule the memory over the model
	aialgo_schedule_inference_memory(&model_q7, memory_ptr, memory_size);

    aialgo_quantize_model_f32_to_q7(&model, &model_q7, representative_f32_dataset);

    printf("\n");
    printf("TEST2\n");
    for(i = 0; i < parameter_memory_size_q7; i++)
    {
        //printf("%d\n",((uint8_t *) parameter_memory_q7)[i]);
        printf("%d\n",q7_parameter_dataset[i]);
    }


    //Free Q7 data memory
    free(input_data_q7);

    free(parameter_memory_q7);

    //Free the inference memory
    free(memory_ptr);


    return(0);
}

int8_t AIFES_E_inference_fnn_q7(aitensor_t *input_tensor,
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

    // ---------------------------------------------------------

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



    // Q7 -----------------------------------------------------------------------------------------
    // Define the activation functions q7
    ailayer_relu_q7_t          relu_layer_q7[relu_count];
    ailayer_sigmoid_q7_t       sigmoid_layer_q7[sigmoid_count];
    ailayer_softmax_q7_t       softmax_layer_q7[softmax_count];
    ailayer_leaky_relu_q7_t    leaky_relu_layer_q7[leaky_relu_count]; // alpha=0.01
    ailayer_elu_q7_t           elu_layer_q7[elu_count];               // alpha=1.0
    ailayer_tanh_q7_t          tanh_layer_q7[tanh_count];
    ailayer_softsign_q7_t      softsign_layer_q7[softsign_count];


    // Init leaky relu with default alpha
    for (i = 0; i < leaky_relu_count; i++) {
        leaky_relu_layer_q7[i].alpha.shift = FLOAT_TO_Q7(0.01f,10,0);
        leaky_relu_layer_q7[i].alpha.value = 10;
        leaky_relu_layer_q7[i].alpha.zero_point = 0;
    }
    // Init elu with default alpha
    for (i = 0; i < elu_count; i++) {
        elu_layer_q7[i].alpha.shift = 0;
        elu_layer_q7[i].alpha.value = 1;
        elu_layer_q7[i].alpha.zero_point = 0;
    }

    // Define the dense layer q7
    ailayer_dense_t         dense_layer_q7[AIFES_E_fnn->layer_count - 1];

    // -----------------------------------------------------------------------------------------

    // Use the variables as counter
    relu_count = 0;
    sigmoid_count = 0;
    softmax_count = 0;
    leaky_relu_count = 0;
    elu_count = 0;
    tanh_count = 0;
    softsign_count = 0;
    linear_count = 0;

    // Input layer q7 and model
    uint16_t input_layer_shape[] = {1, AIFES_E_fnn->fnn_structure[0]};      // Definition of the input layer shape (Must fit to the input tensor)
    ailayer_input_t input_layer_q7;                // Creation of the AIfES input layer
    input_layer_q7.input_dim = 2;                  // Definition of the input dimension (Must fit to the input tensor)
    input_layer_q7.input_shape = input_layer_shape;// Handover of the input layer shape

    aimodel_t model_q7;  // AIfES model
    ailayer_t *x_q7;     // Layer object from AIfES, contains the layers

    // Generate the dense layer and the output layer
    for (i = 0; i < AIFES_E_fnn->layer_count - 1; i++) {

        if(i==0)
        {
            // Q7
            // Input layer
             model_q7.input_layer = ailayer_input_q7_default(&input_layer_q7);
             // first dense layer
             dense_layer_q7[i].neurons =  AIFES_E_fnn->fnn_structure[i+1];
             // Create first dense layer
             x_q7 = ailayer_dense_q7_default(&dense_layer_q7[i], model_q7.input_layer);
        }
        else
        {

            dense_layer_q7[i].neurons =  AIFES_E_fnn->fnn_structure[i+1];  // Don't use the input neurons
            x_q7 = ailayer_dense_q7_default(&dense_layer_q7[i], x_q7);
        }

        switch(AIFES_E_fnn->fnn_activations[i]) {
            case AIfES_E_relu:
                if(i == AIFES_E_fnn->layer_count - 2){
                    model_q7.output_layer = ailayer_relu_q7_default(&relu_layer_q7[relu_count], x_q7);  // Output layer
                }else{
                    x_q7 = ailayer_relu_q7_default(&relu_layer_q7[relu_count], x_q7);
                }
                relu_count ++;
                break;
            case AIfES_E_sigmoid:
                if(i == AIFES_E_fnn->layer_count - 2){
                    model_q7.output_layer = ailayer_sigmoid_q7_default(&sigmoid_layer_q7[sigmoid_count], x_q7);
                }else{
                    x_q7 = ailayer_sigmoid_q7_default(&sigmoid_layer_q7[sigmoid_count], x_q7);
                }
                sigmoid_count ++;
                break;
            case AIfES_E_softmax:
                if(i == AIFES_E_fnn->layer_count - 2){
                    model_q7.output_layer = ailayer_softmax_q7_default(&softmax_layer_q7[softmax_count], x_q7);
                }else{
                    x_q7 = ailayer_softmax_q7_default(&softmax_layer_q7[softmax_count], x_q7);
                }
                softmax_count ++;
                break;
            case AIfES_E_leaky_relu:
                if(i == AIFES_E_fnn->layer_count - 2){
                    model_q7.output_layer = ailayer_leaky_relu_q7_default(&leaky_relu_layer_q7[leaky_relu_count], x_q7);
                }else{
                    x_q7 = ailayer_leaky_relu_q7_default(&leaky_relu_layer_q7[leaky_relu_count], x_q7);
                }
                leaky_relu_count ++;
                break;
            case AIfES_E_elu:
                if(i == AIFES_E_fnn->layer_count - 2){
                    model_q7.output_layer = ailayer_elu_q7_default(&elu_layer_q7[elu_count], x_q7);
                }else{
                    x_q7 = ailayer_elu_q7_default(&elu_layer_q7[elu_count], x_q7);
                }
                elu_count ++;
                break;
            case AIfES_E_tanh:
                if(i == AIFES_E_fnn->layer_count - 2){
                    model_q7.output_layer = ailayer_tanh_q7_default(&tanh_layer_q7[tanh_count], x_q7);
                }else{
                    x_q7 = ailayer_tanh_q7_default(&tanh_layer_q7[tanh_count], x_q7);
                }
                tanh_count ++;
                break;
            case AIfES_E_softsign:
                if(i == AIFES_E_fnn->layer_count - 2){
                    model_q7.output_layer = ailayer_softsign_q7_default(&softsign_layer_q7[softsign_count], x_q7);
                }else{
                    x_q7 = ailayer_softsign_q7_default(&softsign_layer_q7[softsign_count], x_q7);
                }
                softsign_count ++;
                break;
            case AIfES_E_linear:
                if(i == AIFES_E_fnn->layer_count - 2){
                    model_q7.output_layer = x_q7;
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



    aialgo_compile_model(&model_q7);
    // -------------------------------- Pass the weights to the model ----------------------------------
    //Calculate required memory for the weights in Byte

    uint32_t parameter_memory_size_q7 = aialgo_sizeof_parameter_memory(&model_q7);
	//printf("Required memory for parameter of the q7 model (Weights, Biases, quantization parameters): %d Byte\n", parameter_memory_size_q7);

	// Distribute the weights over the model
    aialgo_distribute_parameter_memory(&model_q7, (void *) AIFES_E_fnn->flat_weights, parameter_memory_size_q7);


    // -------------------------------- Allocate and schedule the working memory for inference ---------
    // Allocate memory for result and temporal data
    uint32_t memory_size = aialgo_sizeof_inference_memory(&model_q7);

    void *memory_ptr = malloc(memory_size);

    if(memory_ptr == NULL)
    {
        //printf("ERROR! Not enough memory\n" );
        return(-6);
    }

	// Schedule the memory over the model
	aialgo_schedule_inference_memory(&model_q7, memory_ptr, memory_size);

    // Create Q7 Input Tensor
    // ---------------------------------------------------------
    int8_t *input_data_q7;

    input_data_q7 = (int8_t *) malloc(input_tensor->shape[0] * input_tensor->shape[1] * sizeof(int8_t));

    if(input_data_q7 == NULL)
    {
        //printf("ERROR! Not enough memory\n" );
        return(-6);
    }

	aitensor_t input_tensor_q7 = AITENSOR_2D_Q7(input_tensor->shape, input_layer_q7.base.result.tensor_params, input_data_q7);

    // Create Q7 Output Tensor
    int8_t *output_data_q7;

    output_data_q7 = (int8_t *) malloc(output_tensor->shape[0] * output_tensor->shape[1] * sizeof(int8_t));

    if(output_data_q7 == NULL)
    {
        //printf("ERROR! Not enough memory\n" );
        return(-6);
    }

	aimath_q7_params_t output_params;
	aitensor_t output_tensor_q7 = AITENSOR_2D_Q7(output_tensor->shape, &output_params, output_data_q7);

	// Quantize the F32 input tensor to q7
	aimath_q7_quantize_tensor_from_f32(input_tensor, &input_tensor_q7);

    //Do the inference and pass the result to the output_tensor
    aialgo_inference_model(&model_q7, &input_tensor_q7, &output_tensor_q7);

    // Convert the calculated Q7 output data to f32
    for (i = 0; i < output_tensor->shape[0] * output_tensor->shape[1]; i++) {
        ((float *) output_tensor->data)[i] = Q7_TO_FLOAT(output_data_q7[i], output_params.shift, output_params.zero_point);
    }


    //Free Q7 data memory
    free(input_data_q7);
    free(output_data_q7);

    //Free the inference memory
    free(memory_ptr);


    return(0);
}
