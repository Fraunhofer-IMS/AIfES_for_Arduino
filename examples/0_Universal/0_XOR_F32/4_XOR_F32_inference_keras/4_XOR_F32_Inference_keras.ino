/*
  www.aifes.ai
  https://github.com/Fraunhofer-IMS/AIfES_for_Arduino
  Copyright (C) 2020-2022  Fraunhofer Institute for Microelectronic Circuits and Systems.
  All rights reserved.

  AIfES is free software: you can redistribute it and/or modify
  it under the terms of the GNU Affero General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Affero General Public License for more details.

  You should have received a copy of the GNU Affero General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.

  AIfES XOR inference keras demo
  --------------------

  Versions:
    1.0.0   Initial version
    1.1.0   Change tensor initialization for new AIfES version
  
  The sketch shows how to import the weights of an artificial neural network (ANN) trained in Keras into AIfES. 
  The Phython script "XOR_Inference_keras.py" can be found in the AIfES libarys folder of this sketch.
  Or here: https://github.com/Fraunhofer-IMS/AIfES_for_Arduino/tree/main/examples/0_Universal/0_XOR/4_XOR_Inference_keras 
  The script trains an ANN with the same network structure as in this AIfES example. 
  The weights calculated in Keras are printed in the script and can be easily pasted into this source code.
  The network structure is identical to the example sketch "0_XOR_Inference.ino"
  
  
  XOR truth table
  Input    Output
  0   0    0
  0   1    1
  1   0    1
  1   1    0
  
  Input data in the example (0.0 | 1.0)
  
  
  Tested on:
    Arduino UNO
    Arduino Nano
    Arduino Nano 33 BLE Sense (Cortex M4)
    Seeeduino XIAO (Cortex M0+)
    
 */

//Serial keyword: "inference"

#include <aifes.h>      // include the AIfES libary

 
void setup() {
  Serial.begin(115200); //115200 baud rate (If necessary, change in the serial monitor)
  while (!Serial);

  Serial.println(F("AIfES XOR inference keras demo"));
  Serial.println(F("Type >inference< to start"));
}

void loop() {

  while(Serial.available() > 0 ){
    String str = Serial.readString();
    if(str.indexOf("inference") > -1){        //Keyword "inference"
      Serial.println(F("AIfES:"));
      Serial.println();
      
      //Tensor for the input data
      float input_data[] = {0.0f, 1.0f};                                        // Input data for the XOR ANN (0.0 / 1.0) 
      uint16_t input_shape[] = {1, 2};                                          // Definition of the input shape
      aitensor_t input_tensor = AITENSOR_2D_F32(input_shape, input_data);       // Creation of the input AIfES tensor with two dimensions and data type F32 (float32)

      // ---------------------------------- Layer definition ---------------------------------------
      
      // Input layer
      uint16_t input_layer_shape[] = {1, 2};      // Definition of the input layer shape (Must fit to the input tensor. The first dimension is the batch size.)
      ailayer_input_f32_t input_layer = AILAYER_INPUT_F32_M( /*input dimension=*/ 2, /*input shape=*/ input_layer_shape);   // Creation of the AIfES input layer

      // Dense layer (hidden layer)
      // These weights are calculated in the Keras script. Please insert the new weights for the first dense layer here:
      //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      float weights_data_dense_1[] = {
        -6.7788754f,
        6.0988016f,
        -6.119846f,
        1.9992247f,
        -7.746908f,
        -7.292215f
      };
      
      float bias_data_dense_1[] = {
        0.62134665f,
        -2.8388352f,
        2.2663612f
      };
      //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      ailayer_dense_f32_t dense_layer_1 = AILAYER_DENSE_F32_M( /*neurons=*/ 3, /*weights=*/ weights_data_dense_1, /*bias=*/ bias_data_dense_1); // Creation of the AIfES hidden dense layer with 3 neurons

      // Hidden layer activation function
      ailayer_sigmoid_f32_t sigmoid_layer_1 = AILAYER_SIGMOID_F32_M();
    
      // Output dense layer
      // These weights are calculated in the Keras script. Please insert the new weights for the second dense layer here:
      //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      float weights_data_dense_2[] = {
        5.1256347f,
        5.155546f,
        -5.0949035f
      };
      
      float bias_data_dense_2[] = {
        -2.282636f
      };
      //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      ailayer_dense_f32_t dense_layer_2 = AILAYER_DENSE_F32_M( /*neurons=*/ 1, /*weights=*/ weights_data_dense_2, /*bias=*/ bias_data_dense_2); // Creation of the AIfES output dense layer with 1 neuron
      
      // Output layer activation function
      ailayer_sigmoid_f32_t sigmoid_layer_2 = AILAYER_SIGMOID_F32_M();
    
      // --------------------------- Define the structure of the model ----------------------------
    
      aimodel_t model;  // AIfES model
      ailayer_t *x;     // Layer object from AIfES to connect the layers

      // Connect the layers to an AIfES model
      model.input_layer = ailayer_input_f32_default(&input_layer);
      x = ailayer_dense_f32_default(&dense_layer_1, model.input_layer);
      x = ailayer_sigmoid_f32_default(&sigmoid_layer_1, x);
      x = ailayer_dense_f32_default(&dense_layer_2, x);
      model.output_layer = ailayer_sigmoid_f32_default(&sigmoid_layer_2, x);
    
      aialgo_compile_model(&model); // Compile the AIfES model
      
      // ------------------------------------- Print the model structure ------------------------------------
      
      Serial.println(F("-------------- Model structure ---------------"));
      aialgo_print_model_structure(&model);
      Serial.println(F("----------------------------------------------\n"));
    
      // -------------------------------- Allocate and schedule the working memory for inference ---------
    
      // Allocate memory for result and temporal data
      uint32_t memory_size = aialgo_sizeof_inference_memory(&model);
      Serial.print(F("Required memory for intermediate results: "));
      Serial.print(memory_size);
      Serial.print(F(" bytes"));
      Serial.println();
      byte *memory_ptr = (byte *) malloc(memory_size);
      // Here is an alternative if no "malloc" should be used
      // Do not forget to comment out the "free(memory_ptr);" at the end if you use this solution
      // byte memory_ptr[memory_size];
    
      // Schedule the memory over the model
      aialgo_schedule_inference_memory(&model, memory_ptr, memory_size);
    
      // ------------------------------------- Run the inference ------------------------------------

      // Create an empty output tensor for the inference result
      uint16_t output_shape[2] = {1, 1};
      float output_data[1*1];                 // Empty data array of size output_shape
      aitensor_t output_tensor = AITENSOR_2D_F32(output_shape, output_data);
      
      aialgo_inference_model(&model, &input_tensor, &output_tensor); // Inference / forward pass
      
      // ------------------------------------- Print result ------------------------------------
      
      Serial.println(F(""));
      Serial.println(F("Results:"));
      Serial.println(F("input 1:\tinput 2:\treal output:\tcalculated output:"));
      Serial.print (input_data[0]);
      //Serial.print(((float* ) input_tensor.data)[0]); //Alternative print for the tensor
      Serial.print (F("\t\t"));
      Serial.print (input_data[1]);
      Serial.print (F("\t\t"));
      Serial.print (F("1.0"));
      Serial.print (F("\t\t"));
      Serial.println(((float *) output_tensor.data)[0]);
    
      // How to print the weights example
      // Serial.println(F("Dense 1 - Weights:")));
      // print_aitensor(&dense_layer_1.weights);
      // Serial.println(F("Dense 1 - Bias:"));
      // print_aitensor(&dense_layer_1.bias);
      
      free(memory_ptr);
      
      Serial.println(F(""));
      Serial.println(F("Type >inference< to restart"));
    } else {
      Serial.println(F("unknown"));
    }
  }


}
