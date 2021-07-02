
  /*
  www.aifes.ai
  https://github.com/Fraunhofer-IMS/AIfES_for_Arduino
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

  AIfES XOR Inference demo
  --------------------

  Versions:
    1.0.0   Initial version
  
  The sketch shows an example of how the inference of an already trained network is performed. 
  In the concrete example, a neural network was trained to map an XOR gate. 
  The neural network was trained in Keras and the configuration including the weights was imported into AIfES. 
  The network structure is 2-3(Sigmoid)-1(Sigmoid) and Sigmoid is used as activation function.
  The calculation is done in float 32.
  
  XOR truth table
  Input    Output
  0   0    0
  0   1    1
  1   0    1
  1   1    0
  
  Input data in the example (0.0 | 1.0)
  
  
  Tested on:
    Arduino UNO
    Arduino Nano 33 BLE Sense
   
*/

//Serial keyword: "inference"

#include <aifes.h>                  // include the AIfES libary

void setup() {
  Serial.begin(115200); //115200 baud rate (If necessary, change in the serial monitor)
  while (!Serial);

  delay(100);
  Serial.println(F("AIfES XOR inference demo"));
  Serial.println(F("Type >inference< to start"));
}

void loop() {

  while(Serial.available() > 0 ){
    String str = Serial.readString();
    if(str.indexOf("inference") > -1){        // Keyword "inference"

          Serial.println(F("AIfES"));
          Serial.println(F(""));
          
          //Tensor for the input data
          float input_data[] = {0.0f, 1.0f};    // Input data for the XOR ANN (0.0 / 1.0) 
          uint16_t input_shape[] = {1, 2};      // Definition of the input shape
          aitensor_t input_tensor;              // Creation of the input AIfES tensor
          input_tensor.dtype = aif32;           // Definition of the used data type, here float with 32 bits, different ones are available
          input_tensor.dim = 2;                 // Dimensions of the tensor, here 2 dimensions, as specified at input_shape
          input_tensor.shape = input_shape;     // Set the shape of the input_tensor
          input_tensor.data = input_data;       // Assign the input_data array to the tensor. It expects a pointer to the array where the data is stored

          // ---------------------------------- Layer definition ---------------------------------------
        
          // Input layer
          uint16_t input_layer_shape[] = {1, 2};      // Definition of the input layer shape (Must fit to the input tensor)
          ailayer_input_t input_layer;                // Creation of the AIfES input layer
          input_layer.input_dim = 2;                  // Definition of the input dimension (Must fit to the input tensor)
          input_layer.input_shape = input_layer_shape;// Handover of the input layer shape

          // Dense layer (hidden layer)
          float weights_data_dense_1[] = {-10.1164f, -8.4212f, 5.4396f, 7.297f, -7.6482f, -9.0155f}; //Hidden layer weights 
          float bias_data_dense_1[] = {-2.9653f,  2.3677f, -1.5968f}; //Hidden layer bias weights 
          ailayer_dense_t dense_layer_1; // Creation of the AIfES hidden dense layer
          dense_layer_1.neurons = 3;     // Number of neurons
          dense_layer_1.weights.data = weights_data_dense_1; // Passing the hidden layer weights
          dense_layer_1.bias.data = bias_data_dense_1;       // Passing the hidden layer bias weights


          // Sigmoid activation function
          ailayer_sigmoid_t sigmoid_layer_1;
        
          // Output dense layer
          float weights_data_dense_2[] = {12.0305f, -6.5858f, 11.9371f}; //Output dense layer weights
          float bias_data_dense_2[] = {-5.4247f}; //Output dense layer bias weights
          ailayer_dense_t dense_layer_2; // Creation of the AIfES ouput dense layer
          dense_layer_2.neurons = 1; // Number of neurons
          dense_layer_2.weights.data = weights_data_dense_2; // Passing the output layer weights
          dense_layer_2.bias.data = bias_data_dense_2;  // Passing the output layer bias weights
          
          // Sigmoid activation function
          ailayer_sigmoid_t sigmoid_layer_2;
        
          // --------------------------- Define the structure of the model ----------------------------
        
          aimodel_t model;  // AIfES model
          ailayer_t *x;     // Layer object from AIfES, contains the layers

          // Passing the layers to the AIfES model
          model.input_layer = ailayer_input_f32_default(&input_layer);
          x = ailayer_dense_f32_default(&dense_layer_1, model.input_layer);
          x = ailayer_sigmoid_f32_default(&sigmoid_layer_1, x);
          x = ailayer_dense_f32_default(&dense_layer_2, x);
          model.output_layer = ailayer_sigmoid_f32_default(&sigmoid_layer_2, x);
        
          aialgo_compile_model(&model); // Compile the AIfES model
        
          // -------------------------------- Allocate and schedule the working memory for inference ---------
        
          // Allocate memory for result and temporal data
          uint32_t memory_size = aialgo_sizeof_inference_memory(&model);
          Serial.print(F("Required memory for intermediate results: "));
          Serial.print(memory_size);
          Serial.print(F("Byte"));
          Serial.println(F(""));
          void *memory_ptr = malloc(memory_size);
          // Here is an alternative if no "malloc" should be used
          // Do not forget to comment out the "free(memory_ptr);" at the end if you use this solution
          //byte memory_ptr[memory_size];
        
          // Schedule the memory over the model
          aialgo_schedule_inference_memory(&model, memory_ptr, memory_size);
        
          // ------------------------------------- Run the inference ------------------------------------
        
          aitensor_t *output_tensor; // Creation of the AIfES ouput Tensor. The dimension etc. is created automatically
          output_tensor = aialgo_forward_model(&model, &input_tensor); // Forward pass (inference)
        
          // ------------------------------------- Print result ------------------------------------

          Serial.println(F(""));
          Serial.println(F("Results:"));
          Serial.println(F("input 1:\tinput 2:\treal output:\tcalculated output:"));
          Serial.print (input_data[0]);
          Serial.print (F("\t\t"));
          Serial.print (input_data[1]);
          Serial.print (F("\t\t"));
          Serial.print (F("1.0"));
          Serial.print (F("\t\t"));
          Serial.println(((float* ) output_tensor->data)[0]);
        
          //How to print the weights example
          //Serial.println(((float *) dense_layer_1.weights.data)[0]);
          //Serial.println(((float *) dense_layer_1.bias.data)[0]);

          free(memory_ptr);

          Serial.println(F(""));
          Serial.println(F("Type >inference< to restart"));
      }

    else{
      Serial.println(F("unknown"));
    }
  }


}
