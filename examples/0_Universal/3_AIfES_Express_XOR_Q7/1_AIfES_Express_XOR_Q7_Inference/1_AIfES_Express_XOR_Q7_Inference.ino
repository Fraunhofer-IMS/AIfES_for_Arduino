
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

  AIfES-Express XOR Q7 Inference demo
  --------------------

  Versions:
    1.0.0   Initial version

  AIfES-Express is a simplified API for AIfES, which is directly integrated. So you can simply decide which variant you want to use.
  
  The parameters / FlatWeights were calculated with the example Sketch "0_AIfES_Express_XOR_Q7_model_quantisation"
  
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

//Serial keyword: "inference"

#include <aifes.h>                  // include the AIfES libary

#define DATASETS 4
#define FNN_3_LAYERS 3

void setup() {
  Serial.begin(115200); //115200 baud rate (If necessary, change in the serial monitor)
  while (!Serial);

  delay(100);
  Serial.println(F("AIfES-Express XOR Q7 inference demo (FNN: 2-3-1)"));
  Serial.println(F("Type >inference< to start"));
}

void loop() {

  while(Serial.available() > 0 ){
    String str = Serial.readString();
    if(str.indexOf("inference") > -1){        // Keyword "inference"

        Serial.println(F("AIfES-Express Q7 inference"));
        Serial.println(F(""));

        uint32_t i;

        // -------------------------------- describe the feed forward neural network ----------------------------------
        // neurons each layer
        // FNN_structure[0] = input layer with 2 inputs
        // FNN_structure[1] = hidden (dense) layer with 3 neurons
        // FNN_structure[2] = output (dense) layer with 1 output
        uint32_t FNN_structure[FNN_3_LAYERS] = {2,3,1};

        // select the activation functions for the dense layer
        AIFES_E_activations FNN_activations[FNN_3_LAYERS - 1];
        FNN_activations[0] = AIfES_E_sigmoid; // Sigmoid for hidden (dense) layer
        FNN_activations[1] = AIfES_E_sigmoid; // Sigmoid for output (dense) layer

        /* possible activation functions
        AIfES_E_relu
        AIfES_E_sigmoid
        AIfES_E_softmax
        AIfES_E_leaky_relu
        AIfES_E_elu
        AIfES_E_tanh
        AIfES_E_softsign
        AIfES_E_linear
        */

        // AIfES Express function: calculate the number of weights needed
        uint32_t weight_number = AIFES_E_flat_weights_number_fnn_q7(FNN_structure, FNN_3_LAYERS);

        int8_t error = 0;

        Serial.print(F("Number of weights/parameters: "));
        Serial.println(weight_number);

        // 4 byte memory aligned FlatWeights array Q7, e.g. for Arduino Nano 33 BLE
        uint32_t FlatWeights_Q7_aligned_4[ 16 ] = {
            0x00BA0007, 0x00290003, 0x000B0003, 0x00000003, 0x3A2CBDAF, 0x0000B8C3, 0x0000000A, 0x00000000, 0xFFFFF424, 0x00000979,
            0xFFFFF99D, 0x00000003, 0x005FCB60, 0x0000000B, 0x00000000, 0xFFFFD49A
        };

        // 2 byte memory aligned FlatWeights array Q7, e.g. for Arduino Uno
        uint16_t FlatWeights_Q7_aligned_2[ 29 ] = {
            0x0007, 0x00BA, 0x0003, 0x0029, 0x0003, 0x000B, 0x0003, 0x0000, 0xBDAF, 0x3A2C, 0xB8C3, 0x000A, 0x0000, 0x0000, 0xF424, 0xFFFF,
            0x0979, 0x0000, 0xF99D, 0xFFFF, 0x0003, 0x0000, 0xCB60, 0x005F, 0x000B, 0x0000, 0x0000, 0xD49A, 0xFFFF
        };
        
        void *FlatWeights_Q7;
        
        // Set the right weight buffer that fits to the cpu architecture (8 / 16 / 32 bit)
        if(AIFES_MEMORY_ALIGNMENT == 4){
            // For example for Arduino Nano 33 BLE
            FlatWeights_Q7 = FlatWeights_Q7_aligned_4;
            if(weight_number > sizeof(FlatWeights_Q7_aligned_4)/sizeof(uint8_t)) {
                Serial.println(F("Error: number of weights wrong!"));
                //return;
            }
        } else if(AIFES_MEMORY_ALIGNMENT == 2){
            // For example for Arduino Uno
            FlatWeights_Q7 = FlatWeights_Q7_aligned_2;
            if(weight_number > sizeof(FlatWeights_Q7_aligned_2)/sizeof(uint8_t)) {
                Serial.println(F("Error: number of weights wrong!"));
                //return;
            }
        } else {
            Serial.println(F("Error: Processor architecture not supported!"));
        }

      // fill the AIfES Express struct
      AIFES_E_model_parameter_fnn_f32 FNN;
      FNN.layer_count = FNN_3_LAYERS;
      FNN.fnn_structure = FNN_structure;
      FNN.fnn_activations = FNN_activations;
      FNN.flat_weights = FlatWeights_Q7;

      // -------------------------------- create the tensors ----------------------------------

      float input_data[] = {0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f};// Input data
      uint16_t input_shape[] = {DATASETS, (uint16_t)FNN_structure[0]};      // Definition of the input shape
      aitensor_t input_tensor = AITENSOR_2D_F32(input_shape, input_data);   // Macro for the simple creation of a float32 tensor. Also usable in the normal AIfES version    

      float output_data[DATASETS];                                        // Output data
      uint16_t output_shape[] = {DATASETS, (uint16_t)FNN_structure[2]};    // Definition of the output shape
      aitensor_t output_tensor = AITENSOR_2D_F32(output_shape, output_data);   // Macro for the simple creation of a float32 tensor. Also usable in the normal AIfES version

      // Expected results
      const float target_data[] = {0.0f, 1.0f, 1.0f, 0.0f};
      
      // -------------------------------- do the inference ----------------------------------
      // AIfES Express function: do the inference
      // AIfES Express function: do the inference Q7
      // The float input tensor is converted to a Q7 tensor in the function
      // The inference is performed with the Q7 FNN
      // The result is converted to float and output via the output tensor
      AIFES_E_inference_fnn_q7(&input_tensor,&FNN,&output_tensor);

      error_handling_inference(error);

      // -------------------------------- print the results ----------------------------------
      Serial.println(F(""));
      Serial.println(F("Results Q7:"));
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

      Serial.println(F(""));
      Serial.println(F("Type >inference< to restart"));
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
