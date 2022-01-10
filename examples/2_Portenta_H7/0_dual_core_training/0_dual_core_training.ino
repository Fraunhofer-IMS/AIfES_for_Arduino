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

  AIfES-Express Portenta H7 Dual-Core demo
    --------------------

    Versions:
    1.0.0   Initial version

  AIfES-Express is a simplified API for AIfES, which is directly integrated. So you can simply decide which variant you want to use.

  - This example uses the dual-core feature of the Portenta H7
  - The sketch must be flashed on both cores (M7 & M4)
  - The M7 uses the blue LED and the M4 the green LED 
  
  What does the example:
  - The M7 can only execute the artificial neural network (ANN)
  - At startup the ANN is not trained
  - The M7 can give the M4 the task to train the ANN
  - The M4 then trains the ANN and passes the calculated weights to the M7
  - The M7 can perform the inference after training

  Possible commands:
  >inference< The M7 performs an inference (The FNN must be trained previously)
  >training< The M7 tells the M4 to train (Further training possible by repeating the command)
  >reset< The M7 resets the trained weights

  How it works:
  - On both cores the same network structure is built with AIfES
  - Both cores have a memory area for the weights
  - Remote Procedure Call (RPC) is used for communication between the cores
  - For the commands variables are used as flags
  - When the M4 is done with the training, the weights are transferred to the M7

  Which ANN was used:
  - The neural network is trained from scratch in AIfES-Express using training data.
  - As in the example "2_AIfES-Express_XOR_training", an XOR gate is mapped here using a neural network. 
  - The 4 different states of an XOR gate are fed in as training data here. 
  - The network structure is 2-3(Sigmoid)-1(Sigmoid) and Sigmoid is used as activation function.
  
  XOR truth table / training data
  Input    Output
  0   0    0
  0   1    1
  1   0    1
  1   1    0
  
  Only for:
    Arduino Portenta H7

  Known issues:
  - After longer operation, the Portenta may show an error(the Portenta starts blinking red)
  https://forum.arduino.cc/t/passing-variable-from-cm7-to-cm4-using-rpc/692026

  You can find more AIfES tutorials here:
  https://create.arduino.cc/projecthub/aifes_team
*/

#include "RPC_internal.h"           
#include <aifes.h>                  // include the AIfES libary

//Defines for M7 and M4
#define DATASETS        4
#define FNN_3_LAYERS    3

#define INPUTS          2
#define NEURONS         3
#define OUTPUTS         1

#define WEIGHTS         13
#define PRINT_INTERVAL  10

//Global variables for M7 and M4
int global_LED;

// AIfES tensors for M7 and M4
float input_data[] = {0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f};// Input data
uint16_t input_shape[] = {DATASETS, INPUTS};
aitensor_t input_tensor = AITENSOR_2D_F32(input_shape, input_data);

float target_data[] = {0.0f, 1.0f, 1.0f, 0.0f};
uint16_t target_shape[] = {DATASETS, OUTPUTS};
aitensor_t target_tensor = AITENSOR_2D_F32(target_shape, target_data); 

float output_data[DATASETS];
uint16_t output_shape[] = {DATASETS, OUTPUTS};
aitensor_t output_tensor = AITENSOR_2D_F32(output_shape, output_data);

// AIfES network structure for M7 and M4
uint32_t FNN_structure[FNN_3_LAYERS] = {INPUTS,NEURONS,OUTPUTS};

// AIfES activation functions for M7 and M4
AIFES_E_activations FNN_activations[FNN_3_LAYERS - 1];

// AIfES model parameter for M7 and M4 (needed for inference (M7) and training (M4))
AIFES_E_model_parameter_fnn_f32 FNN;

// Global variables and functions for M7
#ifdef CORE_CM7

  // Flags for the communication between the cores
  int train_task_flag = 0;
  int net_trained_flag = 0;
  
  // Global weights of the M7 to which the M4 has access
  float M7_weights[WEIGHTS] = {0};

  //float M7_weights []= {-10.1164f, -8.4212f, 5.4396f, 7.297f, -7.6482f, -9.0155f, -2.9653f,  2.3677f, -1.5968f, 12.0305f, -6.5858f, 11.9371f,-5.4247f};

  void M7_inference(){   
    uint32_t i = 0;
    uint32_t input_counter = 0;
    int8_t error = 0;

    error = AIFES_E_inference_fnn_f32(&input_tensor,&FNN,&output_tensor);
    
    for(i = 0; i < DATASETS; i++)
    {
      Serial.print(input_data[input_counter]);
      input_counter++;
      Serial.print(F("\t\t"));
      Serial.print(input_data[input_counter]);
      input_counter++;
      Serial.print(F("\t\t"));
      Serial.print(target_data[i]);
      Serial.print(F("\t\t"));
      Serial.println(output_data[i], 5);
      
    }
  }
  
  // Functions for communication between the cores
  // M4 can check if a training task is there
  int check_train_task() {
    return train_task_flag;
  }

  // M4 can check if training has been done before
  int check_net_trained() {
    return net_trained_flag;
  }

  // M4 can signal that he has finished training
  int net_trained() {
    net_trained_flag = 1;
    train_task_flag = 0;
    return net_trained_flag;
  }

  // M4 can read a M7 weight
  float get_weight(int index) {
    return M7_weights[index];
  }

  // M4 can set a M7 weight
  float set_weight(int index, float value) {
    M7_weights[index] = (float) value;
    return value;
  }
  
#endif

// Global variables and functions for M4
#ifdef CORE_CM4

  // Storage area of the weights for the training. They are transferred to the memory area of the M7 after training
  float M4_weights[WEIGHTS] = {0};

  // Stores the needed epochs
  uint32_t M4_epoch_counter = 0;
  // Stores the loss
  float M4_actual_loss = 0;

  // The print function for the loss. Nothing is printed but the intermediate results are saved
  void printLoss(float loss)
  {
      M4_epoch_counter = M4_epoch_counter + 1;
      M4_actual_loss = loss;
  }

  // The training function of the M4. Here only the variables for the training are needed.
  int M4_train() {
    uint32_t i = 0;
    AIFES_E_init_weights_parameter_fnn_f32  FNN_INIT_WEIGHTS;
    AIFES_E_training_parameter_fnn_f32  FNN_TRAIN;

    // Delete old training results
    M4_epoch_counter = 0;
    M4_actual_loss = 0;

    // Training parameters that always remain the same
    FNN_TRAIN.optimizer = AIfES_E_adam;                     // Adam optimizer
    FNN_TRAIN.loss = AIfES_E_mse;                           // MSE loss
    FNN_TRAIN.batch_size = DATASETS;                        // Here a full batch
    FNN_TRAIN.epochs = 1000;                                // Number of epochs
    FNN_TRAIN.sgd_momentum = 0.0;                           // Only interesting for SGD
    FNN_TRAIN.epochs_loss_print_interval = PRINT_INTERVAL;  // Store the loss every x times
    FNN_TRAIN.loss_print_function = printLoss;              // Individual print function
    FNN_TRAIN.early_stopping = AIfES_E_early_stopping_on;   // Use early stopping 

    // Check if the FNN has already been trained
    auto res0 = RPC1.call("check_net_trained").as<int>();
    
    // It has not been trained yet. Initialize the weights.
    if(res0 == 0)
    {
      FNN_INIT_WEIGHTS.init_weights_method = AIfES_E_init_uniform;
      FNN_INIT_WEIGHTS.min_init_uniform = -2; // only for the AIfES_E_init_uniform
      FNN_INIT_WEIGHTS.max_init_uniform = 2;  // only for the AIfES_E_init_uniform  
      
      FNN_TRAIN.learn_rate = 0.05f;                 // Learning rate relatively high for fast training
      FNN_TRAIN.early_stopping_target_loss = 0.004; // Target loss  
    }

    // It has already been trained but the FNN will be further trained
    if(res0 == 1)
    {
      FNN_INIT_WEIGHTS.init_weights_method = AIfES_E_init_no_init; // Use the old weights as
      FNN_TRAIN.learn_rate = 0.01f;                                // Reduce the learning rate
      FNN_TRAIN.early_stopping_target_loss = 0.002;                // Reduce the target loss

      // Copy the current weights from the M7
      for (i = 0; i < WEIGHTS; i++) {
        auto res1 = RPC1.call("get_weight", i).as<float>();
        M4_weights[i] = res1;
      }
    }

    int8_t error = 0;

    // Do the training
    error = AIFES_E_training_fnn_f32(&input_tensor,&target_tensor,&FNN,&FNN_TRAIN,&FNN_INIT_WEIGHTS,&output_tensor);

    // Send the current weights to the M7
    for (i = 0; i < WEIGHTS; i++) {
      auto res2 = RPC1.call("set_weight", i,M4_weights[i]).as<float>();
    }   

    // Set the flag so that the M7 knows that has been trained
    auto res2 = RPC1.call("net_trained").as<int>();

    // Print the results
    if(res0 == 0)
    {
      RPC1.print("M4 Core: Training from scratch finished after " + String(M4_epoch_counter * PRINT_INTERVAL) + " epochs and an loss of ");
      RPC1.print(M4_actual_loss, 6);
      RPC1.print(" (learning rate: ");
      RPC1.print(FNN_TRAIN.learn_rate, 5);
      RPC1.println(")");
    }
    if(res0 == 1)
    {
      RPC1.print("M4 Core: Continued training with " + String(M4_epoch_counter * PRINT_INTERVAL) + " epochs and an loss of ");
      RPC1.print(M4_actual_loss, 6);
      RPC1.print(" (learning rate: ");
      RPC1.print(FNN_TRAIN.learn_rate, 5);
      RPC1.println(")");
    }
    return 0;
  }

#endif



void setup() {

  // Setup for M7 and M4
   randomSeed(analogRead(A0));
   RPC1.begin();
   
  // Configure AIfES activations
  FNN_activations[0] = AIfES_E_sigmoid; // Sigmoid for hidden (dense) layer
  FNN_activations[1] = AIfES_E_sigmoid; // Sigmoid for output (dense) layer
  
  // Configure AIfES model        
  FNN.layer_count = FNN_3_LAYERS;
  FNN.fnn_structure = FNN_structure;
  FNN.fnn_activations = FNN_activations;

  // M7 setup
  #ifdef CORE_CM7  
    LL_RCC_ForceCM4Boot();  
    global_LED = LEDB; // M7 uses the blue LED
    Serial.begin(115200);

    // M7 CPU makes the functions available under the defined names
    RPC1.bind("check_train_task", check_train_task);
    RPC1.bind("check_net_trained", check_net_trained);
    RPC1.bind("net_trained", net_trained); 
    RPC1.bind("get_weight", get_weight); 
    RPC1.bind("set_weight", set_weight);

    // The M7 uses its own weights, which are updated from the M4
    FNN.flat_weights = M7_weights;
    
    while (!Serial);

    // Print the introduction
    Serial.println(F("AIfES-Express Portenta H7 Dual-Core demo"));
    Serial.println(F("M7 blue LED / M4 green LED"));
    Serial.println(F("Possible commands:"));
    Serial.println(F(">inference< The M7 performs an inference (The FNN must be trained previously)"));
    Serial.println(F(">training< The M7 tells the M4 to train (Further training possible by repeating the command)"));
    Serial.println(F(">reset< The M7 resets the trained weights"));
         
  #endif

  // M4 setup
  #ifdef CORE_CM4  
     global_LED = LEDG; // M4 uses the green LED

     // The M4 uses its own weights for training
     FNN.flat_weights = M4_weights;
  #endif   
   
  pinMode(global_LED, OUTPUT);
}

void loop() {
   // LED signal of the cores
   digitalWrite(global_LED, LOW); 
   delay(200); 
   digitalWrite(global_LED, HIGH); 
   
   
   #ifdef CORE_CM4  
    
     //int myRand = rand() % 100;  // from 0 to 99
     //int myRand = 0;
     delay(50); 
     auto res = RPC1.call("check_train_task").as<int>();
     
     if (res == 0)
     {
        //RPC1.println("From M4 Core: No task");
     }

     if (res == 1)
     {
        RPC1.println("M4 Core: I start training");
        M4_train();
     }

     
     
   
   #endif   
  
   #ifdef CORE_CM7   
     while(Serial.available() > 0 ){
      String str = Serial.readString();
        if(str.indexOf("inference") > -1){
          if(net_trained_flag == 0)
          {
            Serial.println("M7 Core: FNN not trained");
            //M7_inference();
          }
          else
          {
            Serial.println("M7 Core: I do the inference");
            M7_inference();
          }          
        }
        else if(str.indexOf("training") > -1){
          train_task_flag = 1;
          Serial.println("M7 Core: I tell the M4 that he should train");
        }
        else if(str.indexOf("reset") > -1){       
          net_trained_flag = 0;
          Serial.println("M7 Core: I reset the weights");
        }
        else{
          Serial.println("M7 Core: unknown command");
        }
     }
     
      delay(50);
     while (RPC1.available()) {
       Serial.write(RPC1.read()); // check if the M4 has sent an RPC println
     }
   #endif  

   delay(100); // Wait for a random amount of time below 6 seconds.
                  // Code would be better using a timer instead of a delay
}
