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

  AIfES Object color identification demo
  --------------------

  Versions:
    1.0.0   Initial version
    1.0.1   Adjusted comments, rearranging code in multiple tabs, start of recording of new data only if new object is close
   

  Hardware: Arduino Nano 33 BLE Sense board.

  Libraries: Arduino_APDS9960 library is necessary. It needs to be installed via the library manager. You can search for it and
                              install the latest version. It is needed for the RGB Sensor on the board.

  Learns the colors of three different objects and can later classify them.

  Usage:
  Everything works automatically, no need to send commands. You are guided by the instructions via the serial monitor.
  This is the rough order of operations:
  1. Place the board over the first object and wait until the measurement is finished.
  2. Place the board over the second object
  3. Place the board over the third object
  4. Wait until AIfES has trained the artificial neural network (ANN)
  5. Place the board again over an object to get the prediction of the classification displayed

  The artificial neural network:
  3 Input neurons / RGB data values
  6 hidden layer neurons (sigmoid activation function)
  3 output neurons (softmax activation function or sigmoid activation function) / 3 outputs for three objects/classes

  Data acquisition, creation of the AIfES model and training take place in the setup().
  The classification takes place in the main loop().

  The demo was inspired by this Arduino project:
  https://blog.arduino.cc/2019/11/07/fruit-identification-using-arduino-and-tensorflow/

*/

#include <Arduino_APDS9960.h>
#include <aifes.h>

#define SAMPLES_PER_CLASS 5                   // Measurements per object/class
#define NUMBER_OF_DATA 3*SAMPLES_PER_CLASS    // Calculates the total number of recorded samples, for each of the three classes we take SAMPLES_PER_CLASS samples
#define USE_SOFTMAX                           // if defined (default) you use softmax as activation function, if not defined (i.e. just comment it out like //#define USE_SOFTMAX) sigmoid is used as the activation of the output layer
                                              // Also, the loss function is selected depending on the activation layer. If softmax is used as activation function, then cross-entropy is used as loss function.
                                              // If sigmoid is selected (i.e. //#define USE_SOFTMAX), the mean square error is used as the loss function.

void setup() {

  // Start serial connection
  Serial.begin(115200);
  // Wait for serial connection to be available
  while (!Serial) {};

  // Initialize RGB and distance sensor
  if (!APDS.begin()) {
    // If the sensor is not initialized, throw an error and wait until it is available.
    Serial.print(F("Error initializing APDS9960 sensor."));
    while (!APDS.begin()) {
      Serial.print(F("."));
      delay(500);
    }
  }

  //IMPORTANT
  //AIfES requires random weights for training
  //Here the random seed is generated by the noise of an analog pin
  srand(analogRead(A5)); 

  // The training data is captured and stored in arrays. Details of the capturing process are in the tab capturing_training_data.
  capturing_training_data();

  // This is where the AIfES model is built. The function is defined in the tab creation_and_training.
  // The different layers are defined and storage is reserved for the trainable parameters (like weights, bias, ...) of the model.
  build_AIfES_model();

  //A simple countdown to give the user time to read the UART outputs
  countdown();

  // The AIfES model is trained with the captured data. This function is also defined in the tab creation_and_training
  // Not only is the training carried out here, but also the necessary memory is reserved for intermediate results, gradients and moments of the training and the results of the training are displayed
  train_AIfES_model();
  // Now the ANN is prepared and ready for the classification of the three objects
  
  // User output
  Serial.println(F(""));
  Serial.println(F("The model is trained. We are ready for classification.\nPlease hold the sensor close to one of the three objects. Then a detection starts automatically.\nThe results are displayed."));
  Serial.println(F("The classification is repeated as soon as an object is detected. To train the model with new objects reset the microcontroller"));
  // Give the user some time to read the output
  delay(1000);
  Serial.println(F("Ready to classify"));
}


void loop() {

  // Executes the classification with the ANN if the sensor is close to an object. The function is located in the detection tab.
  color_detection();

}

void countdown(){
  Serial.println(F("Training starts in:"));
  delay(1000);
  Serial.println(F("3"));
  delay(1000);
  Serial.println(F("2"));
  delay(1000);
  Serial.println(F("1"));
  delay(1000);
  Serial.println(F("GO :)"));
}