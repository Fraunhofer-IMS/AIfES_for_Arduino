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

  AIfES CNN Training F32 Example
  --------------------

  Versions:
    1.0.0   Initial version
  
  The sketch shows an example of how the training of a convolutional neural network (CNN) is performed. The training data consists of images with horizontal and vertical stripes.
  This example is just for demonstration of the training procedure with a CNN and has no deeper meaning. 

  Tested on:
    Arduino Nano BLE 33
    Seeeduino XIAO
   
*/

#include <aifes.h>


void setup() {
  // Init serial port for communication
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect.
  }

  Serial.println(F("AIfES CNN Training F32 Example"));
  Serial.println(F("Type >training< (or anything else) to start"));
  while(!Serial.available());
  Serial.readString();
  Serial.println();

  uint32_t i;

	float input_data[] = {0.0f, 1.0f, 0.0f,
                        0.0f, 1.0f, 0.0f,
                        0.0f, 1.0f, 0.0f,
                        0.0f, 1.0f, 0.0f,

                        0.0f, 0.0f, 0.0f,
                        1.0f, 1.0f, 1.0f,
                        0.0f, 0.0f, 0.0f,
                        0.0f, 0.0f, 0.0f,

                        0.0f, 0.0f, 1.0f,
                        0.0f, 0.0f, 1.0f,
                        0.0f, 0.0f, 1.0f,
                        0.0f, 0.0f, 1.0f,

                        0.0f, 0.0f, 0.0f,
                        0.0f, 0.0f, 0.0f,
                        0.0f, 0.0f, 0.0f,
                        1.0f, 1.0f, 1.0f};
	uint16_t input_shape[] = {4, 1, 4, 3}; // [sample count, channels, height, width] <- channels first format
	aitensor_t input_tensor = AITENSOR_4D_F32(input_shape, input_data);

	float target_data[] = {0.0f,
                        1.0f,
                        0.0f,
                        1.0f};
	uint16_t target_shape[] = {4, 1}; // [batch-size, output neurons]
	aitensor_t target_tensor = AITENSOR_2D_F32(target_shape, target_data);


	float test_data[] = {1.0f, 0.0f, 0.0f,
                        1.0f, 0.0f, 0.0f,
                        1.0f, 0.0f, 0.0f,
                        1.0f, 0.0f, 0.0f,

                        0.0f, 1.0f, 0.0f,
                        0.0f, 1.0f, 0.0f,
                        0.0f, 1.0f, 0.0f,
                        0.0f, 1.0f, 0.0f,

                        1.0f, 1.0f, 1.0f,
                        0.0f, 0.0f, 0.0f,
                        0.0f, 0.0f, 0.0f,
                        0.0f, 0.0f, 0.0f,

                        0.0f, 0.0f, 0.0f,
                        0.0f, 0.0f, 0.0f,
                        1.0f, 1.0f, 1.0f,
                        0.0f, 0.0f, 0.0f};
	uint16_t test_shape[] = {4, 1, 4, 3}; // [sample count, channels, height, width] <- channels first format
	aitensor_t test_tensor = AITENSOR_4D_F32(test_shape, test_data);

	float test_target_data[] = {0.0f,
                              0.0f,
                              1.0f,
                              1.0f};
	uint16_t test_target_shape[] = {4, 1};
	aitensor_t test_target_tensor = AITENSOR_2D_F32(test_target_shape, test_target_data);

	// ---------------------------------- Layer definition ---------------------------------------

	uint16_t input_layer_shape[]            = {4, 1, 4, 3}; // [batch-size, channels, height, width] <- channels first format
	ailayer_input_f32_t input_layer         = AILAYER_INPUT_F32_A(4, input_layer_shape);
	ailayer_conv2d_t conv2d_layer_1         = AILAYER_CONV2D_F32_A(
                                                                     /* filters =*/     2,
                                                                     /* kernel_size =*/ HW(2, 2),
                                                                     /* stride =*/      HW(2, 1),
                                                                     /* dilation =*/    HW(1, 1),
                                                                     /* padding =*/     HW(1, 1)
                                                                    );
	ailayer_batch_norm_f32_t bn_layer_1     = AILAYER_BATCH_NORM_F32_A(/* momentum =*/ 0.9f, /* eps =*/ 1e-6f);
	ailayer_relu_f32_t relu_layer_1         = AILAYER_RELU_F32_A();
	ailayer_maxpool2d_t maxpool2d_layer_1   = AILAYER_MAXPOOL2D_F32_A(
                                                                     /* pool_size =*/   HW(2, 1),
                                                                     /* stride =*/      HW(2, 1),
                                                                     /* padding =*/     HW(0, 0)
                                                                    );
	ailayer_conv2d_t conv2d_layer_2         = AILAYER_CONV2D_F32_A(
                                                                     /* filters =*/     1,
                                                                     /* kernel_size =*/ HW(1, 1),
                                                                     /* stride =*/      HW(1, 1),
                                                                     /* dilation =*/    HW(1, 1),
                                                                     /* padding =*/     HW(0, 0)
                                                                    );
	ailayer_batch_norm_f32_t bn_layer_2     = AILAYER_BATCH_NORM_F32_A(/* momentum =*/ 0.9f, /* eps =*/ 1e-6f);
	ailayer_relu_f32_t relu_layer_2         = AILAYER_RELU_F32_A();
	ailayer_flatten_t flatten_layer         = AILAYER_FLATTEN_F32_A();
	ailayer_dense_f32_t dense_layer_1       = AILAYER_DENSE_F32_A(/* neurons =*/ 1);
	ailayer_sigmoid_f32_t sigmoid_layer_3   = AILAYER_RELU_F32_A();

	ailoss_mse_f32_t mse_loss;

	// --------------------------- Define the structure of the model ----------------------------

	aimodel_t model;
	ailayer_t *x;

  // The channels first related functions ("chw" or "cfirst") are used, because the input data is given as channels first format.
	model.input_layer = ailayer_input_f32_default(&input_layer);
	x = ailayer_conv2d_chw_f32_default(&conv2d_layer_1, model.input_layer);
  x = ailayer_batch_norm_cfirst_f32_default(&bn_layer_1, x);
	x = ailayer_relu_f32_default(&relu_layer_1, x);
	x = ailayer_maxpool2d_chw_f32_default(&maxpool2d_layer_1, x);
	x = ailayer_conv2d_chw_f32_default(&conv2d_layer_2, x);
	x = ailayer_batch_norm_cfirst_f32_default(&bn_layer_2, x);
	x = ailayer_relu_f32_default(&relu_layer_2, x);
	x = ailayer_flatten_f32_default(&flatten_layer, x);
	x = ailayer_dense_f32_default(&dense_layer_1, x);
	x = ailayer_sigmoid_f32_default(&sigmoid_layer_3, x);
	model.output_layer = x;

	model.loss = ailoss_mse_f32_default(&mse_loss, model.output_layer);

	aialgo_compile_model(&model);

	// ------------------------------------- Print the model structure ------------------------------------

  Serial.print("-------------- Model structure ---------------\n");
	aialgo_print_model_structure(&model);
  Serial.print("----------------------------------------------\n\n");

	// ------------------------------- Allocate memory for the parameters of the model ------------------------------

	uint32_t parameter_memory_size = aialgo_sizeof_parameter_memory(&model);
	Serial.print(F("Required memory for parameter (Weights, Bias, ...): ")); Serial.print(parameter_memory_size); Serial.print(F(" Byte\n"));
	void *parameter_memory = malloc(parameter_memory_size);

	// Distribute the memory to the trainable parameters of the model
	aialgo_distribute_parameter_memory(&model, parameter_memory, parameter_memory_size);

	// ------------------------------- Initialize the parameters ------------------------------

	srand(analogRead(A5));
	aialgo_initialize_parameters_model(&model);

	// -------------------------------- Define the optimizer for training ---------------------
    aiopti_t *optimizer;

/*
	aiopti_sgd_f32_t sgd_opti = AIOPTI_SGD_F32(1.0f);
	optimizer = aiopti_sgd_f32_default(&sgd_opti);
*/

	aiopti_adam_f32_t adam_opti = AIOPTI_ADAM_F32(0.01f, 0.9f, 0.999f, 1e-6f);
	optimizer = aiopti_adam_f32_default(&adam_opti);


	// -------------------------------- Allocate and schedule the working memory for training ---------

	uint32_t working_memory_size = aialgo_sizeof_training_memory(&model, optimizer);
	Serial.print(F("Required memory for training (Intermediate result, gradients, momentums): ")); Serial.print(working_memory_size); Serial.print(F(" Byte\n"));;
	void *working_memory = malloc(working_memory_size);

	// Schedule the memory over the model
	aialgo_schedule_training_memory(&model, optimizer, working_memory, working_memory_size);

	aialgo_init_model_for_training(&model, optimizer);

	// ------------------------------------- Run the training ------------------------------------

	float loss;
	uint32_t batch_size = 4;
	uint32_t epochs = 250;
  
  Serial.println(F("Start training."));
	for(i = 0; i < epochs; i++)
	{
		// One epoch of training. Iterates through the whole data once
		aialgo_train_model(&model, &input_tensor, &target_tensor, optimizer, batch_size);

		// Calculate and print loss every 10 epochs
		if(i % 10 == 0)
		{
			aialgo_calc_loss_model_f32(&model, &input_tensor, &target_tensor, &loss);
			Serial.print(F("Epoch ")); Serial.print(i); Serial.print(F(": loss: ")); Serial.println(loss, 5);
		}
	}

	// ----------------------------------------- Evaluate the trained model --------------------------

  // Create empty tensor for the results of the network
	float output_data[4*1];
	uint16_t output_shape[2] = {4, 1};
	aitensor_t output_tensor = AITENSOR_2D_F32(output_shape, output_data);

	Serial.print(F("\nResults after training:\n"));

	aialgo_inference_model(&model, &input_tensor, &output_tensor);

  Serial.println(F("\n+++ Training data +++"));
	Serial.print(F("Predicted values:\n"));
	print_aitensor(&output_tensor);
	Serial.print(F("Target values:\n"));
	print_aitensor(&target_tensor);

	aialgo_inference_model(&model, &test_tensor, &output_tensor);

  Serial.println(F("\n+++ Test data +++"));
	Serial.print(F("Predicted values:\n"));
	print_aitensor(&output_tensor);
	Serial.print(F("Target values:\n"));
	print_aitensor(&test_target_tensor);

	free(working_memory);
	free(parameter_memory);
	return;
}

void loop() {
  // Do nothing
}
