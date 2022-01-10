
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

  AIfES XOR Inference demo
  --------------------

  Versions:
    1.0.0   Initial version
    1.1.0   Change tensor initialization for new AIfES version
  
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
    Arduino Nano
    Arduino Nano 33 BLE Sense (Cortex M4)
    Seeeduino XIAO (Cortex M0+)
   
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
      Serial.println();
      
      //Tensor for the input data (8-bit quantized)
      aimath_q7_params_t input_q_params = { .shift = 7, .zero_point = -64 };              // Quantization parameter of the input
      int8_t input_data[] = {-64, 64};                                                    // Input data for the XOR ANN (0.0 / 1.0) 
      uint16_t input_shape[] = {1, 2};
      aitensor_t input_tensor = AITENSOR_2D_Q7(input_shape, &input_q_params, input_data); // Creation of the input AIfES tensor with two dimensions and data type Q7 (int8)

      // ---------------------------------- Layer definition ---------------------------------------
    
      // Input layer
      uint16_t input_layer_shape[] = {1, 2};      // Definition of the input layer shape (Must fit to the input tensor. The first dimension is the batch size.)
      ailayer_input_q7_t input_layer = AILAYER_INPUT_Q7_M( /*input dimension=*/ 2, /*input shape=*/ input_layer_shape, /*input quantization parameters=*/ &input_q_params);   // Creation of the AIfES input layer

      // Hidden dense layer
      // Weights (8 bit quantized)
      aimath_q7_params_t weights_q_params_dense_1 = { .shift = 3, .zero_point = 0 };
      int8_t weights_data_dense_1[] = {-81, -67, 44, 58, -61, -72};
      // Bias (32 bit quantized)
      aimath_q31_params_t bias_q_params_dense_1 = { .shift = 10, .zero_point = 0 };
      int32_t bias_data_dense_1[] = {-3036,  2425, -1635}; 
      // Result (8 bit quantized)
      aimath_q7_params_t result_q_params_dense_1 = { .shift = 3, .zero_point = 41 };
      // Creation of the AIfES hidden dense layer with 3 neurons
      ailayer_dense_q7_t dense_layer_1 = AILAYER_DENSE_Q7_M( /*neurons=*/ 3,
                                                             /*weights=*/ weights_data_dense_1, /*weights quantization params=*/ &weights_q_params_dense_1,
                                                             /*bias=*/ bias_data_dense_1, /*bias quantization params=*/ &bias_q_params_dense_1,
                                                             /*result quantization params=*/ &result_q_params_dense_1);

      // Hidden layer activation function
      ailayer_sigmoid_q7_t sigmoid_layer_1 = AILAYER_SIGMOID_Q7_M();
    
      // Output dense layer
     // Weights (8 bit quantized)
      aimath_q7_params_t weights_q_params_dense_2 = { .shift = 3, .zero_point = 0 };
      int8_t weights_data_dense_2[] = {96, -53, 95};
      // Bias (32 bit quantized)
      aimath_q31_params_t bias_q_params_dense_2 = { .shift = 11, .zero_point = 0 };
      int32_t bias_data_dense_2[] = {-11110};
      // Result (8 bit quantized)
      aimath_q7_params_t result_q_params_dense_2 = { .shift = 3, .zero_point = 11 };
      // Creation of the AIfES output dense layer with 1 neuron
      ailayer_dense_q7_t dense_layer_2 = AILAYER_DENSE_Q7_M( /*neurons=*/ 1,
                                                             /*weights=*/ weights_data_dense_2, /*weights quantization params=*/ &weights_q_params_dense_2,
                                                             /*bias=*/ bias_data_dense_2, /*bias quantization params=*/ &bias_q_params_dense_2,
                                                             /*result quantization params=*/ &result_q_params_dense_2);

      // Output layer activation function
      ailayer_sigmoid_q7_t sigmoid_layer_2 = AILAYER_SIGMOID_Q7_M();
    
      // --------------------------- Define the structure of the model ----------------------------
    
      aimodel_t model;  // AIfES model
      ailayer_t *x;     // Layer object from AIfES to connect the layers

      // Connect the layers to an AIfES model
      model.input_layer = ailayer_input_q7_default(&input_layer);
      x = ailayer_dense_q7_default(&dense_layer_1, model.input_layer);
      x = ailayer_sigmoid_q7_default(&sigmoid_layer_1, x);
      x = ailayer_dense_q7_default(&dense_layer_2, x);
      model.output_layer = ailayer_sigmoid_q7_default(&sigmoid_layer_2, x);
    
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
      aimath_q7_params_t output_q_params;
      uint16_t output_shape[2] = {1, 1};
      int8_t output_data[1*1];                 // Empty data array of size output_shape
      aitensor_t output_tensor = AITENSOR_2D_Q7(output_shape, &output_q_params, output_data);
      
      aialgo_inference_model(&model, &input_tensor, &output_tensor); // Inference / forward pass
    
      // ------------------------------------- Print result ------------------------------------

      Serial.println(F(""));
      Serial.println(F("Results:"));
      Serial.println(F("input 1:\tinput 2:\treal output:\tcalculated output:"));
      Serial.print(Q7_TO_FLOAT(input_data[0], input_q_params.shift, input_q_params.zero_point));
      Serial.print(F("\t\t"));
      Serial.print(Q7_TO_FLOAT(input_data[1], input_q_params.shift, input_q_params.zero_point));
      Serial.print(F("\t\t"));
      Serial.print(F("1.0"));
      Serial.print(F("\t\t"));
      Serial.println(Q7_TO_FLOAT(((int8_t* ) output_tensor.data)[0], output_q_params.shift, output_q_params.zero_point));
    
      // How to print the weights example
      // Serial.println(F("Dense 1 - Weights:"));
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
