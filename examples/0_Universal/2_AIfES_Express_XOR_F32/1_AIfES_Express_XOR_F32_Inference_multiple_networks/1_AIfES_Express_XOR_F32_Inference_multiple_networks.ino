
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

  AIfES-Express XOR Inference multiple networks demo
  --------------------

  Versions:
    1.0.0   Initial version
  
  AIfES-Express is a simplified API for AIfES, which is directly integrated. So you can simply decide which variant you want to use.
  
  In this sketch, inference is performed for three different feed forward neural networks(FNN).
  This means a device could perform multiple tasks by simply loading a new FNN.
  This example even runs on the Arduino UNO

  The first two FNNs were trained to emulate an XOR gate. The FNNs differ in their network structure:
  1. network: 2-3(Sigmoid)-1(Sigmoid)                       XOR_FNN_2_3_1();
  2. network: 2-3-(Sigmoid)-3-(ReLU)-1-(Sigmoid)            XOR_FNN_2_3_3_1();
  For both FNNs, the following XOR truth table was trained:
  XOR truth table
  Input    Output
  0   0    0
  0   1    1
  1   0    1
  1   1    0

  In the third FNN, a fictive XOR gate with two outputs was trained.
  3. network: 2-3(Sigmoid)-2(Sigmoid)                      XOR_FNN_2_3_2(); 
  Fictional XOR table with two outputs / training data
  Input       Output
  0   0       1    1
  0   1       1    0
  1   0       0    1
  1   1       0    0
  
  Tested on:
    Arduino UNO
    Arduino Nano 33 BLE Sense
    Arduino Portenta H7  

  You can find more AIfES tutorials here:
  https://create.arduino.cc/projecthub/aifes_team
*/

//Serial keyword: "inference"

#include <aifes.h>                  // include the AIfES libary

#define DATASETS        4
#define FNN_3_LAYERS    3
#define FNN_4_LAYERS    4

// Input data XOR
const float input_data_XOR[] = {0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f};
// Output data XOR
float output_data_XOR[DATASETS];
// Expected results XOR
const float target_data_XOR[] = {0.0f, 1.0f, 1.0f, 0.0f};


// Output data XOR with 2 outputs 
float output_data_XOR_2out[DATASETS][2];
// Expected results XOR with 2 outputs 
float target_data_XOR_2out[DATASETS][2] = {
{1.0f, 1.0f},
{1.0f, 0.0f},
{0.0f, 1.0f},
{0.0f, 0.0f}
}; 


void setup() {
  Serial.begin(115200); //115200 baud rate (If necessary, change in the serial monitor)
  while (!Serial);

  delay(100);
  Serial.println(F("AIfES-Express XOR inference multiple networks demo"));
  Serial.println(F("Type >inference< to start"));
}

void XOR_FNN_2_3_1()
{
  Serial.println(F("AIfES-Express XOR inference: FNN 2-3-1"));
  
  uint32_t i;
  
  // -------------------------------- describe the feed forward neural network ----------------------------------
  // neurons each layer
  
  uint32_t FNN_structure[FNN_3_LAYERS] = {2,3,1};
  
  // select the activation functions for the dense layer
  AIFES_E_activations FNN_activations[FNN_3_LAYERS - 1];
  FNN_activations[0] = AIfES_E_sigmoid; // Sigmoid for hidden (dense) layer
  FNN_activations[1] = AIfES_E_sigmoid; // Sigmoid for output (dense) layer
  
  // AIfES Express function: calculate the number of weights needed
  uint32_t weight_number = AIFES_E_flat_weights_number_fnn_f32(FNN_structure,FNN_3_LAYERS);
  
  int8_t error = 0; //
  
  Serial.print(F("Number of weights: "));
  Serial.println(weight_number);
  
  // FlatWeights array
  float FlatWeights[] = {-10.1164f, -8.4212f, 5.4396f, 7.297f, -7.6482f, -9.0155f, -2.9653f,  2.3677f, -1.5968f, 12.0305f, -6.5858f, 11.9371f,-5.4247f};
  
  if(weight_number != sizeof(FlatWeights)/sizeof(float))
  {
      Serial.println(F("Error: number of weights wrong!"));
      return;
  }
  
  // fill the AIfES Express struct
  AIFES_E_model_parameter_fnn_f32 FNN;
  FNN.layer_count = FNN_3_LAYERS;
  FNN.fnn_structure = FNN_structure;
  FNN.fnn_activations = FNN_activations;
  FNN.flat_weights = FlatWeights;
  
  // -------------------------------- create the tensors ----------------------------------          
  uint16_t input_shape[] = {DATASETS, (uint16_t)FNN_structure[0]};                  // Definition of the input shape
  aitensor_t input_tensor = AITENSOR_2D_F32(input_shape, (void*)input_data_XOR);    // Macro for the simple creation of a float32 tensor. Also usable in the normal AIfES version  

        
  uint16_t output_shape[] = {DATASETS, (uint16_t)FNN_structure[FNN_3_LAYERS - 1]};  // Definition of the output shape
  aitensor_t output_tensor = AITENSOR_2D_F32(output_shape, (void*)output_data_XOR); // Macro for the simple creation of a float32 tensor. Also usable in the normal AIfES version

  // -------------------------------- do the inference ----------------------------------
  // AIfES Express function: do the inference
  error = AIFES_E_inference_fnn_f32(&input_tensor,&FNN,&output_tensor);
  
  error_handling_inference(error);
  
  // -------------------------------- print the results ----------------------------------
  Serial.println(F("Results:"));
  Serial.println(F("input 1:\tinput 2:\treal output:\tcalculated output:"));
  
  uint32_t input_counter = 0;
  
  for(i = 0; i < DATASETS; i++)
  {
    Serial.print (input_data_XOR[input_counter]);
    input_counter++;
    Serial.print (F("\t\t"));
    Serial.print (input_data_XOR[input_counter]);
    input_counter++;
    Serial.print (F("\t\t"));
    Serial.print (target_data_XOR[i]);
    Serial.print (F("\t\t"));
    Serial.println(output_data_XOR[i], 5);
    //Serial.println(((float* ) output_tensor.data)[i], 5); // alternative print
  }  
}

void XOR_FNN_2_3_3_1()
{
  Serial.println(F("AIfES-Express XOR inference: FNN 2-3-3-1"));
  
  uint32_t i;
  
  // -------------------------------- describe the feed forward neural network ----------------------------------
  // neurons each layer
  
  uint32_t FNN_structure[FNN_4_LAYERS] = {2,3,3, 1};
  
  // select the activation functions for the dense layer
  AIFES_E_activations FNN_activations[FNN_4_LAYERS - 1];
  FNN_activations[0] = AIfES_E_sigmoid; // Sigmoid for the first hidden (dense) layer
  FNN_activations[1] = AIfES_E_relu;    // Sigmoid for the second hidden (dense) layer
  FNN_activations[2] = AIfES_E_sigmoid; // Sigmoid for output (dense) layer
  
  // AIfES Express function: calculate the number of weights needed
  uint32_t weight_number = AIFES_E_flat_weights_number_fnn_f32(FNN_structure,FNN_4_LAYERS);
  
  int8_t error = 0; //
  
  Serial.print(F("Number of weights: "));
  Serial.println(weight_number);
  
  // FlatWeights array
  float FlatWeights[] = {3.68613f, -3.50253f, 2.61260f, -4.21124f, 2.66794f, 3.48708f, -2.56205f, -1.27997f, -0.40625f, 2.50684f, 3.12326f, 2.16353f, 2.25493f, 2.87724f, 1.08713f, 0.32959f, 0.79199f, -0.09795f, -0.85033f, -1.24613f, -0.82667f, 2.78485f, 2.50918f, 0.17473f, -3.35409f};

  
  if(weight_number != sizeof(FlatWeights)/sizeof(float))
  {
      Serial.println(F("Error: number of weights wrong!"));
      return;
  }
  
  // fill the AIfES Express struct
  AIFES_E_model_parameter_fnn_f32 FNN;
  FNN.layer_count = FNN_4_LAYERS;
  FNN.fnn_structure = FNN_structure;
  FNN.fnn_activations = FNN_activations;
  FNN.flat_weights = FlatWeights;
  
  // -------------------------------- create the tensors ----------------------------------          
  uint16_t input_shape[] = {DATASETS, (uint16_t)FNN_structure[0]};                  // Definition of the input shape
  aitensor_t input_tensor = AITENSOR_2D_F32(input_shape, (void*)input_data_XOR);    // Macro for the simple creation of a float32 tensor. Also usable in the normal AIfES version

  uint16_t output_shape[] = {DATASETS, (uint16_t)FNN_structure[FNN_4_LAYERS - 1]};  // Definition of the output shape
  aitensor_t output_tensor = AITENSOR_2D_F32(output_shape, (void*)output_data_XOR); // Macro for the simple creation of a float32 tensor. Also usable in the normal AIfES version
 
  // -------------------------------- do the inference ----------------------------------
  // AIfES Express function: do the inference
  error = AIFES_E_inference_fnn_f32(&input_tensor,&FNN,&output_tensor);
  
  error_handling_inference(error);
  
  // -------------------------------- print the results ----------------------------------
  Serial.println(F("Results:"));
  Serial.println(F("input 1:\tinput 2:\treal output:\tcalculated output:"));
  
  uint32_t input_counter = 0;
  
  for(i = 0; i < DATASETS; i++)
  {
    Serial.print (input_data_XOR[input_counter]);
    input_counter++;
    Serial.print (F("\t\t"));
    Serial.print (input_data_XOR[input_counter]);
    input_counter++;
    Serial.print (F("\t\t"));
    Serial.print (target_data_XOR[i]);
    Serial.print (F("\t\t"));
    Serial.println(output_data_XOR[i], 5);
    //Serial.println(((float* ) output_tensor.data)[i], 5); // alternative print
  }  
}

void XOR_FNN_2_3_2()
{
  Serial.println(F("AIfES-Express XOR inference with 2 Outputs: FNN 2-3-2"));
  
  uint32_t i;
  
  // -------------------------------- describe the feed forward neural network ----------------------------------
  // neurons each layer
  
  uint32_t FNN_structure[FNN_3_LAYERS] = {2,3,2};
  
  // select the activation functions for the dense layer
  AIFES_E_activations FNN_activations[FNN_3_LAYERS - 1];
  FNN_activations[0] = AIfES_E_sigmoid; // Sigmoid for hidden (dense) layer
  FNN_activations[1] = AIfES_E_sigmoid; // Sigmoid for output (dense) layer
  
  // AIfES Express function: calculate the number of weights needed
  uint32_t weight_number = AIFES_E_flat_weights_number_fnn_f32(FNN_structure,FNN_3_LAYERS);
  
  int8_t error = 0; //
  
  Serial.print(F("Number of weights: "));
  Serial.println(weight_number);
  
  // FlatWeights array
  float FlatWeights[] = {0.29496f, 2.90107f, -4.23272f, -4.80778f, -2.63794f, -1.68329f, 1.63589f, 0.20421f, 2.43719f, -1.20958f, 6.39140f, -2.49475f, 2.15817f, 7.75255f, 1.29185f, -1.44993f, -4.48125f};

  
  if(weight_number != sizeof(FlatWeights)/sizeof(float))
  {
      Serial.println(F("Error: number of weights wrong!"));
      return;
  }
  
  // fill the AIfES Express struct
  AIFES_E_model_parameter_fnn_f32 FNN;
  FNN.layer_count = FNN_3_LAYERS;
  FNN.fnn_structure = FNN_structure;
  FNN.fnn_activations = FNN_activations;
  FNN.flat_weights = FlatWeights;
  
  // -------------------------------- create the tensors ----------------------------------          
  uint16_t input_shape[] = {DATASETS, (uint16_t)FNN_structure[0]};                  // Definition of the input shape
  aitensor_t input_tensor = AITENSOR_2D_F32(input_shape, (void*)input_data_XOR);    // Macro for the simple creation of a float32 tensor. Also usable in the normal AIfES version
 
  uint16_t output_shape[] = {DATASETS, (uint16_t)FNN_structure[FNN_3_LAYERS - 1]}; // Definition of the output shape
  aitensor_t output_tensor = AITENSOR_2D_F32(output_shape, (void*)output_data_XOR_2out); // Macro for the simple creation of a float32 tensor. Also usable in the normal AIfES version
  
  // -------------------------------- do the inference ----------------------------------
  // AIfES Express function: do the inference
  error = AIFES_E_inference_fnn_f32(&input_tensor,&FNN,&output_tensor);
  
  error_handling_inference(error);
  
  // -------------------------------- print the results ----------------------------------
  Serial.println(F("Results:"));
  Serial.println(F("input 1:\tinput 2:\treal output 1:\treal output 2:\tcalc 1:\t\tcalc 2:"));
  
  uint32_t input_counter = 0;
  
  for(i = 0; i < DATASETS; i++)
  {
    Serial.print (input_data_XOR[input_counter]);
    input_counter++;
    Serial.print (F("\t\t"));
    Serial.print (input_data_XOR[input_counter]);
    input_counter++;
    Serial.print (F("\t\t"));
    Serial.print (target_data_XOR_2out[i][0]);
    Serial.print (F("\t\t"));
    Serial.print (target_data_XOR_2out[i][1]);
    Serial.print (F("\t\t"));
    Serial.print (output_data_XOR_2out[i][0], 5);
    Serial.print (F("\t\t"));
    Serial.println(output_data_XOR_2out[i][1], 5);
  //Serial.println(((float* ) output_tensor.data)[i], 5); // alternative print
  }  
}

void loop() {

  while(Serial.available() > 0 ){
    String str = Serial.readString();
    if(str.indexOf("inference") > -1){        // Keyword "inference"

      Serial.println(F(""));
      XOR_FNN_2_3_1();
      Serial.println(F(""));
      XOR_FNN_2_3_3_1();
      Serial.println(F(""));
      XOR_FNN_2_3_2();
  
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
