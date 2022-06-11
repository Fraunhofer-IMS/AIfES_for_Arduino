/**
  * This file contains the neural network
  *
  */
  
#include "weights.h"

aimodel_t model;

// Layer definition
uint16_t input_layer_shape[] = {1, 1, 4, 3}; // [batch-size, channels, height, width] <- channels first format
ailayer_input_f32_t input_layer         = AILAYER_INPUT_F32_M(4, input_layer_shape);
ailayer_conv2d_t conv2d_layer_1         = AILAYER_CONV2D_F32_M(
                                                                  /* filters =*/     2,
                                                                  /* kernel_size =*/ HW(2, 2),
                                                                  /* stride =*/      HW(1, 1),
                                                                  /* dilation =*/    HW(1, 1),
                                                                  /* padding =*/     HW(0, 0),
                                                                  /* weights =*/     kernel_data_conv2d_1,
                                                                  /* bias =*/        bias_data_conv2d_1
                                                                );

ailayer_sigmoid_f32_t sigmoid_layer_1   = AILAYER_SIGMOID_F32_M();
ailayer_flatten_t flatten_layer         = AILAYER_FLATTEN_F32_M();
ailayer_dense_f32_t dense_layer_1       = AILAYER_DENSE_F32_M(
                                                                  /* neurons =*/  1,
                                                                  /* weights =*/  weights_data_dense_1,
                                                                  /* bias =*/     bias_data_dense_1
                                                              );
ailayer_sigmoid_f32_t sigmoid_layer_2   = AILAYER_SIGMOID_F32_M();

void *inference_memory;

aimodel_t *init_neural_network()
{
  // Layer pointer to perform the connection
  ailayer_t *x;

  // The channels first related functions ("chw" or "cfirst") are used, because the input data is given as channels first format.
  model.input_layer = ailayer_input_f32_default(&input_layer);
	x = ailayer_conv2d_chw_f32_default(&conv2d_layer_1, model.input_layer);
	x = ailayer_sigmoid_f32_default(&sigmoid_layer_1, x);
	x = ailayer_flatten_f32_default(&flatten_layer, x);
	x = ailayer_dense_f32_default(&dense_layer_1, x);
	x = ailayer_sigmoid_f32_default(&sigmoid_layer_2, x);
	model.output_layer = x;

  // Finish the model creation by checking the connections and setting some parameters for further processing
  aialgo_compile_model(&model);

  Serial.print(F("\n-------------- Neural network model structure ---------------\n"));
  aialgo_print_model_structure(&model);
  Serial.print(F("--------------------------------------------------------\n\n"));

  // Allocate memory for intermediate results of the inference
  uint32_t inference_memory_size = aialgo_sizeof_inference_memory(&model);
  Serial.print("The model needs "); Serial.print(inference_memory_size); Serial.println(" bytes of memory for inference.\n");
  inference_memory = malloc(inference_memory_size);

  // Schedule the memory to the model
  aialgo_schedule_inference_memory(&model, inference_memory, inference_memory_size);

  return &model;
}
