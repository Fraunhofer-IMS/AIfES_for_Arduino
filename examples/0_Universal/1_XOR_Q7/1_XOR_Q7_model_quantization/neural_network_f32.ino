

// ---------------------------------- F32 model (source) ---------------------------------------

aimodel_t nn_f32;  // AIfES model

// Input layer
uint16_t input_layer_shape[] = {1, 2};      // Definition of the input layer shape (Must fit to the input tensor. The first dimension is the batch size.)
ailayer_input_f32_t input_layer_f32 = AILAYER_INPUT_F32_M( /*input dimension=*/ 2, /*input shape=*/ input_layer_shape);   // Creation of the AIfES input layer

// Hidden dense layer
float weights_data_dense_1[] = {-10.1164f, -8.4212f, 5.4396f, 7.297f, -7.6482f, -9.0155f};  // Hidden layer weights 
float bias_data_dense_1[] = {-2.9653f,  2.3677f, -1.5968f};                                 // Hidden layer bias weights 
ailayer_dense_f32_t dense_layer_f32_1 = AILAYER_DENSE_F32_M( /*neurons=*/ 3, /*weights=*/ weights_data_dense_1, /*bias=*/ bias_data_dense_1); // Creation of the AIfES hidden dense layer with 3 neurons

// Hidden layer activation function
ailayer_sigmoid_f32_t sigmoid_layer_f32_1 = AILAYER_SIGMOID_F32_M();

// Output dense layer
float weights_data_dense_2[] = {12.0305f, -6.5858f, 11.9371f};  // Output dense layer weights
float bias_data_dense_2[] = {-5.4247f};                         // Output dense layer bias weights
ailayer_dense_f32_t dense_layer_f32_2 = AILAYER_DENSE_F32_M( /*neurons=*/ 1, /*weights=*/ weights_data_dense_2, /*bias=*/ bias_data_dense_2); // Creation of the AIfES output dense layer with 1 neuron

// Output layer activation function
ailayer_sigmoid_f32_t sigmoid_layer_f32_2 = AILAYER_SIGMOID_F32_M();


byte *memory_ptr_f32;

aimodel_t *init_model_f32()
{
  ailayer_t *x;     // Layer object from AIfES to connect the layers
  
  // Connect the layers to an AIfES model
  nn_f32.input_layer = ailayer_input_f32_default(&input_layer_f32);
  x = ailayer_dense_f32_default(&dense_layer_f32_1, nn_f32.input_layer);
  x = ailayer_sigmoid_f32_default(&sigmoid_layer_f32_1, x);
  x = ailayer_dense_f32_default(&dense_layer_f32_2, x);
  nn_f32.output_layer = ailayer_sigmoid_f32_default(&sigmoid_layer_f32_2, x);
  
  aialgo_compile_model(&nn_f32); // Compile the AIfES model
  
  // ------------------------------------- Print the model structure ------------------------------------
  
  Serial.println(F("\n-------------- F32 model structure ---------------"));
  aialgo_print_model_structure(&nn_f32);
  Serial.println(F("----------------------------------------------\n"));
  
  // -------------------------------- Allocate and schedule the working memory for inference ---------
  
  // Allocate memory for result and temporal data
  uint32_t memory_size_f32 = aialgo_sizeof_inference_memory(&nn_f32);
  Serial.print(F("Required memory for intermediate results of f32 model: "));
  Serial.print(memory_size_f32);
  Serial.print(F(" bytes"));
  Serial.println();
  memory_ptr_f32 = (byte *) malloc(memory_size_f32);
  
  // Schedule the memory over the model
  aialgo_schedule_inference_memory(&nn_f32, memory_ptr_f32, memory_size_f32);

  return &nn_f32;
}

void print_model_f32_parameters()
{
  Serial.println(F("F32 model parameters:"));
  Serial.println(F("Dense 1 - Weights:"));
  print_aitensor(&dense_layer_f32_1.weights);
  Serial.println(F("Dense 1 - Bias:"));
  print_aitensor(&dense_layer_f32_1.bias);
  Serial.println(F("Dense 2 - Weights:"));
  print_aitensor(&dense_layer_f32_2.weights);
  Serial.println(F("Dense 2 - Bias:"));
  print_aitensor(&dense_layer_f32_2.bias);
}

void free_model_f32()
{
  free(memory_ptr_f32);
}
