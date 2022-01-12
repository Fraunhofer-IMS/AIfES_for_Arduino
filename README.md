[![Docs](https://img.shields.io/badge/Docs-Doxygen-green)](https://fraunhofer-ims.github.io/AIfES_for_Arduino/)

# AIfES for Arduino®

AIfES (Artificial Intelligence for Embedded Systems) is a platform-independent and standalone AI software framework optimized for embedded systems.
The Feedforward Neural Networks (FNN) implemented in AIfES can be freely parameterized, trained, modified or reloaded at runtime.
In this version, it is optimized for the Arduino IDE and compatible to almost any Arduino board.
AIfES is developed in the C programming language and uses only standard libraries based on the GNU Compiler Collection (GCC).
AIfES thus runs on almost any hardware from 8-bit microcontrollers over Raspberry PI to smartphones or PCs.
Not only inference of FNN is possible, but also training directly in the device. Furthermore, compatibility to other AI software frameworks such as Keras or TensorFlow is also given.

AIfES was developed by the Fraunhofer Institute for Microelectronic Circuits and Systems IMS. Additional information can be found at <www.aifes.ai>

The Fraunhofer IMS with AIfES and Arduino preapare to enter a partnership.

We are also at the Arduino Project Hub. Here you can find our new AIfES projects and examples:
https://create.arduino.cc/projecthub/aifes_team

Follow us on LinkedIn to get all the latest news about AIfES:
https://www.linkedin.com/showcase/aifes-ai

## Dual License
### For Maker and Open-Source-Projects (GNU AGPLv3):
For private projects or developers of Free Open Source Software (FOSS) under version 3 of the GNU Affero General Public License (AGPL), the AIfES version offered here can be used free of charge.

### For use in commercial applications:
If AIfES is to be combined and distributed with commercially licensed software and/or if you do not wish to distribute the AIfES source code
for the commercially licensed software under version 3 of the GNU Affero General Public License (AGPL), you must conclude a license agreement with Fraunhofer IMS.
For more information and contact, refer to our [homepage](<https://www.ims.fraunhofer.de/en/Business-Unit/Industry/Industrial-AI/Artificial-Intelligence-for-Embedded-Systems-AIfES/AIFES_Cooperation.html>)

## Compatibility
AIfES in the current version supports almost all microcontroller types and Arduino boards:
*	8-Bit-Microcontroller
*	16-Bit-Microcontroller
*	32-Bit-Microcontroller
*	64-Bit-Microcontroller

### ARM CMSIS
AIfES also supports the *Cortex Microcontroller Software Interface Standard* (CMSIS) of the ARM Cortex controllers.
This standard can speed up inference and training for large FNNs.
Please read the README.md in https://github.com/Fraunhofer-IMS/AIfES_for_Arduino/tree/main/src/CMSIS for more information on how to add the CMSIS library.

### Python
To help you with the quantization of a neural network, we provide some Python tools. You can install them via pip with:

```pip install https://github.com/Fraunhofer-IMS/AIfES_for_Arduino/raw/main/etc/python/aifes_tools.zip```

Please read the README.md in https://github.com/Fraunhofer-IMS/AIfES_for_Arduino/tree/main/etc/python for more information
on the AIfES pytools.

We also created some examples to show you how a tf.Keras and a PyTorch neural network can be quantized for AIfES, using the pytools
https://github.com/Fraunhofer-IMS/AIfES_for_Arduino/tree/main/etc/python/examples.

## Features

Have a look at our [**documentation**](https://fraunhofer-ims.github.io/AIfES_for_Arduino/#OverviewFeatures) for a detailed overview of the provided features with code examples.

### Data types and quantization
AIfES supports the following data types:
* F32:	32 Bit float
* Q31:	32 Bit integer
* Q7: 	 8 Bit integer

Neural network types
AIfES supports in the current version:
*	Feedforward Neural Network (FNN) inference and training
*	Other network types are in progress (see roadmap)

The number of neurons and the number of different layers can be adapted individually. Inference and training can be performed on the controller.

**Inference layer**

| Layer      | f32     | q31     | q7      |
|------------|---------|---------|---------|
| Dense | ailayer_dense_f32_default()<br>ailayer_dense_f32_cmsis()<br>ailayer_dense_f32_avr_pgm() | ailayer_dense_q31_default() | ailayer_dense_q7_default()<br>ailayer_dense_wt_q7_default()<br>ailayer_dense_wt_q7_cmsis()<br>ailayer_dense_q7_avr_pgm()<br>ailayer_dense_wt_q7_avr_pgm() |
| Input | ailayer_input_f32_default() | ailayer_input_q31_default() | ailayer_input_q7_default() |
| ReLU | ailayer_relu_f32_default() | ailayer_relu_q31_default() | ailayer_relu_q7_default()<br>ailayer_relu_q7_avr_pgm() |
| Sigmoid | ailayer_sigmoid_f32_default() | ailayer_sigmoid_q31_default() | ailayer_sigmoid_q7_default()<br>ailayer_sigmoid_q7_avr_pgm() |
| Softmax | ailayer_softmax_f32_default() | ailayer_softmax_q31_default() | ailayer_softmax_q7_default()<br>ailayer_softmax_q7_avr_pgm() |
| Leaky ReLU | ailayer_leaky_relu_f32_default() | ailayer_leaky_relu_q31_default()| ailayer_leaky_relu_q7_default()<br>ailayer_leaky_relu_q7_avr_pgm() |
| ELU | ailayer_elu_f32_default() | ailayer_elu_q31_default() | ailayer_elu_q7_default()<br>ailayer_elu_q7_avr_pgm() |
| Tanh | ailayer_tanh_f32_default() | ailayer_tanh_q31_default() | ailayer_tanh_q7_default()<br>ailayer_tanh_q7_avr_pgm() |
| Softsign | ailayer_softsign_f32_default() | ailayer_softsign_q31_default()| ailayer_softsign_q7_default()<br>ailayer_softsign_q7_avr_pgm() |

**Training layer**

| Layer      | f32     | q31     | q7      |
|------------|---------|---------|---------|
| Dense | ailayer_dense_f32_default()<br>ailayer_dense_f32_cmsis()<br>ailayer_dense_f32_avr_pgm() | ailayer_dense_q31_default() |         |
| Input | ailayer_input_f32_default() | ailayer_input_q31_default() | ailayer_input_q7_default() |
| ReLU | ailayer_relu_f32_default() | ailayer_relu_q31_default() | ailayer_relu_q7_default() |
| Sigmoid | ailayer_sigmoid_f32_default() | ailayer_sigmoid_q31_default() |         |
| Softmax | ailayer_softmax_f32_default() | ailayer_softmax_q31_default()|         |
| Leaky ReLU | ailayer_leaky_relu_f32_default() | ailayer_leaky_relu_q31_default() |         |
| ELU | ailayer_elu_f32_default() | ailayer_elu_q31_default()|         |
| Tanh | ailayer_tanh_f32_default() | ailayer_tanh_q31_default() |         |
| Softsign | ailayer_softsign_f32_default() | ailayer_softsign_q31_default() |         |

**Loss:**

| Loss                     | f32     | q31     | q7      |
|--------------------------|---------|---------|---------|
| Mean Squared Error (MSE) | ailoss_mse_f32_default() | ailoss_mse_q31_default() |         |
| Crossentropy | ailoss_crossentropy_f32_default()<br>ailoss_crossentropy_sparse8_f32_default() |         |         |

**Optimizer:**

| Optimizer                         | f32     | q31     | q7      |
|-----------------------------------|---------|---------|---------|
| Stochastic Gradient Descent (SGD) | aiopti_sgd_f32_default() | aiopti_sgd_q31_default() |         |
| Adam | aiopti_adam_f32_default() |         |         |

## Installation
You can download and install AIfES® (search for "aifes") with the Arduino library manager.
https://www.arduino.cc/en/guide/libraries

Alternatively, the manual download is also possible:
Download the AIfES repository as a ZIP archive and follow these instructions:
<https://www.arduino.cc/en/guide/libraries>

## Roadmap
The AIfES team at Fraunhofer IMS is constantly working on new features and network types. The next feature you can look forward to is:

### Convolutional Neural Networks (ConvNet) 
Soon, classical ConvNets can be executed and trained in AIfES.
