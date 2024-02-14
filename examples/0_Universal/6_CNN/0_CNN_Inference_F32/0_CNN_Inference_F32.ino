/*
  www.aifes.ai
  https://github.com/Fraunhofer-IMS/AIfES_for_Arduino
  Copyright (C) 2020-2023  Fraunhofer Institute for Microelectronic Circuits and Systems.
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

  AIfES CNN Inference F32 Example
  --------------------

  Versions:
    1.0.0   Initial version
  
  The sketch shows an example of how the inference of an already trained convolutional neural network (CNN) is performed. 
  This example is just for demonstration of the inference procedure with a CNN and has no deeper meaning. 

  Tested on:
    Arduino Nano BLE 33
    Arduino Nano Every
    Seeeduino XIAO
   
*/

#include <aifes.h>


void setup() {
  // Init serial port for communication
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect.
  }

  Serial.println(F("AIfES CNN Inference F32 Example"));
  Serial.println(F("Type >inference< (or anything else) to start"));
  while(!Serial.available());
  Serial.readString();
  Serial.println();

  // Init neural network
  aimodel_t *neural_network = init_neural_network();

  // Create input data for testing
  float input_data[] = {0.0f, 1.0f, 0.0f,
						            0.0f, 1.0f, 0.0f,
						            0.0f, 1.0f, 0.0f,
						            0.0f, 1.0f, 0.0f};
	uint16_t input_shape[] = {1, 1, 4, 3}; // [batch-size, channels, height, width] <- channels first format
	aitensor_t input_tensor = AITENSOR_4D_F32(input_shape, input_data);

  // Create an empty output tensor
	float output_data[1];
	uint16_t output_shape[2] = {1, 1}; // [batch-size, output neurons]
	aitensor_t output_tensor = AITENSOR_2D_F32(output_shape, output_data);

  // Run the neural network and write the results into the output_tensor
  aialgo_inference_model(neural_network, &input_tensor, &output_tensor);
  
	Serial.print("Inputs:\n");
	print_aitensor(&input_tensor);

	Serial.print("\nPrediction:\n");
	print_aitensor(&output_tensor);
}

void loop() {
  // Do nothing
}
