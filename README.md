# AIfES for Arduino®

AIfES (Artificial Intelligence for Embedded Systems) is a platform-independent and standalone AI software framework optimized for embedded systems.
The Feedforward Neural Networks (FNN) implemented in AIfES can be freely parameterized, trained, modified or reloaded at runtime.
In this version, it is optimized for the Arduino IDE and compatible to almost any Arduino board.
AIfES is developed in the C programming language and uses only standard libraries based on the GNU Compiler Collection (GCC).
AIfES thus runs on almost any hardware from 8-bit microcontrollers over Raspberry PI to smartphones or PCs.
Not only inference of FNN is possible, but also training directly in the device. Furthermore, compatibility to other AI software frameworks such as Keras or TensorFlow is also given.

AIfES was developed by the Fraunhofer Institute for Microelectronic Circuits and Systems IMS. Additional information can be found at <www.aifes.ai>

The Fraunhofer IMS with AIfES and Arduino intend to enter a partnership.

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
Q31:	32 Bit integer
Q7:		8 Bit integer
Quantization of an F32 FNN in Q31 and Q7 can be performed directly in AIfES.

Neural network types
AIfES supports in the current version:
*	Feedforward Neural Network (FNN) inference and training
*	Other network types are in progress (see roadmap)

The number of neurons and the number of different layers can be adapted individually. Inference and training can be performed on the controller.
**Please note, that we are currently working on the training functionality for q31 nets.**

**Inference layer**

| Layer      | f32     |
|------------|---------|
| \link ailayer_dense.h Dense \endlink | ailayer_dense_f32_default()<br>ailayer_dense_f32_cmsis() |
| \link ailayer_input.h Input \endlink | ailayer_input_f32_default() |
| \link ailayer_relu.h ReLU \endlink | ailayer_relu_f32_default() |
| \link ailayer_sigmoid.h Sigmoid \endlink | ailayer_sigmoid_f32_default() |
| \link ailayer_softmax.h Softmax \endlink | ailayer_softmax_f32_default() |
| \link ailayer_leaky_relu.h Leaky ReLU \endlink | ailayer_leaky_relu_f32_default() |
| \link ailayer_elu.h ELU \endlink | ailayer_elu_f32_default() |
| \link ailayer_tanh.h Tanh \endlink | ailayer_tanh_f32_default() |
| \link ailayer_softsign.h Softsign \endlink | ailayer_softsign_f32_default() |

**Training layer**

| Layer      | f32     |
|------------|---------|
| \link ailayer_dense.h Dense \endlink | ailayer_dense_f32_default()<br>ailayer_dense_f32_cmsis() |
| \link ailayer_input.h Input \endlink| ailayer_input_f32_default() |
| \link ailayer_relu.h ReLU \endlink | ailayer_relu_f32_default() |
| \link ailayer_sigmoid.h Sigmoid \endlink | ailayer_sigmoid_f32_default() |
| \link ailayer_softmax.h Softmax \endlink | ailayer_softmax_f32_default() |
| \link ailayer_leaky_relu.h Leaky ReLU \endlink | ailayer_leaky_relu_f32_default() |
| \link ailayer_elu.h ELU \endlink | ailayer_elu_f32_default() |
| \link ailayer_tanh.h Tanh \endlink | ailayer_tanh_f32_default() |
| \link ailayer_softsign.h Softsign \endlink | ailayer_softsign_f32_default() |

**Loss**

| Loss                     | f32     |
|--------------------------|---------|
| \link ailoss_mse.h Mean Squared Error (MSE) \endlink | ailoss_mse_f32_default() |
| \link ailoss_crossentropy.h Crossentropy \endlink | ailoss_crossentropy_f32_default() |

**Optimizer**

| Optimizer                         | f32     |
|-----------------------------------|---------|
| \link aiopti_sgd.h Stochastic Gradient Descent (SGD) \endlink | aiopti_sgd_f32_default() |
| \link aiopti_adam.h Adam \endlink | aiopti_adam_f32_default() |

## Installation
Download the AIfES repository as a ZIP archive and follow these instructions:
<https://www.arduino.cc/en/guide/libraries>

## Roadmap
The AIfES team at Fraunhofer IMS is constantly working on new features and network types. The next feature you can look forward to is:

### Convolutional Neural Networks (ConvNet) 
Soon, classical ConvNets can be executed and trained in AIfES.
