/*
  The sketch shows an example of how a neural network is retrained in AIfES after training in Keras.
  Afterwards, the retrained neural network is executed to check the accuracy of the network.
  This is a script to retrain and execute a neural network for the MNIST dataset on a PC.
  
  If you need help to execute the example look here: 
  https://create.arduino.cc/projecthub/aifes_team/how-to-use-aifes-on-a-pc-for-training-9ad5f8?ref=user&ref_id=1924948&offset=1
*/


#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>

#include "aifes.h"

#include "MNIST_training_data.h"
#include "MNIST_training_data_label.h"
#include "MNIST_test_data.h"
#include "MNIST_test_data_label.h"
#include "MNIST_weights.h"

#define INPUTS  784
#define NEURONS_1 10
#define NEURONS_2 10
#define OUTPUTS 10
#define NUM_TRAIN_DATA 30000
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

    uint16_t input_shape[] = {NUM_TRAIN_DATA, INPUTS};              // Definition of the input shape
    aitensor_t input_tensor;                                        // Creation of the input AIfES tensor
      input_tensor.dtype = aif32;                                   // Definition of the used data type, here float with 32 bits, different ones are available
      input_tensor.dim = 2;                                         // Dimensions of the tensor, here 2 dimensions, as specified at input_shape
      input_tensor.shape = input_shape;                             // Set the shape of the input_tensor
      input_tensor.data = MNIST_training_data;                      // Assign the input_data array to the tensor. It expects a pointer to the array where the data is stored


    uint16_t input_shape_test_data[] = {NUM_TEST_DATA, INPUTS};     // Definition of the input shape
    aitensor_t input_tensor_test_data;                              // Creation of the input AIfES tensor
      input_tensor_test_data.dtype = aif32;                         // Definition of the used data type, here float with 32 bits, different ones are available
      input_tensor_test_data.dim = 2;                               // Dimensions of the tensor, here 2 dimensions, as specified at input_shape
      input_tensor_test_data.shape = input_shape_test_data;         // Set the shape of the input_tensor
      input_tensor_test_data.data = MNIST_test_data;                // Assign the input_data array to the tensor. It expects a pointer to the array where the data is stored


    uint16_t target_shape[] = {NUM_TRAIN_DATA, OUTPUTS};            // Definition of the output shape
    aitensor_t target_tensor;                                       // Creation of the input AIfES tensor
      target_tensor.dtype = aif32;                                  // Definition of the used data type, here float with 32 bits, different ones are available
      target_tensor.dim = 2;                                        // Dimensions of the tensor, here 2 dimensions, as specified at input_shape
      target_tensor.shape = target_shape;                           // Set the shape of the input_tensor
      target_tensor.data = MNIST_training_data_label;               // Assign the target_data array to the tensor. It expects a pointer to the array where the data is stored


    uint16_t target_shape_test_data[] = {NUM_TEST_DATA, OUTPUTS};   // Definition of the output shape
    aitensor_t target_tensor_test_data;                             // Creation of the input AIfES tensor
      target_tensor_test_data.dtype = aif32;                        // Definition of the used data type, here float with 32 bits, different ones are available
      target_tensor_test_data.dim = 2;                              // Dimensions of the tensor, here 2 dimensions, as specified at input_shape
      target_tensor_test_data.shape = target_shape_test_data;       // Set the shape of the input_tensor
      target_tensor_test_data.data = MNIST_test_data_label;         // Assign the target_data array to the tensor. It expects a pointer to the array where the data is stored

    // Tensor for the output data (result after training).
    // Same configuration as for the target tensor
    float output_data[NUM_TRAIN_DATA][OUTPUTS];
    uint16_t output_shape[] = {NUM_TRAIN_DATA, OUTPUTS};
    aitensor_t output_tensor;
      output_tensor.dtype = aif32;
      output_tensor.dim = 2;
      output_tensor.shape = output_shape;
      output_tensor.data = output_data;

	// Tensor for the output test data (result of testing).
    // Same configuration as for the target tensor
    float output_test_data[NUM_TEST_DATA][OUTPUTS];
    uint16_t output_shape_test_data[] = {NUM_TEST_DATA, OUTPUTS};
    aitensor_t output_tensor_test_data;
      output_tensor_test_data.dtype = aif32;
      output_tensor_test_data.dim = 2;
      output_tensor_test_data.shape = output_shape_test_data;
      output_tensor_test_data.data = output_test_data;

    // ---------------------------------- Layer definition ---------------------------------------

    // Input layer
    uint16_t input_layer_shape[] = {1, INPUTS};                     // Definition of the input layer shape (Must fit to the input tensor)
    ailayer_input_t input_layer;                                    // Creation of the AIfES input layer
      input_layer.input_dim = 2;                                    // Definition of the input dimension (Must fit to the input tensor)
      input_layer.input_shape = input_layer_shape;                  // Handover of the input layer shape

    // Dense layer (hidden layer_1)
    ailayer_dense_t dense_layer_1;                                  // Creation of the AIfES hidden dense layer
    dense_layer_1.neurons = NEURONS_1;                                // Number of neurons
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


    ailoss_crossentropy_t crossentropy_loss;                        // Loss: Crossentropy

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

    // Add the loss to the AIfES model
    model.loss = ailoss_crossentropy_f32_default(&crossentropy_loss, model.output_layer);

    aialgo_compile_model(&model); // Compile the AIfES model

    // -------------------------------- Define the optimizer for training ---------------------

    aiopti_t *optimizer; // Object for the optimizer

    //ADAM optimizer
    aiopti_adam_f32_t adam_opti;
    adam_opti.learning_rate = 0.001f;
    adam_opti.beta1 = 0.9f;
    adam_opti.beta2 = 0.999f;
    adam_opti.eps = 1e-7;

    // Choose the optimizer
    optimizer = aiopti_adam_f32_default(&adam_opti);

    // -------------------------------- Allocate and schedule the working memory for training ---------

    uint32_t memory_size = aialgo_sizeof_training_memory(&model, optimizer);
    printf("Required memory for the training (Intermediate results, gradients, optimization memory): %d Byte\n", memory_size);

    void *memory_ptr = malloc(memory_size);

    // Schedule the memory over the model
    aialgo_schedule_training_memory(&model, optimizer, memory_ptr, memory_size);

    // Initialize the AIfES model
    aialgo_init_model_for_training(&model, optimizer);

    // --------------------------------- Result with imported keras weights ----------------------------------

    aialgo_inference_model(&model, &input_tensor_test_data, &output_tensor_test_data);
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
                    printf("%f",output_test_data[i][j]);
                    printf("\t");
                }
            }
        }
        printf("\n\n");
    }

    predict_model_acc(&training_acc, NUM_TEST_DATA, OUTPUTS, &MNIST_test_data_label, &output_test_data);
    printf("Model accuracy with imported keras weights: %.3f%%\n\n", training_acc);

    // ------------------------------------- Run the training ------------------------------------

    float loss_training;
    uint32_t batch_size = 200;
    uint16_t epochs = 50;
    uint16_t print_interval = 10;

    printf("\n");
    printf("Start training\n");
    for(i = 0; i < epochs; i++)
    {
        // One epoch of training. Iterates through the whole data once
        aialgo_train_model(&model, &input_tensor, &target_tensor, optimizer, batch_size);

        // Calculate and print loss every print_interval epochs
        if(i % print_interval == 0)
        {
          aialgo_calc_loss_model_f32(&model, &input_tensor, &target_tensor, &loss_training);

          //reduction / normalization of the loss
          loss_training = loss_training / (OUTPUTS * NUM_TRAIN_DATA);

          printf("Epoch: ");
          printf("%d",i);
          printf(" Loss: ");
          printf("%f\n",loss_training);

        }
    }
    printf("Finished training\n\n");

    // ----------------------------------------- Evaluate the trained model --------------------------


    float loss_test;

    // Do the inference after training
    aialgo_inference_model(&model, &input_tensor_test_data, &output_tensor_test_data);


    printf("After training:\n");
    printf("Results:\n");

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
                    printf("%f",output_test_data[i][j]);
                    printf("\t");
                }
            }
        }
        printf("\n");
        printf("\n");
    }

    predict_model_acc(&test_acc, NUM_TEST_DATA, OUTPUTS, &MNIST_test_data_label, &output_test_data);
    printf("Model accuracy after training: %.3f%%\n\n", test_acc);

    aialgo_calc_loss_model_f32(&model, &input_tensor_test_data, &target_tensor_test_data, &loss_test);

    //reduction / normalization of the loss
    loss_test = loss_test / (OUTPUTS * NUM_TRAIN_DATA);
    printf("Loss: %f\n",loss_test);

    
	
    free(memory_ptr);

	system("pause");

	return 0;
}
