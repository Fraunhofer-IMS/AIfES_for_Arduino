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

// This file covers the prediction of the current object

void color_detection() {
  // This function is responsible for getting a RGB sample and use the trained model to detect the corresponding object
  // It displays the probability for each object/class

  int r, g, b, c, p;
  float sum;

  // ----------------------------------------- Definition of tensors --------------------------
  // Tensor for the input RGB
  // Those values are the input of the ANN
  float input_data[3];                                      // Array for storage of the RGB input data
  uint16_t input_shape[] = {1, 3};                          // Definition of the shape of the tensor, here: {1 (i.e. 1 sample), 3 (i.e. the sample contains 3 RGB values)}
  aitensor_t input_tensor;                                  // Creation of the input AIfES tensor
  input_tensor.dtype = aif32;                               // Definition of the used data type, here float with 32 bits, different ones are available
  input_tensor.dim = 2;                                     // Dimensions of the tensor, here 2 dimensions, as specified at input_shape
  input_tensor.shape = input_shape;                         // Set the shape of the input_tensor
  input_tensor.data = input_data;                           // Assign the input_data array to the tensor. It expects a pointer to the array where the data is stored

  // Tensor for the output with 3 classes
  // Output values of the ANN are saved here
  float output_data[3];                                     // Array for storage of the output data, for each object/class one output is created
  uint16_t output_shape[] = {1, 3};                         // Definition of the shape of the tensor, here: {1 (i.e. 1 sample), 3 (i.e. the sample contains predictions for 3 classes/objects)}
  aitensor_t output_tensor;                                 // Creation of the output AIfES tensor
  output_tensor.dtype = aif32;                              // Definition of the used data type, here float with 32 bits, different ones are available
  output_tensor.dim = 2;                                    // Dimensions of the tensor, here 2 dimensions, as specified at output_shape
  output_tensor.shape = output_shape;                       // Set the shape of the input_tensor
  output_tensor.data = output_data;                         // Assign the output_data array to the tensor. It expects a pointer to the array where the data is stored

  // ----------------------------------------- Get RGB values from object --------------------------
  // Wait for proximity and color sensor data
  while (!APDS.colorAvailable() || !APDS.proximityAvailable()) {}

  // Wait for an object
  wait_for_object_long();

  // read the color and proximity data
  APDS.readColor(r, g, b, c);
  sum = r + g + b;
  p = APDS.readProximity();

  // Normalization of input data
  float redRatio = r / sum;
  float greenRatio = g / sum;
  float blueRatio = b / sum;

  // Calculating the proportions of the individual colours in the total signal. They are normalized into a range from 0 to 1.
  input_data[0] = redRatio;
  input_data[1] = greenRatio;
  input_data[2] = blueRatio;

  // ----------------------------------------- Run the AIfES model to detect the object --------------------------
  // Run the inference with the trained AIfES model
  // The function needs the trained model, the input_tensor with the input data and the output_tensor where the results are saved in the corresponding array
  aialgo_inference_model(&model, &input_tensor, &output_tensor);

  // ----------------------------------------- Output of results --------------------------
  Serial.print (F("Object 1: "));
  Serial.print (output_data[0] * 100); // Probability in percent for class 0
  Serial.print (F("%"));
  Serial.println(F(""));
  Serial.print (F("Object 2: "));
  Serial.print (output_data[1] * 100); // Probability in percent for class 1
  Serial.print (F("%"));
  Serial.println(F(""));
  Serial.print (F("Object 3: "));
  Serial.print (output_data[2] * 100); // Probability in percent for class 2
  Serial.print (F("%"));
  Serial.println(F(""));
  Serial.println(F(""));

  // Reduce the output speed to be able to read the result of the detection comfortably
  //delay(1000);

}


void wait_for_object_long() {
  // Function periodically checks whether there is an object near the sensor.
  // If an object is near, the function releases the execution.

  float sum;
  bool object = false;
  long last_millis = millis();

  while (!object) {
    // is_object_close already implemented in capturing_training_data tab at the end of the file
    if (is_object_close()) {
      // There is an object with enough light detected
      object = true;
    } else {
      // There is no object detected or it is too dark
      object = false;
      // Print every 10 sec a debug message
      if (millis() - last_millis > 10000) {
        last_millis = millis();
        Serial.println(F("No object detected. This may be because: "));
        // Check if object is close
        if (APDS.readProximity() > 0)
        {
          Serial.println(F("You are not close enough to the object"));
        } else {
          // Object is close, but it is too dark
          Serial.println(F("It is too dark. Please switch the light on or go close to a window"));
        }
      }
    }
  }
}
