/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "AIfES 2", "index.html", [
    [ "Main page", "index.html", [
      [ "Vocabulary and Abbreviations", "index.html#autotoc_md0", null ],
      [ "Overview (Documentation)", "index.html#autotoc_md1", null ],
      [ "Overview (Features)", "index.html#OverviewFeatures", [
        [ "Data types", "index.html#autotoc_md2", null ],
        [ "General", "index.html#autotoc_md3", null ],
        [ "For Inference", "index.html#OverviewInference", null ],
        [ "For Training", "index.html#OverviewTraining", null ],
        [ "AIfES Express", "index.html#OverviewExpress", null ]
      ] ],
      [ "Python tools", "index.html#autotoc_md4", null ],
      [ "Structure and design concepts of AIfES", "index.html#autotoc_md5", null ]
    ] ],
    [ "Tutorial inference F32", "_tutorial_inference_f32.html", [
      [ "Example", "_tutorial_inference_f32.html#autotoc_md6", null ],
      [ "Design the neural network", "_tutorial_inference_f32.html#autotoc_md7", null ],
      [ "Get the pre-trained weights and biases", "_tutorial_inference_f32.html#autotoc_md8", null ],
      [ "Create the neural network in AIfES", "_tutorial_inference_f32.html#autotoc_md9", [
        [ "Declaration and configuration of the layers", "_tutorial_inference_f32.html#autotoc_md10", null ],
        [ "Connection and initialization of the layers", "_tutorial_inference_f32.html#autotoc_md11", null ],
        [ "Print the layer structure to the console", "_tutorial_inference_f32.html#autotoc_md12", null ]
      ] ],
      [ "Perform the inference", "_tutorial_inference_f32.html#autotoc_md13", [
        [ "Allocate and initialize the working memory", "_tutorial_inference_f32.html#autotoc_md14", null ],
        [ "Run the inference", "_tutorial_inference_f32.html#autotoc_md15", null ]
      ] ]
    ] ],
    [ "Tutorial training F32", "_tutorial_training_f32.html", [
      [ "Example", "_tutorial_training_f32.html#autotoc_md16", null ],
      [ "Design the neural network", "_tutorial_training_f32.html#autotoc_md17", null ],
      [ "Create the neural network in AIfES", "_tutorial_training_f32.html#autotoc_md18", [
        [ "Declaration and configuration of the layers", "_tutorial_training_f32.html#autotoc_md19", null ],
        [ "Connection and initialization of the layers", "_tutorial_training_f32.html#autotoc_md20", null ],
        [ "Set the memory for the trainable parameters", "_tutorial_training_f32.html#autotoc_md21", null ],
        [ "Print the layer structure to the console", "_tutorial_training_f32.html#autotoc_md22", null ]
      ] ],
      [ "Train the neural network", "_tutorial_training_f32.html#autotoc_md23", [
        [ "Configure the loss", "_tutorial_training_f32.html#autotoc_md24", null ],
        [ "Configure the optimizer", "_tutorial_training_f32.html#autotoc_md25", null ],
        [ "Initialize the trainable parameters", "_tutorial_training_f32.html#autotoc_md26", null ],
        [ "Allocate and initialize the working memory", "_tutorial_training_f32.html#autotoc_md27", null ],
        [ "Perform the training", "_tutorial_training_f32.html#autotoc_md28", null ]
      ] ],
      [ "Test the trained model", "_tutorial_training_f32.html#autotoc_md29", null ],
      [ "Change working memory", "_tutorial_training_f32.html#autotoc_md30", null ]
    ] ],
    [ "Tutorial inference Q7", "_tutorial_inference_q7.html", [
      [ "Example", "_tutorial_inference_q7.html#autotoc_md31", null ],
      [ "Design the neural network", "_tutorial_inference_q7.html#autotoc_md32", null ],
      [ "Get the pre-trained weights and biases", "_tutorial_inference_q7.html#GetWeightsAndBiases", null ],
      [ "Q7 quantization", "_tutorial_inference_q7.html#autotoc_md33", [
        [ "General", "_tutorial_inference_q7.html#autotoc_md34", null ],
        [ "Tensor quantization helper", "_tutorial_inference_q7.html#autotoc_md35", null ]
      ] ],
      [ "Manual quantization of the neural network", "_tutorial_inference_q7.html#autotoc_md36", [
        [ "Quantization of the intermediate results", "_tutorial_inference_q7.html#autotoc_md37", null ],
        [ "Quantization of the weights and biases", "_tutorial_inference_q7.html#autotoc_md38", null ]
      ] ],
      [ "Create the neural network in AIfES", "_tutorial_inference_q7.html#autotoc_md39", [
        [ "Manual declaration and configuration of the layers", "_tutorial_inference_q7.html#ManualLayerDeclaration", null ],
        [ "Automatic declaration and configuration of the layers", "_tutorial_inference_q7.html#AutomaticLayerDeclaration", null ],
        [ "Connection and initialization of the layers", "_tutorial_inference_q7.html#autotoc_md40", null ],
        [ "Print the layer structure to the console", "_tutorial_inference_q7.html#autotoc_md41", null ]
      ] ],
      [ "Perform the inference", "_tutorial_inference_q7.html#autotoc_md42", [
        [ "Allocate and initialize the working memory", "_tutorial_inference_q7.html#autotoc_md43", null ],
        [ "Run the inference", "_tutorial_inference_q7.html#RunInferenceQ7", null ]
      ] ],
      [ "Automatic quantization in AIfES", "_tutorial_inference_q7.html#AutomaticQuantizationAIfES", null ],
      [ "Automatic quantization in Python", "_tutorial_inference_q7.html#AutomaticQuantizationPython", null ]
    ] ],
    [ "Data Structures", "annotated.html", [
      [ "Data Structures", "annotated.html", "annotated_dup" ],
      [ "Data Structure Index", "classes.html", null ],
      [ "Data Fields", "functions.html", [
        [ "All", "functions.html", "functions_dup" ],
        [ "Variables", "functions_vars.html", "functions_vars" ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ],
      [ "Globals", "globals.html", [
        [ "All", "globals.html", "globals_dup" ],
        [ "Functions", "globals_func.html", "globals_func" ],
        [ "Variables", "globals_vars.html", null ],
        [ "Typedefs", "globals_type.html", null ],
        [ "Enumerations", "globals_enum.html", null ],
        [ "Enumerator", "globals_eval.html", null ]
      ] ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"_tutorial_inference_f32.html",
"ailayer__sigmoid_8h.html#ac8b3179554261f9cb3dbf4036e18a265",
"aimath__q7__avr__pgm_8h.html#ad4d2f63c565e7d4b9ac2cf7aef6c4844",
"structailayer__leaky__relu.html"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';