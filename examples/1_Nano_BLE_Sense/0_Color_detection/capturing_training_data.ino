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

// This file is responsible for capturing training data

float training_data[NUMBER_OF_DATA][3];       // For each sample we need 3 values for each corresponding RGB value
float labels[NUMBER_OF_DATA][3];              // For each sample we need to specify the corresponding object/class, 3 classes per sample possible

void capturing_training_data() {
  // This function handles the measurements of the RGB values of three different objects and manages the user interaction.
  // A measurement starts as soon as an object is close to the Arduino Nano 33 BLE Board and there is enough light available.
  // This procedure is repeated for each object.

  // User output
  Serial.println(F("AIfES demo: color identification"));
  Serial.println(F("Grab three objects with different colors"));

  // ----------------------------------------- Data recording for the three objects/classes --------------------------

  // Record samples for class 0
  Serial.println(F("Place the board over the first object. The measurement starts automatically as soon as the sensor is close to the object."));
  Serial.println(F("The RGB values are displayed after each sample."));
  delay(5000);                                // Wait for the user to read the output
  wait_for_object();                          // Wait for user to move over the first object
  get_data_for_class(0);                      // Record data samples for object 1
  Serial.println(F("Class 0 ready. Please move away from the current object."));
  wait_for_no_object();                       // Wait for the user to move away from object 1

  // Record samples for class 1
  Serial.println(F(""));
  Serial.println(F("Please move the board over the second object"));
  wait_for_object();                          // Wait for the user to move over the second object
  get_data_for_class(1);                      // Record data samples for object 2
  Serial.println(F("Class 1 ready. Please move away from the current object."));
  wait_for_no_object();                       // Wait for the user to move away from object 2

  // Record samples for class 2
  Serial.println(F(""));
  Serial.println(F("Please move the board over the third object"));
  wait_for_object();                          // Wait for the user to move over the third object
  get_data_for_class(2);                      // Record data samples for object 3
  Serial.println(F("Class 2 ready. Please move away from the current object."));
  wait_for_no_object();                       // Wait for the user to move away from object 3

  // Data recording finished
  Serial.println(F("Data recording finished"));
  Serial.println(F(""));
}



void get_data_for_class(int class_nr) {
  // This function reads the RGB sensor and saves the measured values in the training_data array.
  // It also generates the labels for the corresponding object and saves them in the labels array.
  // This is necessary to train the ANN afterwards.

  // This function is called by the capturing_training_data() function.

  int r, g, b, c, p;
  float sum;
  int sample_counter = 0;

  // ----------------------------------------- Measure samples for training data --------------------------
  // User output
  Serial.println(F("Red,Green,Blue"));

  // Record the defined number of samples
  while (sample_counter < SAMPLES_PER_CLASS) {
    // Wait until the data from the proximity and colour sensors are available
    while (!APDS.colorAvailable() || !APDS.proximityAvailable()) {}

    // Check if sensor is close to an object. Wait until the sensor is close and give the user feedback about the current status
    wait_for_object();
     
    // When available, read the color and proximity data
    APDS.readColor(r, g, b, c);
    sum = r + g + b;
    p = APDS.readProximity();
   
    // Calculating the proportions of the individual colours in the total signal. They are normalized into a range from 0 to 1.
    float redRatio = r / sum;
    float greenRatio = g / sum;
    float blueRatio = b / sum;

    // Store the data in the training_data array
    // The index is used to store the training data of the first object with all its samples at the front.
    // The training data of the second object is after the first object and so on.
    // For each sample the three normalized RGB values are stored.
    training_data[class_nr * SAMPLES_PER_CLASS + sample_counter][0] = redRatio;
    training_data[class_nr * SAMPLES_PER_CLASS + sample_counter][1] = greenRatio;
    training_data[class_nr * SAMPLES_PER_CLASS + sample_counter][2] = blueRatio;

    // Generate the labels for training of the ANN. It needs to know which object belongs to each corresponding sample in the training_data array.
    // For each output neuron of a sample a label needs to be specified (second index).
    switch (class_nr) {
      case 0 :
        // Data recording for object 1. Therefore, the corresponding label needs to be set to 1.
        labels[class_nr * SAMPLES_PER_CLASS + sample_counter][0] = 1.0f;
        labels[class_nr * SAMPLES_PER_CLASS + sample_counter][1] = 0.0f;
        labels[class_nr * SAMPLES_PER_CLASS + sample_counter][2] = 0.0f;
        break;
      case 1 :
        // Data recording for object 2. Therefore, the corresponding label needs to be set to 1.
        labels[class_nr * SAMPLES_PER_CLASS + sample_counter][0] = 0.0f;
        labels[class_nr * SAMPLES_PER_CLASS + sample_counter][1] = 1.0f;
        labels[class_nr * SAMPLES_PER_CLASS + sample_counter][2] = 0.0f;
        break;
      case 2 :
        // Data recording for object 3. Therefore, the corresponding label needs to be set to 1.
        labels[class_nr * SAMPLES_PER_CLASS + sample_counter][0] = 0.0f;
        labels[class_nr * SAMPLES_PER_CLASS + sample_counter][1] = 0.0f;
        labels[class_nr * SAMPLES_PER_CLASS + sample_counter][2] = 1.0f;
        break;
    }

    sample_counter = sample_counter + 1;

    // print the data in CSV format
    Serial.print(redRatio, 3);
    Serial.print(F(","));
    Serial.print(greenRatio, 3);
    Serial.print(F(","));
    Serial.println(blueRatio, 3);

    // Slow down measurements to give the possibility to move the sensor over the object to get samples from different regions of the object
    //delay(500);
  }
}


void wait_for_object() {
  // Function periodically checks whether there is an object near the sensor.
  // If an object is near, the function releases the execution.

  float sum;
  bool object = false;
  long last_millis = millis();

  while (!object) {
    if (is_object_close()) {
      // There is an object with enough light detected
      object = true;
    } else {
      // There is no object detected or it is too dark
      object = false;
      // Print every 5 sec a debug message
      if (millis() - last_millis > 5000) {
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

void wait_for_no_object() {
  // Function periodically checks whether there is an object near the sensor.
  // If an object is no longer detected, the function releases the execution.
  
  float sum;
  bool object = true;
  long last_millis = millis();

  while (object) {
    if (is_object_close()) {
      // There is an object detected
      // Print every 5 sec a debug message
      if (millis() - last_millis > 5000) {
        last_millis = millis();
        Serial.println(F("An object is still detected. Please move away from the object."));
      }
      object = true;
    } else {
      // There is no object detected
      object = false;
    }
  }
}

bool is_object_close() {
  // Function checks if an object is close to the sensor
  // Returns the status if an object is detected, true --> object with enough light detected
  //                                            , false --> object not detected or it is too dark

  int r, g, b, c, p, sum;
  // Wait for proximity and color sensor data
  while (!APDS.colorAvailable() || !APDS.proximityAvailable()) {}

  // Read the color and proximity data
  APDS.readColor(r, g, b, c);
  sum = r + g + b;
  p = APDS.readProximity();
  if (p == 0 && c > 10 && sum > 0) {
    // Object close and enough light
    return true;
  } else {
    // No object close
    return false;
  }
}
