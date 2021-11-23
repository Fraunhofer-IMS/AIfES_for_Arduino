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
### For Maker and Open-Source-Projects (GNU GPLv3):
For private projects or developers of Free Open Source Software (FOSS) under version 3 of the GNU General Public License (GPL), the AIfES version offered here can be used free of charge.

### For use in commercial applications:
If AIfES is to be combined and distributed with commercially licensed software and/or if you do not wish to distribute the AIfES source code
for the commercially licensed software under version 3 of the GNU General Public License (GPL), you must conclude a license agreement with Fraunhofer IMS.
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
Please install the *Arduino_CMSIS-DSP* library in the Arduino IDE.


## Features
### Data types and quantization
AIfES supports the following data types:
F32:	32 Bit float

We are currently working on Q31 (32 Bit integer) and Q7 (8 Bit integer) support. We will release these features in the near future. Stay tuned!

Neural network types
AIfES supports in the current version:
*	Feedforward Neural Network (FNN) inference and training
*	Other network types are in progress (see roadmap)

The number of neurons and the number of different layers can be adapted individually. Inference and training can be performed on the controller.

**Inference layer**

| Layer      | f32     |
|------------|---------|
| ailayer_dense.h Dense | ailayer_dense_f32_default()<br>ailayer_dense_f32_cmsis() |
| ailayer_input.h Input | ailayer_input_f32_default() |
| ailayer_relu.h ReLU | ailayer_relu_f32_default() |
| ailayer_sigmoid.h Sigmoid | ailayer_sigmoid_f32_default() |
| ailayer_softmax.h Softmax | ailayer_softmax_f32_default() |
| ailayer_leaky_relu.h Leaky ReLU | ailayer_leaky_relu_f32_default() |
| ailayer_elu.h ELU | ailayer_elu_f32_default() |
| ailayer_tanh.h Tanh | ailayer_tanh_f32_default() |
| ailayer_softsign.h Softsign | ailayer_softsign_f32_default() |

**Training layer**

| Layer      | f32     |
|------------|---------|
| ailayer_dense.h Dense | ailayer_dense_f32_default()<br>ailayer_dense_f32_cmsis() |
| ailayer_input.h Input | ailayer_input_f32_default() |
| ailayer_relu.h ReLU | ailayer_relu_f32_default() |
| ailayer_sigmoid.h Sigmoid | ailayer_sigmoid_f32_default() |
| ailayer_softmax.h Softmax | ailayer_softmax_f32_default() |
| ailayer_leaky_relu.h Leaky ReLU | ailayer_leaky_relu_f32_default() |
| ailayer_elu.h ELU | ailayer_elu_f32_default() |
| ailayer_tanh.h Tanh | ailayer_tanh_f32_default() |
| ailayer_softsign.h Softsign | ailayer_softsign_f32_default() |

**Loss**

| Loss                     | f32     |
|--------------------------|---------|
| ailoss_mse.h Mean Squared Error (MSE) | ailoss_mse_f32_default() |
| ailoss_crossentropy.h Crossentropy | ailoss_crossentropy_f32_default() |

**Optimizer**

| Optimizer                         | f32     |
|-----------------------------------|---------|
| aiopti_sgd.h Stochastic Gradient Descent (SGD) | aiopti_sgd_f32_default() |
| aiopti_adam.h Adam | aiopti_adam_f32_default() |

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
