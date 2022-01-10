
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

  AIfES XOR F32 to Q7 model quantization demo
  --------------------

  Versions:
    1.0.0   Initial version
  
  The sketch shows an example of how a model with float (f32) weights can be converted to a model with integer (q7) weights.
  
  XOR truth table
  Input    Output
  0   0    0
  0   1    1
  1   0    1
  1   1    0
  
  Input data in the example (0.0 | 1.0)
  
  
  Tested on:
    Arduino Nano
    Seeeduino XIAO (Cortex M0+)
   
*/

#include <aifes.h>                  // include the AIfES libary


//Tensor for the input data
uint16_t input_shape_f32[] = {4, 2};                                                // Definition of the input shape
float input_data_f32[] = {
  0.0f, 0.0f,
  0.0f, 1.0f,
  1.0f, 0.0f,
  1.0f, 1.0f
};                                              // Input data for the XOR ANN (0.0 / 1.0) 
aitensor_t input_tensor_f32 = AITENSOR_2D_F32(input_shape_f32, input_data_f32);     // Creation of the input AIfES tensor with two dimensions and data type F32 (float32)

aimodel_t *model_f32;
aimodel_t *model_q7;

void setup() {
  Serial.begin(115200); //115200 baud rate (If necessary, change in the serial monitor)
  while (!Serial);
  delay(100);
  
  Serial.println(F("########### AIfES XOR F32 to Q7 model quantization demo ###########\n"));
  
  // ------------------------------------- Initialize the models  -------------------------------------

  model_f32 = init_model_f32();
  model_q7 = init_model_q7();

  // ---------------- Quantize the Q7 model with the parameters of the original (F32) model ------------------

  // Quantize model
  aialgo_quantize_model_f32_to_q7(model_f32, model_q7, &input_tensor_f32);

  Serial.println(F("\n\nSuccessfully quantized the F32 model to a Q7 model.\n"));
  print_model_f32_parameters();
  Serial.println();
  print_model_q7_parameters();

  Serial.println(F("\n\nType >f32< to run the inference of the f32 model or >q7< to run the inference of the q7 model."));
}

void loop() {

  while(Serial.available() > 0 ){
    String str = Serial.readString();
    if(str.indexOf("f32") > -1){        // Keyword "f32"
      Serial.println(F("AIfES inference on F32 model"));
      Serial.println();
    
      // ------------------------------------- Run the F32 inference ------------------------------------

      // Create an empty output tensor for the inference result
      uint16_t output_shape_f32[2] = {4, 1};
      float output_data_f32[4*1];                 // Empty data array of size output_shape
      aitensor_t output_tensor_f32 = AITENSOR_2D_F32(output_shape_f32, output_data_f32);
      
      aialgo_inference_model(model_f32, &input_tensor_f32, &output_tensor_f32); // Inference / forward pass
    
      // ------------------------------------- Print result ------------------------------------

      Serial.println();
      Serial.println(F("Inference results of the F32 model:"));
      Serial.println(F("Inputs F32:"));
      print_aitensor(&input_tensor_f32);
      Serial.println(F("Outputs F32:"));
      print_aitensor(&output_tensor_f32);

      Serial.println(F("\nType >f32< or >q7< to restart"));  
    } 
    else if(str.indexOf("q7") > -1){        // Keyword "q7"
      Serial.println(F("AIfES inference on Q7 model"));
      Serial.println();
      

      // ------------------------------------- Quantize input data ------------------------------------
      int8_t input_data_q7[4*2];
      aitensor_t input_tensor_q7 = AITENSOR_2D_Q7(input_shape_f32, model_q7->input_layer->result.tensor_params, input_data_q7);
      aimath_q7_quantize_tensor_from_f32(&input_tensor_f32, &input_tensor_q7);
    
      // ------------------------------------- Run the Q7 inference ------------------------------------

      // Create an empty output tensor for the inference result
      aimath_q7_params_t output_q_params_q7;
      uint16_t output_shape_q7[4] = {4, 1};
      int8_t output_data_q7[4*1];                 // Empty data array of size output_shape
      aitensor_t output_tensor_q7 = AITENSOR_2D_Q7(output_shape_q7, &output_q_params_q7, output_data_q7);
      
      aialgo_inference_model(model_q7, &input_tensor_q7, &output_tensor_q7); // Inference / forward pass
    
      // ------------------------------------- Print result ------------------------------------
      
      Serial.println(F("Inference results of the quantized Q7 model:"));
      Serial.println(F("Inputs Q7:"));
      print_aitensor(&input_tensor_q7);
      Serial.println(F("Outputs Q7:"));
      print_aitensor(&output_tensor_q7);

      Serial.println(F("\nType >f32< or >q7< to restart"));  
    } else {
      Serial.println(F("unknown"));
    }
  }
}
