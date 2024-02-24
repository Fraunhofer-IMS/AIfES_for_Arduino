/*
  Author: Hahn-Schickard-Gesellschaft für angewandte Forschung e.V., Daniel Konegen + Marcus Rueb

  The sketch shows an example of how a neural network is executed in AIfES after training in Keras.
  This is a script to execute and show the accuracy of a neural network for the MNIST dataset on a PC.
  
  If you need help to execute the example look here: 
  https://create.arduino.cc/projecthub/aifes_team/how-to-use-aifes-on-a-pc-for-training-9ad5f8?ref=user&ref_id=1924948&offset=1
*/


#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>

#include "aifes.h"

#include "MNIST_test_data.h"
#include "MNIST_test_data_label.h"
#include "MNIST_weights.h"

#define INPUTS  784
#define NEURONS_1 10
#define NEURONS_2 10
#define OUTPUTS 10
#define NUM_TEST_DATA 10000


void predict_model_acc(float *model_acc, int num_data, int num_output_neurons, float *data_label, float *model_pred){
	/*
	Calculates the accuracy of the neural network
	*/	
	
	float argmax_target_acc;
    uint8_t argmax_target_index;
    float argmax_pred_acc;
    uint8_t argmax_pred_index;
    *model_acc = 0;

    for(int i=0;i<num_data;i++) {
        argmax_target_acc = 0;
        argmax_target_index = 0;
        argmax_pred_acc = 0;
        argmax_pred_index = 0;

        for(int j=0;j<num_output_neurons;j++) {
            if(*((data_label+i*num_output_neurons)+j) > argmax_target_acc) {
                argmax_target_acc = *((data_label+i*num_output_neurons)+j);
                argmax_target_index = j;
            }
            if(*((model_pred+i*num_output_neurons)+j) > argmax_pred_acc) {
                argmax_pred_acc = *((model_pred+i*num_output_neurons)+j);
                argmax_pred_index = j;
            }
        }
        if(argmax_target_index == argmax_pred_index){
            (*model_acc)++;
        }
    }
    *model_acc = (*model_acc/num_data)*100;

}


int main(int argc, char *argv[])
{
	// loop variables
    uint32_t i;
    uint32_t j;
    uint32_t k;

    float training_acc;
    float test_acc;


    uint16_t input_shape[] = {NUM_TEST_DATA, INPUTS};               // Definition of the input shape
    aitensor_t input_tensor;                                        // Creation of the input AIfES tensor
      input_tensor.dtype = aif32;                                   // Definition of the used data type, here float with 32 bits, different ones are available
      input_tensor.dim = 2;                                         // Dimensions of the tensor, here 2 dimensions, as specified at input_shape
      input_tensor.shape = input_shape;                             // Set the shape of the input_tensor
      input_tensor.data = MNIST_test_data;                       	// Assign the input_data array to the tensor. It expects a pointer to the array where the data is stored


    uint16_t target_shape[] = {NUM_TEST_DATA, OUTPUTS};             // Definition of the output shape
    aitensor_t target_tensor;                                       // Creation of the input AIfES tensor
      target_tensor.dtype = aif32;                                  // Definition of the used data type, here float with 32 bits, different ones are available
      target_tensor.dim = 2;                                        // Dimensions of the tensor, here 2 dimensions, as specified at input_shape
      target_tensor.shape = target_shape;                           // Set the shape of the input_tensor
      target_tensor.data = MNIST_test_data_label;                   // Assign the target_data array to the tensor. It expects a pointer to the array where the data is stored
	
	// Tensor for the output data (result after training).
    // Same configuration as for the target tensor
    float output_data[NUM_TEST_DATA][OUTPUTS];
    uint16_t output_shape[] = {NUM_TEST_DATA, OUTPUTS};
    aitensor_t output_tensor;
      output_tensor.dtype = aif32;
      output_tensor.dim = 2;
      output_tensor.shape = output_shape;
      output_tensor.data = output_data;

    // ---------------------------------- Layer definition ---------------------------------------

    // Input layer
    uint16_t input_layer_shape[] = {1, INPUTS};                     // Definition of the input layer shape (Must fit to the input tensor)
    ailayer_input_t input_layer;                                    // Creation of the AIfES input layer
      input_layer.input_dim = 2;                                    // Definition of the input dimension (Must fit to the input tensor)
      input_layer.input_shape = input_layer_shape;                  // Handover of the input layer shape

    // Dense layer (hidden layer_1)
    ailayer_dense_t dense_layer_1;                                  // Creation of the AIfES hidden dense layer
    dense_layer_1.neurons = NEURONS_1;                              // Number of neurons
    dense_layer_1.weights.data = weights_data_dense_1;              // Passing the hidden layer weights
    dense_layer_1.bias.data = bias_data_dense_1;                    // Passing the hidden layer bias weights

    ailayer_relu_t relu_layer_1;                                    // Relu activation function

    // Dense layer (hidden layer_2)
    ailayer_dense_t dense_layer_2;                                  // Creation of the AIfES hidden dense layer
    dense_layer_2.neurons = NEURONS_2;                              // Number of neurons
    dense_layer_2.weights.data = weights_data_dense_2;              // Passing the hidden layer weights
    dense_layer_2.bias.data = bias_data_dense_2;                    // Passing the hidden layer bias weights

    ailayer_relu_t relu_layer_2;                                    // Relu activation function

    // Output dense layer
    ailayer_dense_t dense_layer_3;                                  // Creation of the AIfES ouput dense layer
    dense_layer_3.neurons = OUTPUTS;                                // Number of neurons
    dense_layer_3.weights.data = weights_data_dense_3;              // Passing the output layer weights
    dense_layer_3.bias.data = bias_data_dense_3;                    // Passing the output layer bias weights

    ailayer_softmax_t softmax_layer_3;                              // Softmax activation function


    // --------------------------- Define the structure of the model ----------------------------

    aimodel_t model;                                                // AIfES model
    ailayer_t *x;                                                   // Layer object from AIfES, contains the layers

    // Passing the layers to the AIfES model
    model.input_layer = ailayer_input_f32_default(&input_layer);
    x = ailayer_dense_f32_default(&dense_layer_1, model.input_layer);
    x = ailayer_relu_f32_default(&relu_layer_1, x);
    x = ailayer_dense_f32_default(&dense_layer_2, x);
    x = ailayer_relu_f32_default(&relu_layer_2, x);
    x = ailayer_dense_f32_default(&dense_layer_3, x);
    x = ailayer_softmax_f32_default(&softmax_layer_3, x);
    model.output_layer = x;

    aialgo_compile_model(&model); // Compile the AIfES model

    // -------------------------------- Allocate and schedule the working memory for training ---------

    uint32_t memory_size = aialgo_sizeof_inference_memory(&model);
    printf("Required memory for the training (Intermediate results, gradients, optimization memory): %d Byte\n", memory_size);

    void *memory_ptr = malloc(memory_size);

    // Schedule the memory over the model
    aialgo_schedule_inference_memory(&model, memory_ptr, memory_size);


    // --------------------------------- Result with imported keras weights ----------------------------------

    aialgo_inference_model(&model, &input_tensor, &output_tensor);
    printf("\n");
    printf("Results with imported weights from keras:\n");

    for (i = 0; i < 10; i++) {
        for(k = 0; k < 2; k++) {
            if(k==0){
                printf("real output %d:\n",i);
            }
            else {
                printf("calculated output %d:\n",i);
            }
            for(j = 0; j < 10; j++) {
                if(k==0){
                    printf("%f",MNIST_test_data_label[i][j]);
                    printf("\t");
                }
                else {
                    printf("%f",output_data[i][j]);
                    printf("\t");
                }
            }
        }
        printf("\n\n");
    }

    predict_model_acc(&training_acc, NUM_TEST_DATA, OUTPUTS, &MNIST_test_data_label, &output_data);
    printf("Model accuracy with imported keras weights: %.3f%%\n\n", training_acc);


    free(memory_ptr);

	system("pause");

	return 0;
}
