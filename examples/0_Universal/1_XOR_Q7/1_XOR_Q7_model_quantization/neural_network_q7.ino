

 // ---------------------------------- Q7 model (target) ---------------------------------------

aimodel_t nn_q7;  // AIfES model

ailayer_input_q7_t    input_layer_q7     = AILAYER_INPUT_Q7_A( /*input dimension=*/ 2, /*input shape=*/ input_layer_shape);   // Creation of the AIfES input layer
ailayer_dense_q7_t    dense_layer_q7_1   = AILAYER_DENSE_Q7_A( /*neurons=*/ dense_layer_f32_1.neurons); // Creation of the AIfES hidden dense layer with 3 neurons (like in the F32 model)
ailayer_sigmoid_q7_t  sigmoid_layer_q7_1 = AILAYER_SIGMOID_Q7_A(); // Hidden activation function
ailayer_dense_q7_t    dense_layer_q7_2   = AILAYER_DENSE_Q7_A( /*neurons=*/ dense_layer_f32_2.neurons); // Creation of the AIfES output dense layer with 1 neuron (like in the F32 model)
ailayer_sigmoid_q7_t  sigmoid_layer_q7_2 = AILAYER_SIGMOID_Q7_A(); // Output activation function


byte *memory_ptr_q7;
byte *parameter_memory_q7;

aimodel_t *init_model_q7()
{
  ailayer_t *x;
  
  // Connect the layers to an AIfES model
  nn_q7.input_layer = ailayer_input_q7_default(&input_layer_q7);
  x = ailayer_dense_q7_default(&dense_layer_q7_1, nn_q7.input_layer);
  x = ailayer_sigmoid_q7_default(&sigmoid_layer_q7_1, x);
  x = ailayer_dense_q7_default(&dense_layer_q7_2, x);
  nn_q7.output_layer = ailayer_sigmoid_q7_default(&sigmoid_layer_q7_2, x);
  
  aialgo_compile_model(&nn_q7); // Compile the AIfES model
  
  // ------------------------------------- Print the model structure ------------------------------------
  
  Serial.println(F("\n-------------- Q7 model structure ---------------"));
  aialgo_print_model_structure(&nn_q7);
  Serial.println(F("----------------------------------------------\n"));
  
  // ------------------------------- Allocate memory for the parameters of the model ------------------------------
  
  uint32_t parameter_memory_size_q7 = aialgo_sizeof_parameter_memory(&nn_q7);
  Serial.print(F("Required memory for parameter (Weights, Biases) of q7 model: "));
  Serial.print(parameter_memory_size_q7);
  Serial.print(F(" bytes"));
  Serial.println();
  parameter_memory_q7 = (byte *) malloc(parameter_memory_size_q7);
  
  // Distribute the memory for the trainable parameters of the model
  aialgo_distribute_parameter_memory(&nn_q7, parameter_memory_q7, parameter_memory_size_q7);
  
  // -------------------------------- Allocate and schedule the working memory for inference ---------
  
  // Allocate memory for result and temporal data
  uint32_t memory_size_q7 = aialgo_sizeof_inference_memory(&nn_q7);
  Serial.print(F("Required memory for intermediate results of q7 model: "));
  Serial.print(memory_size_q7);
  Serial.print(F(" bytes"));
  Serial.println();
  memory_ptr_q7 = (byte *) malloc(memory_size_q7);
  
  // Schedule the memory over the model
  aialgo_schedule_inference_memory(&nn_q7, memory_ptr_q7, memory_size_q7);

  return &nn_q7;
}

void print_model_q7_parameters()
{
  Serial.println(F("Q7 model parameters:"));
  Serial.println(F("Dense 1 - Weights:"));
  print_aitensor(&dense_layer_q7_1.weights);
  Serial.println(F("Dense 1 - Bias:"));
  print_aitensor(&dense_layer_q7_1.bias);
  Serial.println(F("Dense 2 - Weights:"));
  print_aitensor(&dense_layer_q7_2.weights);
  Serial.println(F("Dense 2 - Bias:"));
  print_aitensor(&dense_layer_q7_2.bias);
}

void free_model_q7()
{
  free(memory_ptr_q7);
  free(parameter_memory_q7);
}
