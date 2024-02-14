/**
 * \file cnn/default/aimath/aimath_cnn_f32_default.h
 * \internal
 * \date 11.06.2020
 * \endinternal
 * \version 2.2.0
 * \copyright  Copyright (C) 2020-2023  Fraunhofer Institute for Microelectronic Circuits and Systems.
    All rights reserved.<br><br>
    AIfES is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.<br><br>
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.<br><br>
    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * \brief Math functions for \link aimath_f32.h F32 \endlink data type, CNN-specific implementation
 *
 * These functions can be used when no hardware specific implementation is available.
 */

#ifndef AIMATH_CNN_F32_DEFAULT_H
#define AIMATH_CNN_F32_DEFAULT_H

#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "core/aifes_core.h"
#include "basic/base/aimath/aimath_f32.h"
#include "basic/default/aimath/aimath_f32_default.h"


#define AIFES_PADDING_SAME      0xFFFF  // Not fully implemented yet
#define AIFES_PADDING_VALID     0


/** @brief Performs 2D convolution on slices of 4D \link aimath_f32.h F32 \endlink tensors and adds an optional bias
  *
  * The function takes 2D slices out of 4D tensors and performs a 2D convolution. The result is then added to the output slice.
  * This technique allows the use of one function for all use-cases (different order like HWC or CHW; different cases in forward and backward pass).
  *
  * To configure the slices for the convolution, an index array has to be passed for input, kernel and output tensor.
  * The dimension of H is indicated with -1 and W with -2.\n
  * Example: [1, 5, -1, -2] -> Perform a 2D convolution on the last two dimensions. The other two dimensions have indices 1 and 5.
  *
  * The output dimensions of the height and width are given as:
  * @f[
  *  H_{out} = floor \left( \frac{H_{in} + P_{h,top} + P_{h,bottom} - D_h \times (H_{kernel} - 1) - 1}{S_h} \right) + 1
  * @f]
  * @f[
  *  W_{out} = floor \left( \frac{W_{in} + P_{w,left} + P_{w,right} - D_w \times (W_{kernel} - 1) - 1}{S_w} \right) + 1
  * @f]
  *
  * @param input            Input tensor (4D)
  * @param stride           Stride in the direction of height and width
  * @param dilation         Dilation of the kernel tensor in the direction of height and width
  * @param padding          Asymmetric zero padding in the direction of height and width \f$ [[P_{h,top},P_{h,bottom}],[P_{w,left},P_{w,right}]] \f$
  * @param kernel           Kernel / Weights tensor (4D)
  * @param bias             Optional bias tensor (1D; set to null if not in use)
  * @param rotated_kernel   Determines whether or not to rotate the kernel by 180°; default is TRUE (1)
  * @param input_use_dims   Indices for the 2D slice of the input tensor (-1 for the height dimension and -2 for the width dimension)
  * @param output_use_dims  Indices for the 2D slice of the output tensor (-1 for the height dimension and -2 for the width dimension)
  * @param kernel_use_dims  Indices for the 2D slice of the kernel tensor (-1 for the height dimension and -2 for the width dimension)
  * @param output           Output tensor to add the result to (4D)
  */
 void aimath_f32_default_conv2d_add(const aitensor_t *input,
                   const uint16_t stride[2],    // [s_h, s_w]
                   const uint16_t dilation[2],  // [d_h, d_w]
                   const int16_t padding[2][2],
                   const aitensor_t *kernel,
                   const void *bias,
                   const uint8_t rotated_kernel, // wether or not the kernel is rotated by 180°; default is TRUE (1)
                   const int16_t *input_use_dims, // Indices of dimensions for h and w
                   const int16_t *output_use_dims, // Indices of dimensions for h and w
                   const int16_t *kernel_use_dims, // Indices of dimensions for h and w
                   aitensor_t *output);

/** @brief Performs 2D transposed convolution (or deconvolution) on slices of 4D \link aimath_f32.h F32 \endlink tensors and adds an optional bias
  *
  * The function takes 2D slices out of 4D tensors and performs a 2D transposed convolution. The result is then added to the output slice.
  * This technique allows the use of one function for all use-cases (different order like HWC or CHW; different cases in forward and backward pass).
  *
  * To configure the slices for the transposed convolution, an index array has to be passed for input, kernel and output tensor.
  * The dimension of H is indicated with -1 and W with -2.\n
  * Example: [1, 5, -1, -2] -> Perform a 2D transposed convolution on the last two dimensions. The other two dimensions have indices 1 and 5.
  *
  * The output dimensions of the height and width are given as:
  * @f[
  *  H_{out} = S_h \times (H_{in}-1)+1 + P_{h,top} + P_{h,bottom} - D_h \times (H_{kernel} - 1)
  * @f]
  * @f[
  *  W_{out} = S_w \times (W_{in}-1)+1 + P_{w,left} + P_{w,right} - D_w \times (W_{kernel} - 1)
  * @f]
  *
  * @param input            Input tensor (4D)
  * @param stride           Dilation of the input tensor in the direction of height and width
  * @param dilation         Dilation of the kernel tensor in the direction of height and width
  * @param padding          Asymmetric zero padding in the direction of height and width \f$ [[P_{h,top},P_{h,bottom}],[P_{w,left},P_{w,right}]] \f$
  * @param kernel           Kernel / Weights tensor (4D)
  * @param bias             Optional bias tensor (1D; set to null if not in use)
  * @param rotated_kernel   Determines whether or not to rotate the kernel by 180°; default is TRUE (1)
  * @param input_use_dims   Indices for the 2D slice of the input tensor (-1 for the height dimension and -2 for the width dimension)
  * @param output_use_dims  Indices for the 2D slice of the output tensor (-1 for the height dimension and -2 for the width dimension)
  * @param kernel_use_dims  Indices for the 2D slice of the kernel tensor (-1 for the height dimension and -2 for the width dimension)
  * @param output           Output tensor to add the result to (4D)
  */
void aimath_f32_default_conv_transpose2d_add(const aitensor_t *input,
                   const uint16_t stride[2],    // [s_h, s_w]; dilation of input
                   const uint16_t dilation[2],  // [d_h, d_w]; dilation of kernel
                   const int16_t padding[2][2],
                   const aitensor_t *kernel,
                   const void *bias,
                   const uint8_t rotated_kernel, // wether or not the kernel is rotated by 180°; default is TRUE (1)
                   const int16_t *input_use_dims, // Indices of dimensions for h and w
                   const int16_t *output_use_dims, // Indices of dimensions for h and w
                   const int16_t *kernel_use_dims, // Indices of dimensions for h and w
                   aitensor_t *output);



/** @brief Performs 2D convolutions with the given 4D \link aimath_f32.h F32 \endlink tensors and adds a bias (forward pass of the Conv2D layer)
 *
 * @f[
 *  x_{out} = x_{in} \ast w + b
 * @f]
 *
 * This function wraps the aimath_f32_default_conv2d_add() function to perform the forward pass of a Conv2D layer.
 *
 * @param input             Input (\f$ x_{in} \f$) data with dimension \f$ [N,C_{in},H_{in},W_{in}] \f$ (channels first) or \f$ [N,H_{in},W_{in},C_{in}] \f$ (channels last)
 * @param stride            The stride in the direction of height and width
 * @param dilation          The dilation in the direction of height and width
 * @param padding           The (symmetric) zero padding in the direction of height and width
 * @param weights           Convolution kernels with dimension \f$ [C_{out},C_{in},H_{kernel},W_{kernel}] \f$ (channels first) or \f$ [C_{out},H_{kernel},W_{kernel},C_{in}] \f$ (channels last)
 * @param bias              Bias with dimension \f$ C_{out} \f$
 * @param channel_axis      Index of the channel axis (1 for channels first and -1 or 3 for channels last).
 * @param work_space        Pointer to a work space buffer for intermediate results (Not in use)
 * @param output            Output (\f$ x_{out} \f$) after convolution with dimension \f$ [N,C_{out},H_{out},W_{out}] \f$ (channels first) or \f$ [N,H_{out},W_{out},C_{out}] \f$ (channels last)
 */
void aimath_f32_default_conv2d_fwd(
                    const aitensor_t *input,
                    const uint16_t stride[2],    // [s_h, s_w]
                    const uint16_t dilation[2],  // [d_h, d_w]
                    const uint16_t padding[2],
                    const aitensor_t *weights,
                    const aitensor_t *bias,
                    int8_t channel_axis,
                    void *work_space,
                    aitensor_t *output
                    );

/** @brief Calculates the gradients of the Conv2D layer with respect to the weights in \link aimath_f32.h F32 \endlink data type
 *
 * Calculates the gradients with respect to the weights \f$ \partial w = \mathrm{d} L / \mathrm{d} w \f$.
 *
 * @f[
 *  \partial w = x_{in} \ast delta_{out}
 * @f]
 *
 * This function wraps the aimath_f32_default_conv2d_add() function to perform the backward pass of a Conv2D layer.
 *
 * @param x_in              Input data with dimension \f$ [N,C_{in},H_{in},W_{in}] \f$ (channels first) or \f$ [N,H_{in},W_{in},C_{in}] \f$ (channels last)
 * @param stride            The stride in the direction of height and width
 * @param dilation          The dilation in the direction of height and width
 * @param padding           The (symmetric) zero padding in the direction of height and width
 * @param delta_out         Gradients backpropagated from the following layer with dimension \f$ [N,C_{out},H_{out},W_{out}] \f$ (channels first) or \f$ [N,H_{out},W_{out},C_{out}] \f$ (channels last)
 * @param channel_axis      Index of the channel axis (1 for channels first and -1 or 3 for channels last).
 * @param work_space        Pointer to a work space buffer for intermediate results (Not in use)
 * @param d_weights         Output gradients of the weights with dimension \f$ [C_{out},C_{in},H_{kernel},W_{kernel}] \f$ (channels first) or \f$ [C_{out},H_{kernel},W_{kernel},C_{in}] \f$ (channels last)
 */
void aimath_f32_default_conv2d_bwd(
                    const aitensor_t *x_in,
                    const uint16_t stride[2],    // [s_h, s_w]
                    const uint16_t dilation[2],  // [d_h, d_w]
                    const uint16_t padding[2],
                    const aitensor_t *delta_out,
                    int8_t channel_axis,
                    void *work_space,
                    aitensor_t *d_weights
);

/** @brief Calculates the gradients of the Conv2D layer with respect to the weights in \link aimath_f32.h F32 \endlink data type
 *
 * Calculates the gradients with respect to the input \f$ delta_{in} = \mathrm{d} L / \mathrm{d} x_{in} \f$.
 *
 * @f[
 *  delta_{in} = delta_{out} \ast' w
 * @f]
 * \f$ \cdot \ast' \cdot \f$ is a transposed convolution.
 *
 * This function wraps the aimath_f32_default_conv_transpose2d_add() function to perform the backward pass of a Conv2D layer.
 *
 * @param delta_out         Gradients backpropagated from the following layer with dimension \f$ [N,C_{in},H_{in},W_{in}] \f$ (channels first) or \f$ [N,H_{in},W_{in},C_{in}] \f$ (channels last)
 * @param stride            The stride in the direction of height and width
 * @param dilation          The dilation in the direction of height and width
 * @param padding           The (symmetric) zero padding in the direction of height and width
 * @param weights           Convolution kernels with dimension \f$ [C_{out},C_{in},H_{kernel},W_{kernel}] \f$ (channels first) or \f$ [C_{out},H_{kernel},W_{kernel},C_{in}] \f$ (channels last)
 * @param channel_axis      Index of the channel axis (1 for channels first and -1 or 3 for channels last).
 * @param work_space        Pointer to a work space buffer for intermediate results.
 * @param delta_in          Resulting input gradients for backpropagation to the previous layer with dimension \f$ [N,C_{in},H_{in},W_{in}] \f$ (channels first) or \f$ [N,H_{in},W_{in},C_{in}] \f$ (channels last)
 */
void aimath_f32_default_conv2d_bwd_full(
                    const aitensor_t *delta_out,
                    const uint16_t stride[2],    // [s_h, s_w]
                    const uint16_t dilation[2],  // [d_h, d_w]
                    const uint16_t padding[2],
                    const aitensor_t *weights,
                    int8_t channel_axis,
                    void *work_space,
                    aitensor_t *delta_in
);

/** @brief Performs 2D transposed convolutions with the given 4D \link aimath_f32.h F32 \endlink tensors and adds a bias (forward pass of the ConvTranspose2D layer)
 *
 * @f[
 *  x_{out} = x_{in} \ast' w + b
 * @f]
 * \f$ \cdot \ast' \cdot \f$ is a transposed convolution.
 *
 * This function wraps the aimath_f32_default_conv_transpose2d_add() function to perform the forward pass of a ConvTranspose2D layer.
 *
 * @param input             Input (\f$ x_{in} \f$) data with dimension \f$ [N,C_{in},H_{in},W_{in}] \f$ (channels first) or \f$ [N,H_{in},W_{in},C_{in}] \f$ (channels last)
 * @param stride            Dilation of the input in the direction of height and width
 * @param dilation          Dilation of the kernel in the direction of height and width
 * @param padding           The (symmetric) zero padding in the direction of height and width
 * @param output_padding    Additional asymmetric zero padding on one side in the direction of height and width
 * @param weights           Convolution kernels with dimension \f$ [C_{out},C_{in},H_{kernel},W_{kernel}] \f$ (channels first) or \f$ [C_{out},H_{kernel},W_{kernel},C_{in}] \f$ (channels last)
 * @param bias              Bias with dimension \f$ C_{out} \f$
 * @param channel_axis      Index of the channel axis (1 for channels first and -1 or 3 for channels last).
 * @param work_space        Pointer to a work space buffer for intermediate results (Not in use)
 * @param output            Output (\f$ x_{out} \f$) after convolution with dimension \f$ [N,C_{out},H_{out},W_{out}] \f$ (channels first) or \f$ [N,H_{out},W_{out},C_{out}] \f$ (channels last)
 */
void aimath_f32_default_conv_transpose2d_fwd(
                    const aitensor_t *input,
                    const uint16_t stride[2],    // input dilation
                    const uint16_t dilation[2],  // kernel dilation
                    const uint16_t padding[2],
                    const uint16_t output_padding[2],
                    const aitensor_t *weights,
                    const aitensor_t *bias,
                    int8_t channel_axis,
                    void *work_space,
                    aitensor_t *output
                    );

/** @brief 2D max-pooling on 4D \link aimath_f32.h F32 \endlink tensors
 *
 * Performs a 2D max-pooling operation on 2D slices of a 4D input tensor. This function is used as the forward pass of the
 * MaxPool2D layer.
 *
 * For training (max_locations != 0), the index of the max-value in the kernel window is be stored in max_locations.\n
 * An element of max_locations simply consist of the concatenated 16-bit indices for height and width in the pooling window.
 *
 * The output dimensions of the height and width are given as:
 * @f[
 *  H_{out} = floor \left( \frac{H_{in} + 2 * P_h - H_{pool}}{S_h} \right) + 1
 * @f]
 * @f[
 *  W_{out} = floor \left( \frac{W_{in} + 2 * P_w - W_{pool}}{S_w} \right) + 1
 * @f]
 *
 * @param input             Input data with dimension \f$ [N,C,H_{in},W_{in}] \f$ (channels first) or \f$ [N,H_{in},W_{in},C] \f$ (channels last)
 * @param pool_size         The size of the pooling window (height and width)
 * @param stride            The stride in the direction of height and width.
 * @param padding           The (symmetric) minus infinity padding in the direction of height and width
 * @param channel_axis      Index of the channel axis (1 for channels first and -1 or 3 for channels last).
 * @param work_space        Pointer to a work space buffer for intermediate results.
 * @param max_locations     Pointer to memory section where the indices of the maximum values per pooling window are stored.
 * @param output            Output after max-pooling with dimension \f$ [N,C,H_{out},W_{out}] \f$ (channels first) or \f$ [N,H_{out},W_{out},C] \f$ (channels last)
 */
void aimath_f32_default_maxpool2d_fwd(
                                      const aitensor_t *input,
                                      const uint16_t pool_size[2],
                                      const uint16_t stride[2],
                                      const uint16_t padding[2],
                                      int8_t channel_axis,
                                      void *work_space,
                                      uint32_t *max_locations,
                                      aitensor_t *output
                                      );

/** @brief Calculates the gradients of the MaxPool2D layer with respect to the input in \link aimath_f32.h F32 \endlink data type
 *
 * Calculates the gradients with respect to the input \f$ delta_{in} = \mathrm{d} L / \mathrm{d} x_{in} \f$.
 *
 * This is done by simply copying the output gradient to the position in the input gradients depicted by max_locations.\n
 * An element of max_locations consist of the concatenated 16-bit indices for height and width in the pooling window.
 *
 * @param delta_out         Gradients backpropagated from the following layer with dimension \f$ [N,C,H_{out},W_{out}] \f$ (channels first) or \f$ [N,H_{out},W_{out},C] \f$ (channels last)
 * @param pool_size         The size of the pooling window (height and width)
 * @param stride            The stride in the direction of height and width.
 * @param padding           The (symmetric) minus infinity padding in the direction of height and width
 * @param channel_axis      Index of the channel axis (1 for channels first and -1 or 3 for channels last).
 * @param work_space        Pointer to a work space buffer for intermediate results.
 * @param max_locations     Pointer to memory section where the indices of the maximum values per pooling window are stored.
 * @param delta_in          Resulting input gradients for backpropagation to the previous layer \f$ [N,C,H_{in},W_{in}] \f$ (channels first) or \f$ [N,H_{in},W_{in},C] \f$ (channels last)
 */
void aimath_f32_default_maxpool2d_bwd(
                                      const aitensor_t *delta_out,
                                      const uint16_t pool_size[2],
                                      const uint16_t stride[2],
                                      const uint16_t padding[2],
                                      int8_t channel_axis,
                                      void *work_space,
                                      const uint32_t *max_locations,
                                      aitensor_t *delta_in
                                      );

/** @brief Batch Normalization on \link aimath_f32.h F32 \endlink tensors
 *
 * Performs the Batch Normalization operation (proposed by Ioffe and Szegedy, https://arxiv.org/abs/1502.03167):\n
 * @f[
 *  y_{i,j} = \mathit{BN}(x_{i,j}) = \gamma_i \cdot \frac{x_{i,j} - \mu_{i}}{\sqrt{\sigma_{i}^2+\epsilon}} + \beta_i
 * @f]
 *
 * @param x             Input tensor (N-D)
 * @param axis          Axis of the input tensor that stores the channel dimension.
 * @param means         1D vector with the means (\f$ \mu_i \f$) of every channel.
 * @param variances     1D vector with the variances (\f$ \sigma^2_i \f$) of every channel.
 * @param offsets       1D vector with the offset parameters (\f$ \beta_i \f$) of every channel.
 * @param scales        1D vector with the scaling parameters (\f$ \gamma_i \f$) of every channel.
 * @param eps           Small constant for numerical stability.
 * @param result        The resulting normalized tensor (N-D)
 */
void aimath_f32_default_batch_norm(const aitensor_t *x,
                                            int8_t axis,
                                            const aitensor_t *means,
                                            const aitensor_t *variances,
                                            const aitensor_t *offsets,
                                            const aitensor_t *scales,
                                            const void *eps,
                                            aitensor_t *result);

/** @brief Calculates the gradients of Batch Normalization with respect to betas, gammas and the input in \link aimath_f32.h F32 \endlink data type
 *
 * Calculates the derivative of the Batch Normalization with respect to the input
 * and the trainable parameters (\f$ \beta \f$ and \f$ \gamma \f$).\n
 *
 * Please refer to the paper by Ioffe and Szegedy (https://arxiv.org/abs/1502.03167) for the equations of the gradients.
 *
 * @param x_in          Input tensor (N-D)
 * @param axis          Axis of the input tensor that stores the channel dimension.
 * @param means         1D vector with the means (\f$ \mu_i \f$) of every channel.
 * @param vars          1D vector with the variances (\f$ \sigma^2_i \f$) of every channel.
 * @param betas         1D vector with the offset parameters (\f$ \beta_i \f$) of every channel.
 * @param gammas        1D vector with the scaling parameters (\f$ \gamma_i \f$) of every channel.
 * @param delta_out     Gradient calculated by the output layer for gradient backpropagation (N-D)
 * @param eps           Small constant for numerical stability.
 * @param delta_in      The resulting gradients of the input (\f$ \mathrm{d}\mathcal{L} / \mathrm{d}x \f$).
 * @param d_betas       The resulting gradients of the \f$ \beta \f$ parameter (\f$ \mathrm{d}\mathcal{L} / \mathrm{d}\beta \f$).
 * @param d_gammas      The resulting gradients of the \f$ \gamma \f$ parameter (\f$ \mathrm{d}\mathcal{L}  / \mathrm{d}\gamma \f$).
 */
void aimath_f32_default_d_batch_norm(const aitensor_t *x_in,
                                              int8_t axis,
                                              const aitensor_t *means,
                                              const aitensor_t *vars,
                                              const aitensor_t *betas,
                                              const aitensor_t *gammas,
                                              const aitensor_t *delta_out,
                                              const void *eps,
                                              aitensor_t *delta_in,
                                              aitensor_t *d_betas,
                                              aitensor_t *d_gammas);

/** @brief Pads a \link aimath_f32.h F32 \endlink tensor with zeros
 *
 * @param x             Input F32 tensor (N-D)
 * @param padding       Array of the asymmetric zero paddings for each dimension of the input tensor
 * @param result        Resulting padded F32 tensor (N-D)
 */
void aimath_f32_default_pad_zeros(const aitensor_t *x, const uint16_t (*padding)[2], aitensor_t *result);

 #endif // AIMATH_CNN_F32_DEFAULT_H
