
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

  AIfES-Express XOR F32 to Q7 model quantization demo
  --------------------

  Versions:
    1.0.0   Initial version

  AIfES-Express is a simplified API for AIfES, which is directly integrated. So you can simply decide which variant you want to use.
  
  The sketch shows an example of how a model with float (f32) weights can be converted to a model with integer (q7) weights with AIfES-Express.

  Look at the "0_AIfES_Express_XOR_F32_Inference.ino" for more informations about the network.
  
  XOR truth table
  Input    Output
  0   0    0
  0   1    1
  1   0    1
  1   1    0
  
  
  Tested on:
    Arduino UNO
    Arduino Nano 33 BLE Sense
    Arduino Portenta H7

  You can find more AIfES tutorials here:
  https://create.arduino.cc/projecthub/aifes_team 
*/

//Serial keyword: "go"

#include <aifes.h>                  // include the AIfES libary

#define DATASETS 4
#define FNN_3_LAYERS 3

void setup() {
  Serial.begin(115200); //115200 baud rate (If necessary, change in the serial monitor)
  while (!Serial);

  delay(100);
  Serial.println(F("AIfES-Express XOR F32 to Q7 model quantization demo (FNN: 2-3-1)"));
  Serial.println(F("Type >go< to start"));
}

void loop() {

  while(Serial.available() > 0 ){
    String str = Serial.readString();
    if(str.indexOf("go") > -1){        // Keyword "inference"

      Serial.println(F("AIfES-Express model quantization"));
      Serial.println(F(""));
      
      uint32_t i;
      


      // -------------------------------- describe the feed forward neural network ----------------------------------
      uint32_t FNN_structure[FNN_3_LAYERS] = {2,3,1};

      // select the activation functions for the dense layer
      AIFES_E_activations FNN_activations[FNN_3_LAYERS - 1];
      FNN_activations[0] = AIfES_E_sigmoid; // Sigmoid for hidden (dense) layer
      FNN_activations[1] = AIfES_E_sigmoid; // Sigmoid for output (dense) layer

      int8_t error = 0;

      // FlatWeights array for F32
      float FlatWeights_F32[] = {-10.1164f, -8.4212f, 5.4396f, 7.297f, -7.6482f, -9.0155f, -2.9653f,  2.3677f, -1.5968f, 12.0305f, -6.5858f, 11.9371f,-5.4247f};

      // fill the AIfES Express struct
      AIFES_E_model_parameter_fnn_f32 FNN;
      FNN.layer_count = FNN_3_LAYERS;
      FNN.fnn_structure = FNN_structure;
      FNN.fnn_activations = FNN_activations;
      FNN.flat_weights = FlatWeights_F32;

      // -------------------------------- create the tensors ----------------------------------

      float input_data[] = {0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f};// Input data
      uint16_t input_shape[] = {DATASETS, (uint16_t)FNN_structure[0]};      // Definition of the input shape
      aitensor_t input_tensor = AITENSOR_2D_F32(input_shape, input_data);   // Macro for the simple creation of a float32 tensor. Also usable in the normal AIfES version    

      float output_data[DATASETS];                                        // Output data
      uint16_t output_shape[] = {DATASETS, (uint16_t)FNN_structure[2]};    // Definition of the output shape
      aitensor_t output_tensor = AITENSOR_2D_F32(output_shape, output_data);   // Macro for the simple creation of a float32 tensor. Also usable in the normal AIfES version

      // Expected results
      const float target_data[] = {0.0f, 1.0f, 1.0f, 0.0f};
           
      // -------------------------------- do the inference F32 ----------------------------------
      // AIfES Express function: do the inference
      error = AIFES_E_inference_fnn_f32(&input_tensor,&FNN,&output_tensor);

      error_handling_inference(error);

      // -------------------------------- print the results F32 ----------------------------------
      Serial.println(F("Results F32:"));
      Serial.println(F("input 1:\tinput 2:\treal output:\tcalculated output:"));

      uint32_t input_counter = 0;
      
      for(i = 0; i < DATASETS; i++)
      {
        Serial.print (input_data[input_counter]);
        input_counter++;
        Serial.print (F("\t\t"));
        Serial.print (input_data[input_counter]);
        input_counter++;
        Serial.print (F("\t\t"));
        Serial.print (target_data[i]);
        Serial.print (F("\t\t"));
        Serial.println(output_data[i], 5);
        //Serial.println(((float* ) output_tensor.data)[i], 5); // alternative print
      }

      // -------------------------------- Calculate the parameter/Weight memory for Q7 ----------------------------------
      // AIfES Express function: Calculate the array length
      uint32_t q7_parameter_memory_size = AIFES_E_flat_weights_number_fnn_q7(FNN_structure, FNN_3_LAYERS);
      Serial.println(F(""));
      Serial.println(F("Q7 FNN"));
      Serial.print(F("Q7 weights memory size: "));
      Serial.println(q7_parameter_memory_size);
      uint8_t *FlatWeights_Q7 = (uint8_t *) malloc(q7_parameter_memory_size);

      // -------------------------------- Q7 quantisation ----------------------------------
      // AIfES Express function: do the quantisation
      AIFES_E_quantisation_fnn_f32_to_q7(&input_tensor,&FNN,FlatWeights_Q7);

      // Change the flat weights for q7 calculation
      FNN.flat_weights = FlatWeights_Q7;

      // -------------------------------- do the inference Q7 ----------------------------------
      // AIfES Express function: do the inference Q7
      // The float input tensor is converted to a Q7 tensor in the function
      // The inference is performed with the Q7 FNN
      // The result is converted to float and output via the output tensor
      AIFES_E_inference_fnn_q7(&input_tensor,&FNN,&output_tensor);

      input_counter = 0;

      Serial.println(F("Results Q7:"));
      Serial.println(F("input 1:\tinput 2:\treal output:\tcalculated output:"));
      
      for(i = 0; i < DATASETS; i++)
      {
        Serial.print (input_data[input_counter]);
        input_counter++;
        Serial.print (F("\t\t"));
        Serial.print (input_data[input_counter]);
        input_counter++;
        Serial.print (F("\t\t"));
        Serial.print (target_data[i]);
        Serial.print (F("\t\t"));
        Serial.println(output_data[i], 5);
        //Serial.println(((float* ) output_tensor.data)[i], 5); // alternative print
      }

       //Print the Q7 parameter array
      Serial.println(F(""));
      Serial.println(F("Q7 Parameters/FlatWeights"));
      Serial.print(F("uint8_t FlatWeights_Q7[] = {"));
      
      for(i = 0; i < q7_parameter_memory_size; i++)
      {
        Serial.print(FlatWeights_Q7[i]);
        if(i < q7_parameter_memory_size - 1)
        {
          Serial.print(F(", "));
        }
      }
      Serial.println(F("};"));

      Serial.println(F(""));
      Serial.println(F("Type >go< to restart"));
    }
    else{
      Serial.println(F("unknown"));
    }
  }
  
}

void error_handling_inference(int8_t error_nr){
  switch(error_nr){
    case 0:
      //Serial.println(F("No Error :)"));
      break;    
    case -1:
      Serial.println(F("ERROR! Tensor dtype"));
      break;
    case -2:
      Serial.println(F("ERROR! Tensor shape: Data Number"));
      break;
    case -3:
      Serial.println(F("ERROR! Input tensor shape does not correspond to ANN inputs"));
      break;
    case -4:
      Serial.println(F("ERROR! Output tensor shape does not correspond to ANN outputs"));
      break;
    case -5:
      Serial.println(F("ERROR! Unknown activation function"));
      break;
    case -6:
      Serial.println(F("ERROR! Not enough memory"));
      break;
    default :
      Serial.println(F("Unknown error"));
  }
}
