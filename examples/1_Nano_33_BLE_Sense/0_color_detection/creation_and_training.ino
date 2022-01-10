/*
  www.aifes.ai
  Copyright (C) 2020-2021 Fraunhofer Institute for Microelectronic Circuits and Systems. All rights reserved.

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
*/

// This file creates the ANN and trains it

// ---------------------------------- AIfES model ---------------------------------------
// This part of the file contains the global defined model so it is accessible from the setup() and loop() function.

aimodel_t model;                                    // AIfES model
ailayer_t *x;                                       // Layer object from AIfES, contains the layers

void *parameter_memory = NULL;                      // Pointer to the memory stack of the AIfES model

uint16_t input_layer_shape[] = {1, 3};              // 3 Inputs RGB, Definition of the shape of the input. Each training data set contains 3 different values (RGB). Therefore, the input shape is (1,3)
ailayer_input_t input_layer;                        // Definition of the AIfES input layer

ailayer_dense_t hidden_layer_1;                     // Definition of the dense hidden layer
ailayer_sigmoid_t hidden_layer_1_activation;        // Definition of the hidden layer activation function (sigmoid)

ailayer_dense_t output_layer;                       // Definition of the output layer

#ifdef USE_SOFTMAX
  // This part is for the softmax activation function on the output layer and the cross-entropy loss function
  ailayer_softmax_t output_layer_activation_softmax;  // Definition of the hidden layer activation function (softmax)
  ailoss_crossentropy_t crossentropy_loss;            // Loss: cross-entropy / for softmax output activation function
#else
  // This part is for the sigmoid activation function on the output layer and the mean squared error loss function
  ailayer_sigmoid_t output_layer_activation;          // alternative Definition of the hidden layer activation function (sigmoid)
  ailoss_mse_t mse_loss;                              // Loss: mean square error / for sigmoid output activation function
#endif

void build_AIfES_model() {
  // The model is being build in this function. First of all, the layers (input, hidden and output layer) need to be initialized.
  // Then the storage for the parameters of the network is reserved and distributed to the layers.

  // ---------------------------------- Layer definition ---------------------------------------
  input_layer.input_dim = 2;                        // Definition of the input dimension, here a 2 dimensional input layer is selected
  input_layer.input_shape = input_layer_shape;      // Definition of the input shape, here a (1,3) layout is necessary, because each sample consists of 3 RGB values

  hidden_layer_1.neurons = 6;                       // Number of neurons in the dense hidden layer, here: 6 neurons

  output_layer.neurons = 3;                         // Number of neurons in the output layer, here: 3 neurons, for each object one

  // --------------------------- Define the structure of the model ----------------------------
  // Here the order of the layers is defined.
  // First of all, the input layer needs to be added to the model
  model.input_layer = ailayer_input_f32_default(&input_layer);

  // Then the dense hidden layer is added, it needs the corresponding layer (i.e. &hidden_layer_1) and the previous layer (here model.input_layer).
  // It returns the successfully initialized layer structure.
  x = ailayer_dense_f32_default(&hidden_layer_1, model.input_layer);
  // Then the activation function of the hidden layer is added to the model.
  // It is implemented as an additional layer, it needs the corresponding layer (i.e. &hidden_layer_1_activation) and the previous layer (here x, i.e. hidden_layer_1)
  x = ailayer_sigmoid_f32_default(&hidden_layer_1_activation, x);

  // Finally, the output layer needs to be added to the model
  // It follow the same semantics as the other layers
  x = ailayer_dense_f32_default(&output_layer, x);

  //The loss is selected depending on the activation function of the output layer
  #ifdef USE_SOFTMAX
    // When the softmax activation function is chosen, then the loss for the training of the model is the cross-entropy loss
  
    // Add the softmax activation function to the output layer
    // It is implemented as an additional layer, it needs the corresponding layer (i.e. &output_layer_activation_softmax) and the previous layer (here x, i.e. output_layer)
    x = ailayer_softmax_f32_default(&output_layer_activation_softmax, x);
  
    // Assign the output layer to the model, and therefore also the previous layers (i.e. hidden layer)
    model.output_layer = x;
  
    // Define the loss function for the training of the ANN
    // It needs the loss model (i.e. &crossentropy_loss) and the output layer of the model (i.e. model.output_layer)
    model.loss = ailoss_crossentropy_f32_default(&crossentropy_loss, model.output_layer);
  #else
    // When the softmax activation function is not chosen, then the sigmoid activation function is used. The loss is the mean square error
  
    // Add the sigmoid activation function to the output layer
    // It is implemented as an additional layer, it needs the corresponding layer (i.e. &output_layer_activation) and the previous layer (here x, i.e. output_layer)
    x = ailayer_sigmoid_f32_default(&output_layer_activation, x);
  
    // Assign the output layer to the model, and therefore also the previous layers (i.e. hidden layer)
    model.output_layer = x;
  
    // Define the loss function for the training of the ANN
    // It needs the loss model (i.e. &mse_loss) and the output layer of the model (i.e. model.output_layer)
    model.loss = ailoss_mse_f32_default(&mse_loss, model.output_layer);
  #endif

  // Generating the model with the specified structure of the layers
  // Counts the number of layers and trainable parameters in a model as preparation for inference or training.
  aialgo_compile_model(&model);

  // ------------------------------- Allocate memory for the parameters of the model ------------------------------
  // Calculate the memory requirements for the trainable parameters (like weights, bias, ...) of the model.
  uint32_t parameter_memory_size = aialgo_sizeof_parameter_memory(&model);

  // Output of the calculated memory size
  Serial.print(F("Required memory for parameter (Weights, Bias, ...):"));
  Serial.print(parameter_memory_size);
  Serial.println(F("Byte"));

  // Reserve the necessary memory stack
  parameter_memory = malloc(parameter_memory_size);

  // Check if malloc was successful
  if (parameter_memory == NULL) {
    while (true) {
      Serial.println(F("Not enough RAM available. Reduce the number of samples per object and flash the example again."));
      delay(1000);
    }
  }

  // Assign the memory for the trainable parameters (like weights, bias, ...) of the model.
  aialgo_distribute_parameter_memory(&model, parameter_memory, parameter_memory_size);
}


void train_AIfES_model() {
  // In this function the model is trained with the captured training data

  uint32_t i;                                               // Counting variable

  // -------------------------------- Create tensors needed for training ---------------------
  // Create the input tensor for training, contains all samples
  uint16_t input_shape[] = {NUMBER_OF_DATA, 3};             // Definition of the shape of the tensor, here: {# of total samples (i.e. samples per object * 3 objects), 3 (i.e. for each sample we have 3 RGB values)}
  aitensor_t input_tensor;                                  // Creation of the input AIfES tensor
  input_tensor.dtype = aif32;                               // Definition of the used data type, here float with 32 bits, different ones are available
  input_tensor.dim = 2;                                     // Dimensions of the tensor, here 2 dimensions, as specified at input_shape
  input_tensor.shape = input_shape;                         // Set the shape of the input_tensor
  input_tensor.data = training_data;                        // Assign the training_data array to the tensor. It expects a pointer to the array where the data is stored

  // Create the target tensor for training, contains the desired output for the corresponding sample to train the ANN
  uint16_t target_shape[] = {NUMBER_OF_DATA, 3};            // Definition of the shape of the tensor, here: {# of total samples (i.e. samples per object * 3 objects), 3 (i.e. for each sample we have 3 possible output classes)}
  aitensor_t target_tensor;                                 // Creation of the target AIfES tensor
  target_tensor.dtype = aif32;                              // Definition of the used data type, here float with 32 bits, different ones are available
  target_tensor.dim = 2;                                    // Dimensions of the tensor, here 2 dimensions, as specified at target_shape
  target_tensor.shape = target_shape;                       // Set the shape of the target tensor
  target_tensor.data = labels;                              // Assign the labels array to the tensor. It expects a pointer to the array where the data is stored

  // Create an output tensor for training, here the results of the ANN are saved and compared to the target tensor during training
  float output_data[NUMBER_OF_DATA][3];                     // Array for storage of the output data
  uint16_t output_shape[] = {NUMBER_OF_DATA, 3};            // Definition of the shape of the tensor, here: {# of total samples (i.e. samples per object * 3 objects), 3 (i.e. for each sample we have 3 possible output classes)}
  aitensor_t output_tensor;                                 // Creation of the target AIfES tensor
  output_tensor.dtype = aif32;                              // Definition of the used data type, here float with 32 bits, different ones are available
  output_tensor.dim = 2;                                    // Dimensions of the tensor, here 2 dimensions, as specified at output_shape
  output_tensor.shape = output_shape;                       // Set the shape of the output tensor
  output_tensor.data = output_data;                         // Assign the output_data array to the tensor. It expects a pointer to the array where the data is stored

  // -------------------------------- Initialize the weights and bias of the layers ---------------------
  // Here the weights and biases of the hidden and output layer are randomly initialized from a uniform distribution within the given range
  float from = -2.0;
  float to = 2.0;
  aimath_f32_default_tensor_init_uniform(&hidden_layer_1.weights, from, to);
  aimath_f32_default_tensor_init_uniform(&hidden_layer_1.bias, from, to);
  aimath_f32_default_tensor_init_uniform(&output_layer.weights, from, to);
  aimath_f32_default_tensor_init_uniform(&output_layer.bias, from, to);

  // -------------------------------- Define the optimizer for training ---------------------
  // Definition of the pointer towards the optimizer, which helps to optimize the learning process of the ANN
  aiopti_t *optimizer;

  // ADAM optimizer
  aiopti_adam_f32_t adam_opti;                              // Creation of the ADAM optimizer, other optimizers are available
  // Set parameters of the ADAM optimizer
  adam_opti.learning_rate = 0.1f;
  adam_opti.beta1 = 0.9f;
  adam_opti.beta2 = 0.999f;
  adam_opti.eps = 1e-7;

  // Create the optimizer with the specified parameters
  optimizer = aiopti_adam_f32_default(&adam_opti);

  // -------------------------------- Allocate and schedule the working memory for training ---------
  // Calculate the necessary memory size to store intermediate results, gradients and momentums for training.
  // It needs the model and the optimizer as parameters
  uint32_t memory_size = aialgo_sizeof_training_memory(&model, optimizer);

  // Output of the calculated memory size
  Serial.print(F("Required memory for the training (Intermediate results, gradients, optimization memory):"));
  Serial.print(memory_size);
  Serial.print(F("Byte"));
  Serial.println(F(""));
  
  // Reserve the necessary memory stack
  void *memory_ptr = malloc(memory_size);

  // Check if malloc was successful
  if (memory_ptr == NULL) {
    while (true) {
      Serial.println(F("Not enough RAM available. Reduce the number of samples per Object and flash the example again."));
      delay(1000);
    }
  }

  // Schedule the memory over the model
  aialgo_schedule_training_memory(&model, optimizer, memory_ptr, memory_size);

  // Initialize model for training
  aialgo_init_model_for_training(&model, optimizer);


  // ------------------------------------- Run the training ------------------------------------
  float loss;                                            // Variable to store the loss of the model
  uint32_t batch_size = NUMBER_OF_DATA;                  // Setting the batch size, here: full batch
  uint16_t epochs = 300;                                 // Set the number of epochs for training
  uint16_t print_interval = 10;                          // Print every ten epochs the current loss

  Serial.println(F("Start training"));
  for (i = 0; i < epochs; i++) {
    // One epoch of training. Iterates through the whole data once
    aialgo_train_model(&model, &input_tensor, &target_tensor, optimizer, batch_size);

    // Calculate and print loss every print_interval epochs
    if (i % print_interval == 0) {
      // Calculate loss
      aialgo_calc_loss_model_f32(&model, &input_tensor, &target_tensor, &loss);
      // Output current epoch and loss
      Serial.print(F("Epoch: "));
      Serial.print(i);
      Serial.print(F(" Loss: "));
      Serial.print(loss);
      Serial.println(F(""));

    }
  }

  Serial.println(F("Finished training"));

  // ----------------------------------------- Evaluate the trained model --------------------------
  // Here the trained network is tested with the training data. The training data is used as input and the predicted result
  // of the ANN is shown along with the corresponding labels.

  // Run the inference with the trained AIfES model, i.e. predict the output from the training data with the use of the ANN
  // The function needs the trained model, the input_tensor with the input data and the output_tensor where the results are saved in the corresponding array
  aialgo_inference_model(&model, &input_tensor, &output_tensor);

  // Print the original labels and the predicted results
  Serial.println(F("Outputs:"));
  Serial.println(F("labels:\tObject 1\tObject 2\tObject 3\tcalculated:\tObject 1\tObject 2\tObject 3"));

  for (i = 0; i < NUMBER_OF_DATA; i++) {

    Serial.print(F("\t"));
    Serial.print (labels[i][0]);
    Serial.print (F("\t\t"));
    Serial.print (labels[i][1]);
    Serial.print (F("\t\t"));
    Serial.print (labels[i][2]);
    Serial.print (F("\t\t\t\t"));
    Serial.print (output_data[i][0]);
    Serial.print (F("\t\t"));
    Serial.print (output_data[i][1]);
    Serial.print (F("\t\t"));
    Serial.println (output_data[i][2]);

  }
}
