/**
 * \file cnn/default/aimath/aimath_cnn_f32_default.c
 * \version 2.2.0
 * \date 11.06.2020
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
 * \brief
 * \details
 */

#include "cnn/default/aimath/aimath_cnn_f32_default.h"
#include <float.h>

AISTRING_STORAGE_WRAPPER(aistring_error_f32_conv2d_add_default_1, "[aimath_f32_default_conv2d_add] Conv2d output shape doesn't match.\n");

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
                   aitensor_t *output)
{
  // Input format: NCHW
  // Weights format: FCHW

  uint16_t n_h; // Input height
  uint16_t n_w; // Input width
  uint16_t o_h; // Output height
  uint16_t o_w; // Output width
  uint16_t k_h;
  uint16_t k_w;

  uint32_t acc;
  int8_t i;

  // ------------- Calculate the index multiplier for the given dimensions -----------------
  uint32_t x_1;
  uint32_t x_2;
  uint32_t x_3 = 0;
  acc = 1;
  for(i = input->dim - 1; i >= 0; i--)
  {
    if(input_use_dims[i] == -1)
    {
      x_1 = acc;
      n_h = input->shape[i];
    }
    else if(input_use_dims[i] == -2)
    {
      x_2 = acc;
      n_w = input->shape[i];
    }
    else x_3 += input_use_dims[i] * acc;
    acc *= input->shape[i];
  }

  uint32_t k_1;
  uint32_t k_2;
  uint32_t k_3 = 0;
  acc = 1;
  for(i = kernel->dim - 1; i >= 0; i--)
  {
    if(kernel_use_dims[i] == -1)
    {
      k_1 = acc;
      k_h = kernel->shape[i];
    }
    else if(kernel_use_dims[i] == -2)
    {
      k_2 = acc;
      k_w = kernel->shape[i];
    }
    else k_3 += kernel_use_dims[i] * acc;
    acc *= kernel->shape[i];
  }

  uint32_t y_1;
  uint32_t y_2;
  uint32_t y_3 = 0;
  acc = 1;
  for(i = output->dim - 1; i >= 0; i--)
  {
    if(output_use_dims[i] == -1){
        y_1 = acc;
        o_h = output->shape[i];
    } else if(output_use_dims[i] == -2){
        y_2 = acc;
        o_w = output->shape[i];
    } else {
        y_3 += output_use_dims[i] * acc;
    }
    acc *= output->shape[i];
  }
  // -------------------------------------------------------------

  // Stride
  uint16_t s_h = stride[0];
  uint16_t s_w = stride[1];
  // Dilation
  uint16_t d_h = dilation[0];
  uint16_t d_w = dilation[1];

  int16_t p_h0 = padding[0][0];
  int16_t p_h1 = padding[0][1];
  int16_t p_w0 = padding[1][0];
  int16_t p_w1 = padding[1][1];


  int32_t y_h_idx, y_w_idx, k_h_idx, k_w_idx;
  // Output dimensions: floor((N + P0 + P1 - (D * (K - 1) + 1)) / S + 1)
  // N: Input shape
  // P0 and P1: Left and right input padding
  // D: Kernel dilation
  // K: Kernel size
  // S: Stride
  uint32_t out_h = (uint32_t)((n_h + p_h0 + p_h1 - (d_h * (k_h - 1) + 1)) / s_h) + 1;
  uint32_t out_w = (uint32_t)((n_w + p_w0 + p_w1 - (d_w * (k_w - 1) + 1)) / s_w) + 1;

#ifdef AIDEBUG_SHAPE_CHECKS
  if(out_h != o_h || out_w != o_w)
  {
    AILOG_E(aistring_error_f32_conv2d_add_default_1);
    return;
  }
#endif

  float *kernel_data = kernel->data;
  float *y_data = output->data;
  float *x_data = input->data;
  float sum;
  uint32_t idx_h, idx_w; // Auxilary index variables

  for(y_h_idx = 0; y_h_idx < out_h; y_h_idx++)
  {
    for(y_w_idx = 0; y_w_idx < out_w; y_w_idx++)
    {
      sum = 0.0f;
      if(rotated_kernel == FALSE)
      {
        for(k_h_idx = 0; k_h_idx < k_h; k_h_idx++)
        {
          for(k_w_idx = 0; k_w_idx < k_w; k_w_idx++)
          {
            idx_h = s_h * y_h_idx + d_h * k_h_idx;
            idx_w = s_w * y_w_idx + d_w * k_w_idx;
            // Check for padding constraints
            // (Crop some values at zero to make comparing signed and unsigned integers possible)
            if(idx_h >= (p_h0 < 0 ? 0 : p_h0) && idx_w >= (p_w0 < 0 ? 0 : p_w0)
               && idx_h < ((n_h + p_h0) < 0 ? 0 : (n_h + p_h0)) && idx_w < ((n_w + p_w0) < 0 ? 0 : (n_w + p_w0))){
                sum += x_data[x_1 * (idx_h - p_h0) + x_2 * (idx_w - p_w0) + x_3]
                        * kernel_data[k_1 * k_h_idx + k_2 * k_w_idx + k_3];
            }
          }
        }
      }
      else
      {
        // 180° Rotated kernel
        for(k_h_idx = 0; k_h_idx < k_h; k_h_idx++)
        {
            for(k_w_idx = 0; k_w_idx < k_w; k_w_idx++)
            {
                idx_h = s_h * y_h_idx + d_h * k_h_idx;
                idx_w = s_w * y_w_idx + d_w * k_w_idx;
                // Check for padding constraints
                // (Crop some values at zero to make comparing signed and unsigned integers possible)
                if(idx_h >= (p_h0 < 0 ? 0 : p_h0) && idx_w >= (p_w0 < 0 ? 0 : p_w0)
                   && idx_h < ((n_h + p_h0) < 0 ? 0 : (n_h + p_h0)) && idx_w < ((n_w + p_w0) < 0 ? 0 : (n_w + p_w0))){
                    sum += x_data[x_1 * (idx_h - p_h0) + x_2 * (idx_w - p_w0) + x_3]
                            * kernel_data[k_1 * (k_h - k_h_idx - 1) + k_2 * (k_w - k_w_idx - 1) + k_3];
                }
            }
        }
      }
      if(bias != 0){
        sum += *((float*) bias);
      }
      y_data[y_1 * y_h_idx + y_2 * y_w_idx + y_3] += sum;
    }
  }
  return;
}

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
                   aitensor_t *output)
{
  // Input format: NCHW
  // Weights format: FCHW

  uint16_t n_h; // Input height
  uint16_t n_w; // Input width
  uint16_t o_h; // Output height
  uint16_t o_w; // Output width
  uint16_t k_h;
  uint16_t k_w;

  uint32_t acc;
  int8_t i;

  // ------------- Calculate the index multiplier for the given dimensions -----------------
  uint32_t x_1;
  uint32_t x_2;
  uint32_t x_3 = 0;
  acc = 1;
  for(i = input->dim - 1; i >= 0; i--)
  {
    if(input_use_dims[i] == -1)
    {
      x_1 = acc;
      n_h = input->shape[i];
    }
    else if(input_use_dims[i] == -2)
    {
      x_2 = acc;
      n_w = input->shape[i];
    }
    else x_3 += input_use_dims[i] * acc;
    acc *= input->shape[i];
  }

  uint32_t k_1;
  uint32_t k_2;
  uint32_t k_3 = 0;
  acc = 1;
  for(i = kernel->dim - 1; i >= 0; i--)
  {
    if(kernel_use_dims[i] == -1)
    {
      k_1 = acc;
      k_h = kernel->shape[i];
    }
    else if(kernel_use_dims[i] == -2)
    {
      k_2 = acc;
      k_w = kernel->shape[i];
    }
    else k_3 += kernel_use_dims[i] * acc;
    acc *= kernel->shape[i];
  }

  uint32_t y_1;
  uint32_t y_2;
  uint32_t y_3 = 0;
  acc = 1;
  for(i = output->dim - 1; i >= 0; i--)
  {
    if(output_use_dims[i] == -1){
        y_1 = acc;
        o_h = output->shape[i];
    } else if(output_use_dims[i] == -2){
        y_2 = acc;
        o_w = output->shape[i];
    } else {
        y_3 += output_use_dims[i] * acc;
    }
    acc *= output->shape[i];
  }
  // -------------------------------------------------------------

  // Stride (input dilation)
  uint16_t s_h = stride[0];
  uint16_t s_w = stride[1];
  // Dilation (kernel dilation)
  uint16_t d_h = dilation[0];
  uint16_t d_w = dilation[1];

  int16_t p_h0 = padding[0][0];
  int16_t p_h1 = padding[0][1];
  int16_t p_w0 = padding[1][0];
  int16_t p_w1 = padding[1][1];

  int32_t y_h_idx, y_w_idx, k_h_idx, k_w_idx;
  // Output dimensions: S * (N - 1) + 1 + P0 + P1 - (D * (K - 1) + 1) + 1
  // N: Input shape
  // P0 and P1: Left and right input padding
  // D: Kernel dilation
  // K: Kernel size
  // S: Stride / Input dilation
  uint32_t out_h = ((s_h * (n_h - 1) + 1) + p_h0 + p_h1 - (d_h * (k_h - 1) + 1)) + 1;
  uint32_t out_w = ((s_w * (n_w - 1) + 1) + p_w0 + p_w1 - (d_w * (k_w - 1) + 1)) + 1;

#ifdef AIDEBUG_SHAPE_CHECKS
  if(out_h != o_h || out_w != o_w)
  {
    AILOG_E(aistring_error_f32_conv2d_add_default_1);
    return;
  }
#endif

  float *kernel_data = kernel->data;
  float *y_data = output->data;
  float *x_data = input->data;
  float sum;
  uint32_t idx_h, idx_w; // Auxilary index variables

  for(y_h_idx = 0; y_h_idx < out_h; y_h_idx++)
  {
    for(y_w_idx = 0; y_w_idx < out_w; y_w_idx++)
    {
      sum = 0.0f;
      if(rotated_kernel == FALSE)
      {
        for(k_h_idx = 0; k_h_idx < k_h; k_h_idx++)
        {
          for(k_w_idx = 0; k_w_idx < k_w; k_w_idx++)
          {
              idx_h = y_h_idx + d_h * k_h_idx;
              idx_w = y_w_idx + d_w * k_w_idx;
              // Check for dilation and padding constraints
              // (Crop some values at zero to make comparing signed and unsigned integers possible)
              if((idx_h - p_h0) % s_h == 0 && (idx_w - p_w0) % s_w == 0
                 && idx_h / s_h < ((n_h + p_h0 / s_h) < 0 ? 0 : (n_h + p_h0 / s_h))
                 && idx_w / s_w < ((n_w + p_w0 / s_w) < 0 ? 0 : (n_w + p_w0 / s_w))
                 && idx_h >= (p_h0 < 0 ? 0 : p_h0) && idx_w >= (p_w0 < 0 ? 0 : p_w0)){
                    sum += x_data[x_1 * (idx_h - p_h0) / s_h + x_2 * (idx_w - p_w0) / s_w + x_3]
                            * kernel_data[k_1 * k_h_idx + k_2 * k_w_idx + k_3];
              }

          }
        }
      }
      else
      {
        // 180° Rotated kernel
        for(k_h_idx = 0; k_h_idx < k_h; k_h_idx++)
        {
            for(k_w_idx = 0; k_w_idx < k_w; k_w_idx++)
            {
              idx_h = y_h_idx + d_h * k_h_idx;
              idx_w = y_w_idx + d_w * k_w_idx;
              // Check for dilation and padding constraints
              // (Crop some values at zero to make comparing signed and unsigned integers possible)
              if((idx_h - p_h0) % s_h == 0 && (idx_w - p_w0) % s_w == 0
                 && idx_h / s_h < ((n_h + p_h0 / s_h) < 0 ? 0 : (n_h + p_h0 / s_h))
                 && idx_w / s_w < ((n_w + p_w0 / s_w) < 0 ? 0 : (n_w + p_w0 / s_w))
                 && idx_h >= (p_h0 < 0 ? 0 : p_h0) && idx_w >= (p_w0 < 0 ? 0 : p_w0)){
                    sum += x_data[x_1 * (idx_h - p_h0) / s_h + x_2 * (idx_w - p_w0) / s_w + x_3]
                            * kernel_data[k_1 * (k_h - k_h_idx - 1) + k_2 * (k_w - k_w_idx - 1) + k_3];
              }
            }
        }
      }
      if(bias != 0){
        sum += *((float*) bias);
      }
      y_data[y_1 * y_h_idx + y_2 * y_w_idx + y_3] += sum;
    }
  }
  return;
}

void aimath_f32_default_conv2d_fwd(
                    const aitensor_t *input,
                    const uint16_t stride[2],    // [s_h, s_w]
                    const uint16_t dilation[2],  // [d_h, d_w]
                    const uint16_t padding[2],
                    const aitensor_t *weights,
                    const aitensor_t *bias,
                    int8_t channel_axis,
                    void *work_space,
                    aitensor_t *output)
{
    // Dimensions for h and w are fixed for input, kernel and result
    // The dynamic parts are the channel and filter numbers
    int16_t input_dims[4];
    int16_t weights_dims[4];
    int16_t output_dims[4];

    uint8_t channel_uaxis = channel_axis < 0 ? input->dim + channel_axis : channel_axis; // Negative axis = indexing from the end
    uint8_t h_ax, w_ax;
    uint16_t n_idx, f_idx, c_idx;
    uint16_t N = input->shape[0], F = weights->shape[0], C = weights->shape[channel_uaxis];
    int16_t fwd_padding[2][2];

    float *bias_ptr;

    if(channel_uaxis == 1){ // Channels first
        h_ax = 2;
        w_ax = 3;
    } else if(channel_uaxis == 3){ // Channels last
        h_ax = 1;
        w_ax = 2;
    } else {
        // ERROR
        return;
    }
    input_dims[h_ax] = -1;
    input_dims[w_ax] = -2;
    output_dims[h_ax] = -1;
    output_dims[w_ax] = -2;
    weights_dims[h_ax] = -1;
    weights_dims[w_ax] = -2;

    if(padding[0] == AIFES_PADDING_SAME){
        // Output shape should equal input shape
        // P = ceil(0.5 * ((N-1) * S - N + D * (K-1) + 1))
        fwd_padding[0][0] = (uint16_t)((((uint32_t) 1 << 15) * (uint32_t)((input->shape[h_ax] - 1) * stride[0]
                              - input->shape[h_ax] + dilation[0] * (weights->shape[h_ax] - 1) + 1)
                              + ((uint32_t) 1 << 16) - 1) >> 16);
        fwd_padding[0][1] = fwd_padding[0][0];
    } else {
        fwd_padding[0][0] = padding[0];
        fwd_padding[0][1] = padding[0];
    }
    if(padding[1] == AIFES_PADDING_SAME){
        // Output shape should equal input shape
        // P = ceil(0.5 * ((N-1) * S - N + D * (K-1) + 1))
        fwd_padding[1][0] = (uint16_t)((((uint32_t) 1 << 15) * (uint32_t)((input->shape[w_ax] - 1) * stride[1]
                              - input->shape[w_ax] + dilation[1] * (weights->shape[w_ax] - 1) + 1)
                              + ((uint32_t) 1 << 16) - 1) >> 16);
        fwd_padding[1][1] = fwd_padding[1][0];
    } else {
        fwd_padding[1][0] = padding[1];
        fwd_padding[1][1] = padding[1];
    }

    // Init result with zeros
    aimath_f32_default_init_zeros(output);

    // Iterate over all samples
    for (n_idx = 0; n_idx < N; n_idx++) {
        input_dims[0] = n_idx;
        output_dims[0] = n_idx;

        // for all f: y_f = sum_c{x_c * k_fc}
        for(f_idx = 0; f_idx < F; f_idx++)
        {
            output_dims[channel_uaxis] = f_idx;
            weights_dims[0] = f_idx;
            for(c_idx = 0; c_idx < C; c_idx++)
            {
                input_dims[channel_uaxis] = c_idx;
                weights_dims[channel_uaxis] = c_idx;
                if (c_idx + 1 == C) {
                    // Add the bias only when the last channel is reached
                    bias_ptr = (float *) (bias->data) + f_idx;
                } else {
                    bias_ptr = 0;
                }

                aimath_f32_default_conv2d_add(input,
                                              stride,
                                              dilation,
                                              fwd_padding,
                                              weights,
                                              bias_ptr,
                                              FALSE,
                                              input_dims,
                                              output_dims,
                                              weights_dims,
                                              output);
            }
        }
    }
}

// For d_weights calculation
void aimath_f32_default_conv2d_bwd(
                    const aitensor_t *x_in,
                    const uint16_t stride[2],    // [s_h, s_w]
                    const uint16_t dilation[2],  // [d_h, d_w]
                    const uint16_t padding[2],
                    const aitensor_t *delta_out,
                    int8_t channel_axis,
                    void *work_space,
                    aitensor_t *d_weights)
{
    // Dimensions for h and w are fixed for input, kernel and result
    // The dynamic parts are the channel and filter numbers
    int16_t input_dims[4];
    int16_t weights_dims[4];
    int16_t output_dims[4];

    uint8_t channel_uaxis = channel_axis < 0 ? x_in->dim + channel_axis : channel_axis; // Negative axis = indexing from the end
    uint8_t h_ax, w_ax;
    uint16_t n_idx, f_idx, c_idx;
    uint16_t N = x_in->shape[0], F = d_weights->shape[0], C = d_weights->shape[channel_uaxis];
    int16_t bwd_padding[2][2];

    if(channel_uaxis == 1){ // Channels first
        h_ax = 2;
        w_ax = 3;
    } else if(channel_uaxis == 3){ // Channels last
        h_ax = 1;
        w_ax = 2;
    } else {
        // ERROR
        return;
    }
    input_dims[h_ax] = -1;
    input_dims[w_ax] = -2;
    output_dims[h_ax] = -1;
    output_dims[w_ax] = -2;
    weights_dims[h_ax] = -1;
    weights_dims[w_ax] = -2;
    bwd_padding[0][0] = padding[0];
    bwd_padding[0][1] = (int16_t) dilation[0] * (d_weights->shape[h_ax] - 1) - x_in->shape[h_ax] - padding[0] + stride[0] * (delta_out->shape[h_ax] - 1) + 1;
    bwd_padding[1][0] = padding[1];
    bwd_padding[1][1] = (int16_t) dilation[1] * (d_weights->shape[w_ax] - 1) - x_in->shape[w_ax] - padding[1] + stride[1] * (delta_out->shape[w_ax] - 1) + 1;

    aimath_f32_default_init_zeros(d_weights);

    // Iterate over all samples
    for (n_idx = 0; n_idx < N; n_idx++) {
        input_dims[0] = n_idx;
        output_dims[0] = n_idx;

        // for all f, c: w_fc = x_c * dy_f
        for(f_idx = 0; f_idx < F; f_idx++)
        {
            output_dims[channel_uaxis] = f_idx;
            weights_dims[0] = f_idx;
            for(c_idx = 0; c_idx < C; c_idx++)
            {
                input_dims[channel_uaxis] = c_idx;
                weights_dims[channel_uaxis] = c_idx;

                aimath_f32_default_conv2d_add(x_in,
                                              dilation,
                                              stride,
                                              bwd_padding,
                                              delta_out,
                                              0,
                                              FALSE,
                                              input_dims,
                                              weights_dims,
                                              output_dims,
                                              d_weights);
            }
        }
    }
}

void aimath_f32_default_conv2d_bwd_full(
                    const aitensor_t *delta_out,
                    const uint16_t stride[2],    // [s_h, s_w]
                    const uint16_t dilation[2],  // [d_h, d_w]
                    const uint16_t padding[2],
                    const aitensor_t *weights,
                    int8_t channel_axis,
                    void *work_space,
                    aitensor_t *delta_in)
{
    // Dimensions for h and w are fixed for input, kernel and result
    // The dynamic parts are the channel and filter numbers
    int16_t input_dims[4];
    int16_t weights_dims[4];
    int16_t output_dims[4];

    uint8_t channel_uaxis = channel_axis < 0 ? 4 + channel_axis : channel_axis; // Negative axis = indexing from the end
    uint8_t h_ax, w_ax;
    uint16_t n_idx, f_idx, c_idx;
    uint16_t N = delta_out->shape[0], F = weights->shape[0], C = weights->shape[channel_uaxis];
    int16_t full_padding[2][2]; // Also support negative padding / cropping

    if(channel_uaxis == 1){ // Channels first
        h_ax = 2;
        w_ax = 3;
    } else if(channel_uaxis == 3){ // Channels last
        h_ax = 1;
        w_ax = 2;
    } else {
        // ERROR
        return;
    }
    input_dims[h_ax] = -1;
    input_dims[w_ax] = -2;
    output_dims[h_ax] = -1;
    output_dims[w_ax] = -2;
    weights_dims[h_ax] = -1;
    weights_dims[w_ax] = -2;

    full_padding[0][0] = (int32_t) dilation[0] * (weights->shape[h_ax] - 1) - padding[0];
    full_padding[0][1] = (int32_t) dilation[0] * (weights->shape[h_ax] - 1) - padding[0]
                            + (delta_in->shape[h_ax] + 2*padding[0] - (dilation[0] * (weights->shape[h_ax] - 1) + 1)) % stride[0];
    full_padding[1][0] = (int32_t) dilation[1] * (weights->shape[w_ax] - 1) - padding[1];
    full_padding[1][1] = (int32_t) dilation[1] * (weights->shape[w_ax] - 1) - padding[1]
                            + (delta_in->shape[w_ax] + 2*padding[1] - (dilation[1] * (weights->shape[w_ax] - 1) + 1)) % stride[1];

    aimath_f32_default_init_zeros(delta_in);

    // Iterate over all samples
    for (n_idx = 0; n_idx < N; n_idx++) {
        input_dims[0] = n_idx;
        output_dims[0] = n_idx;

        // for all c: dx_c = sum_f{dy0_f * k_fc'} ; k_fc' is the 180° rotated kernel k_fc
        for(c_idx = 0; c_idx < C; c_idx++)
        {
            input_dims[channel_uaxis] = c_idx;
            weights_dims[channel_uaxis] = c_idx;

            for(f_idx = 0; f_idx < F; f_idx++)
            {
                output_dims[channel_uaxis] = f_idx;
                weights_dims[0] = f_idx;

                aimath_f32_default_conv_transpose2d_add(delta_out,
                                              stride,
                                              dilation,
                                              full_padding,
                                              weights,
                                              0,
                                              TRUE,
                                              output_dims,
                                              input_dims,
                                              weights_dims,
                                              delta_in);
            }
        }
    }
}


void aimath_f32_default_conv_transpose2d_fwd(
                    const aitensor_t *input,
                    const uint16_t stride[2],    // [s_h, s_w]
                    const uint16_t dilation[2],  // [d_h, d_w]
                    const uint16_t padding[2],
                    const uint16_t output_padding[2],
                    const aitensor_t *weights,
                    const aitensor_t *bias,
                    int8_t channel_axis,
                    void *work_space,
                    aitensor_t *output)
{
    // Dimensions for h and w are fixed for input, kernel and result
    // The dynamic parts are the channel and filter numbers
    int16_t input_dims[4];
    int16_t weights_dims[4];
    int16_t output_dims[4];

    uint8_t channel_uaxis = channel_axis < 0 ? input->dim + channel_axis : channel_axis; // Negative axis = indexing from the end
    uint8_t h_ax, w_ax;
    uint16_t n_idx, f_idx, c_idx;
    uint16_t N = input->shape[0], F = weights->shape[0], C = weights->shape[channel_uaxis];
    int16_t combined_padding[2][2] = {{padding[0], padding[0] + output_padding[0]},
                                      {padding[1], padding[1] + output_padding[1]}};

    float *bias_ptr;

    if(channel_uaxis == 1){ // Channels first
        h_ax = 2;
        w_ax = 3;
    } else if(channel_uaxis == 3){ // Channels last
        h_ax = 1;
        w_ax = 2;
    } else {
        // ERROR
        return;
    }
    input_dims[h_ax] = -1;
    input_dims[w_ax] = -2;
    output_dims[h_ax] = -1;
    output_dims[w_ax] = -2;
    weights_dims[h_ax] = -1;
    weights_dims[w_ax] = -2;

    // Init result with zeros
    aimath_f32_default_init_zeros(output);

    // Iterate over all samples
    for (n_idx = 0; n_idx < N; n_idx++) {
        input_dims[0] = n_idx;
        output_dims[0] = n_idx;

        // for all f: y_f = sum_c{x_c * k_fc}
        for(f_idx = 0; f_idx < F; f_idx++)
        {
            output_dims[channel_uaxis] = f_idx;
            weights_dims[0] = f_idx;
            for(c_idx = 0; c_idx < C; c_idx++)
            {
                input_dims[channel_uaxis] = c_idx;
                weights_dims[channel_uaxis] = c_idx;
                if (c_idx + 1 == C) {
                    // Add the bias only when the last channel is reached
                    bias_ptr = (float *) (bias->data) + f_idx;
                } else {
                    bias_ptr = 0;
                }

                aimath_f32_default_conv_transpose2d_add(input,
                                              stride,
                                              dilation,
                                              combined_padding,
                                              weights,
                                              bias_ptr,
                                              FALSE,
                                              input_dims,
                                              output_dims,
                                              weights_dims,
                                              output);
            }
        }
    }
}

AISTRING_STORAGE_WRAPPER(aistring_error_f32_maxpool2d_default_1, "[aimath_f32_default_maxpool2d] MaxPool2D output shape doesn't match.\n");

void aimath_f32_default_maxpool2d_fwd(
                                      const aitensor_t *input,
                                      const uint16_t pool_size[2],
                                      const uint16_t stride[2],
                                      const uint16_t padding[2],
                                      int8_t channel_axis,
                                      void *work_space,
                                      uint32_t *max_locations,
                                      aitensor_t *output
                                      )
{
    uint8_t channel_uaxis = channel_axis < 0 ? 4 + channel_axis : channel_axis; // Negative axis = indexing from the end

    // Pooling parameters
    uint16_t pool_h_size = pool_size[0];
    uint16_t pool_w_size = pool_size[1];    // Stride
    uint16_t stride_h = stride[0];
    uint16_t stride_w = stride[1];
    uint16_t padding_h = padding[0];
    uint16_t padding_w = padding[1];
    // Declare iterator variables
    uint16_t out_h_idx, out_w_idx, pool_h_idx, pool_w_idx;
    // Declare variable required for maximum calculation
    float max;
    uint32_t max_idx;
    // Declare intermediate index variables
    uint32_t in_channel_start_idx, out_channel_start_idx, idx;
    uint16_t idx_h, idx_w;
    uint32_t x_n, x_c, x_h, x_w, y_n, y_c, y_h, y_w;
    uint8_t h_ax, w_ax;

    if(channel_uaxis == 1){ // Channels first
        h_ax = 2;
        w_ax = 3;
    } else if(channel_uaxis == 3){ // Channels last
        h_ax = 1;
        w_ax = 2;
    } else {
        // ERROR
        return;
    }

    uint16_t channel_size   = input->shape[channel_uaxis];
    uint16_t in_h_size      = input->shape[h_ax];
    uint16_t in_w_size      = input->shape[w_ax];
    uint16_t out_h_size     = output->shape[h_ax];
    uint16_t out_w_size     = output->shape[w_ax];

    // Calculate the multipliers for indexing
    if(channel_uaxis == 1){
        // NCHW

        x_n = (uint32_t) (channel_size * in_h_size * in_w_size);
        x_c = (uint32_t) (in_h_size * in_w_size);
        x_h = (uint32_t) in_w_size;
        x_w = (uint32_t) 1;

        y_n = (uint32_t) (channel_size * out_h_size * out_w_size);
        y_c = (uint32_t) (out_h_size * out_w_size);
        y_h = (uint32_t) out_w_size;
        y_w = (uint32_t) 1;
    } else{
        // NHWC

        x_n = (uint32_t) (in_h_size * in_w_size * channel_size);
        x_h = (uint32_t) (in_w_size * channel_size);
        x_w = (uint32_t) channel_size;
        x_c = (uint32_t) 1;

        y_n = (uint32_t) (out_h_size * out_w_size * channel_size);
        y_h = (uint32_t) (out_w_size * channel_size);
        y_w = (uint32_t) channel_size;
        y_c = (uint32_t) 1;
    }


    // Output dimensions
    uint32_t out_h = (uint32_t)((in_h_size + 2 * padding_h - pool_h_size) / stride_h) + 1;
    uint32_t out_w = (uint32_t)((in_w_size + 2 * padding_w - pool_w_size) / stride_w) + 1;

    #ifdef AIDEBUG_SHAPE_CHECKS
    if(out_h != out_h_size || out_w != out_w_size)
    {
        AILOG_E(aistring_error_f32_maxpool2d_default_1);
        return;
    }
    #endif


	uint16_t N = input->shape[0]; // Number of images in batch
	uint16_t C = input->shape[channel_uaxis]; // (In- and Out-) Channels

	uint16_t n_idx, c_idx;

	// interate through all images in batch
	for(n_idx = 0; n_idx < N; n_idx++)
	{
		for(c_idx = 0; c_idx < C; c_idx++)
		{
            in_channel_start_idx = n_idx * x_n + c_idx * x_c;
            out_channel_start_idx = n_idx * y_n + c_idx * y_c;

            // Perform the pooling
            for (out_h_idx = 0; out_h_idx < out_h_size; out_h_idx++) { // Slide pooling window over the input horizontally
                for (out_w_idx = 0; out_w_idx < out_w_size; out_w_idx++) { // Slide pooling window over the input vertically
                    // Perform the max pooling
                    max = -FLT_MAX;
                    max_idx = 0;
                    for (pool_h_idx = 0; pool_h_idx < pool_h_size; pool_h_idx++) {
                        for (pool_w_idx = 0; pool_w_idx < pool_w_size; pool_w_idx++) {
                            idx_h = (stride_h * out_h_idx + pool_h_idx);
                            idx_w = (stride_w * out_w_idx + pool_w_idx);

                            // Padding checks
                            if(idx_h >= padding_h && idx_w >= padding_w
                               && idx_h < padding_h + in_h_size && idx_w < padding_w + in_w_size){
                                idx = x_h * (idx_h - padding_h)
                                    + x_w * (idx_w - padding_w)
                                    + in_channel_start_idx;
                                if (((float *) input->data)[idx] > max) {
                                    max = ((float *) input->data)[idx];
                                    max_idx =  (pool_h_idx << 16) | pool_w_idx;
                                }
                            }
                        }
                    }
                    ((float *) output->data)[y_h * out_h_idx + y_w * out_w_idx + out_channel_start_idx] = max;
                    // save position of maximum, used in backpropagation
                    // but check first if memory is allocated for max_position
                    // is not needed if run for inference only)
                    if( max_locations != NULL ){
                        max_locations[y_h * out_h_idx + y_w * out_w_idx + out_channel_start_idx] = max_idx;
                    }
                }
            }
		}
	}
}

// ToDo
void aimath_f32_default_maxpool2d_bwd(
                                      const aitensor_t *input,
                                      const uint16_t pool_size[2],
                                      const uint16_t stride[2],
                                      const uint16_t padding[2],
                                      int8_t channel_axis,
                                      void *work_space,
                                      const uint32_t *max_locations,
                                      aitensor_t *output
                                      )
{
    uint8_t channel_uaxis = channel_axis < 0 ? 4 + channel_axis : channel_axis; // Negative axis = indexing from the end

    // Pooling parameters
    uint16_t stride_h = stride[0];
    uint16_t stride_w = stride[1];
    uint16_t padding_h = padding[0];
    uint16_t padding_w = padding[1];

    // Declare iterator variables
    uint16_t in_h_idx, in_w_idx;
    // Declare aux variables
    uint32_t max_idx;
    uint16_t max_idx_h, max_idx_w;
    // Declare intermediate index variables
    uint32_t in_channel_start_idx, out_channel_start_idx;
    uint16_t idx_h, idx_w;
    uint32_t x_n, x_c, x_h, x_w, y_n, y_c, y_h, y_w;
    uint8_t h_ax, w_ax;

    if(channel_uaxis == 1){ // Channels first
        h_ax = 2;
        w_ax = 3;
    } else if(channel_uaxis == 3){ // Channels last
        h_ax = 1;
        w_ax = 2;
    } else {
        // ERROR
        return;
    }

    uint16_t channel_size   = input->shape[channel_uaxis];
    uint16_t in_h_size      = input->shape[h_ax];
    uint16_t in_w_size      = input->shape[w_ax];
    uint16_t out_h_size     = output->shape[h_ax];
    uint16_t out_w_size     = output->shape[w_ax];

    // Calculate the multipliers for indexing
    if(channel_uaxis == 1){
        // NCHW

        x_n = (uint32_t) (channel_size * in_h_size * in_w_size);
        x_c = (uint32_t) (in_h_size * in_w_size);
        x_h = (uint32_t) in_w_size;
        x_w = (uint32_t) 1;

        y_n = (uint32_t) (channel_size * out_h_size * out_w_size);
        y_c = (uint32_t) (out_h_size * out_w_size);
        y_h = (uint32_t) out_w_size;
        y_w = (uint32_t) 1;
    } else{
        // NHWC

        x_n = (uint32_t) (in_h_size * in_w_size * channel_size);
        x_h = (uint32_t) (in_w_size * channel_size);
        x_w = (uint32_t) channel_size;
        x_c = (uint32_t) 1;

        y_n = (uint32_t) (out_h_size * out_w_size * channel_size);
        y_h = (uint32_t) (out_w_size * channel_size);
        y_w = (uint32_t) channel_size;
        y_c = (uint32_t) 1;
    }

    uint16_t N = input->shape[0]; // Number of images in batch
    uint16_t C = input->shape[channel_uaxis]; // (In- and Out-) Channels

    uint16_t n_idx, c_idx;

    aimath_f32_default_init_zeros(output);

    // interate through all images in batch
    for(n_idx = 0; n_idx < N; n_idx++)
    {
        for(c_idx = 0; c_idx < C; c_idx++)
        {
            in_channel_start_idx = n_idx * x_n + c_idx * x_c;
            out_channel_start_idx = n_idx * y_n + c_idx * y_c;

            // Perform the pooling
            for (in_h_idx = 0; in_h_idx < in_h_size; in_h_idx++) { // Slide pooling window over the input horizontally
                for (in_w_idx = 0; in_w_idx < in_w_size; in_w_idx++) { // Slide pooling window over the input vertically
                    max_idx = max_locations[x_h * in_h_idx + x_w * in_w_idx + in_channel_start_idx];
                    max_idx_h = max_idx >> 16;
                    max_idx_w = max_idx & 0x0000FFFF;
                    idx_h = (in_h_idx * stride_h + max_idx_h);
                    idx_w = (in_w_idx * stride_w + max_idx_w);
                    ((float *) output->data)[y_h * (idx_h - padding_h) + y_w * (idx_w - padding_w) + out_channel_start_idx]
                        += ((float *) input->data)[x_h * in_h_idx + x_w * in_w_idx + in_channel_start_idx];
                }
            }
        }
    }
    return;
}

void aimath_f32_default_batch_norm(const aitensor_t *x,
                                            int8_t axis,
                                            const aitensor_t *means,
                                            const aitensor_t *variances,
                                            const aitensor_t *offsets,
                                            const aitensor_t *scales,
                                            const void *eps,
                                            aitensor_t *result)
{
    uint32_t i, j, k;
    uint32_t idx_multiplier1 = 1, idx_multiplier2 = 1;
    float scale, offset;
    uint8_t uaxis = axis < 0 ? x->dim + axis : axis; // Negative axis = indexing from the end

    for(i = 0; i < uaxis; i++){
        idx_multiplier1 *= x->shape[i];
    }
    for(i = uaxis+1; i < x->dim; i++){
        idx_multiplier2 *= x->shape[i];
    }

    for(i = 0; i < x->shape[uaxis]; i++){
        scale = ((float *) scales->data)[i] / (sqrt(((float *) variances->data)[i] + *((float *) eps)));
        offset = ((float *) offsets->data)[i] - ((float *) means->data)[i] * scale;

        for(j = 0; j < idx_multiplier1; j++){
            for(k = 0; k < idx_multiplier2; k++){
                ((float *) result->data)[i*idx_multiplier2 + j*idx_multiplier2*x->shape[uaxis] + k] =
                    ((float *) x->data)[i*idx_multiplier2 + j*idx_multiplier2*x->shape[uaxis] + k] * scale + offset;
            }
        }
    }
    return;
}

void aimath_f32_default_d_batch_norm(const aitensor_t *x_in,
                                              int8_t axis,
                                              const aitensor_t *means,
                                              const aitensor_t *vars,
                                              const aitensor_t *offsets,
                                              const aitensor_t *scales,
                                              const aitensor_t *delta_out,
                                              const void *eps,
                                              aitensor_t *delta_in,
                                              aitensor_t *d_betas,
                                              aitensor_t *d_gammas)
{
    uint32_t i, j, k, index;
    uint8_t uaxis = axis < 0 ? x_in->dim + axis : axis;
    uint32_t idx_multiplier1 = 1, idx_multiplier2 = 1;
    // Auxillary variables
    float sqrt_var, sqrt_var_inv, d_var, d_mean, xhat, d_xhat, shifted_x;

    // Multipliers for index calculations
    for(i = 0; i < uaxis; i++){
        idx_multiplier1 *= x_in->shape[i];
    }
    for(i = uaxis+1; i < x_in->dim; i++){
        idx_multiplier2 *= x_in->shape[i];
    }

    // Gradients calculation per channel:
    // d_xhat_i = d_y_i * gamma
    // d_var = sum_i=1:m(d_xhat_i * (x_i - mean) * -0.5 * 1/(sqrt(var + eps)^3))
    // d_mean = sum_i=1:m(d_xhat_i * -sqrt(var + eps))
    // d_x_i = d_xhat_i * 1 / sqrt(var + eps) + d_var * 2 * (x_i - mean) / m + d_mean / m
    // d_gamma = sum_i=1:m(d_y_i * x_hat_i)
    // d_beta = sum(d_y_i)

    // Iterate through all channels
    for(i = 0; i < x_in->shape[uaxis]; i++){
        sqrt_var = sqrtf(((float *) vars->data)[i] + *((float *) eps));
        sqrt_var_inv = 1.0f / sqrt_var;


        // d_var = sum_i=1:m(d_xhat_i * (x_i - mean) * -0.5 * 1/(sqrt(var + eps)^3))
        // d_mean = sum_i=1:m(d_xhat_i * -sqrt(var + eps))
        d_var = 0.0f;
        d_mean = 0.0f;
        for(j = 0; j < idx_multiplier1; j++){
            for(k = 0; k < idx_multiplier2; k++){
                index = i*idx_multiplier2 + j*idx_multiplier2*x_in->shape[uaxis] + k;

                // d_xhat_j = d_y_j * gamma
                d_xhat = ((float *) delta_out->data)[index] * ((float *) scales->data)[i];

                shifted_x = ((float *) x_in->data)[index] - ((float *) means->data)[i];

                d_var += d_xhat * shifted_x;
                d_mean += d_xhat;
            }
        }
        d_var *= -0.5f * (sqrt_var_inv * sqrt_var_inv * sqrt_var_inv);
        d_mean *= -sqrt_var_inv;

        for(j = 0; j < idx_multiplier1; j++){
            for(k = 0; k < idx_multiplier2; k++){
                index = i*idx_multiplier2 + j*idx_multiplier2*x_in->shape[uaxis] + k;

                shifted_x = ((float *) x_in->data)[index] - ((float *) means->data)[i];

                // d_xhat_j = d_y_j * gamma
                d_xhat = ((float *) delta_out->data)[index] * ((float *) scales->data)[i];

                // xhat = (x - mean) / sqrt(var + eps)
                xhat = shifted_x * sqrt_var_inv;

                if(delta_in != 0){
                    // d_x_j = d_xhat_j * 1 / sqrt(var + eps) [+ d_var * 2 * (x_j - mean) / m + d_mean / m]
                    ((float *) delta_in->data)[index] =
                        d_xhat * sqrt_var_inv + (d_var * 2 * shifted_x + d_mean) / (idx_multiplier1 * idx_multiplier2);
                }

                if(d_gammas != 0){
                    // d_gamma = sum_j=1:m(d_y_j * x_hat_j)
                    ((float *) d_gammas->data)[i] += ((float *) delta_out->data)[index] * xhat;
                }
                if(d_betas != 0){
                    // d_beta = sum(d_y_j)
                    ((float *) d_betas->data)[i] += ((float *) delta_out->data)[index];
                }
            }
        }
    }

    return;
}

AISTRING_STORAGE_WRAPPER(aistring_error_f32_padding_1, "[aimath_f32_default_pad] Output shape doesn't match.\n");

void aimath_f32_default_pad_zeros(const aitensor_t *x, const uint16_t (*padding)[2], aitensor_t *result)
{
    uint32_t i;
    uint32_t x_idx_multiplier = 1, result_idx_multiplier = 1;
    aitensor_t x_i, result_i;

    #ifdef AIDEBUG_SHAPE_CHECKS
        if(result->shape[0] != x->shape[0] + padding[0][0] + padding[0][1])
        {
            AILOG_E(aistring_error_f32_padding_1);
            return;
        }
    #endif

    if(x->dim == 1){
        // Pad before
        for(i = 0; i < padding[0][0]; i++){
            ((float *) result->data)[i] = 0.0f;
        }
        // Copy values of x
        for(i = 0; i < x->shape[0]; i++){
            ((float *) result->data)[i + padding[0][0]] = ((float *) x->data)[i];
        }
        // Pad afterwards
        for(i = 0; i < padding[0][1]; i++){
            ((float *) result->data)[i + x->shape[0] + padding[0][0]] = 0.0f;
        }
    } else {
        for(i = 1; i < x->dim; i++){
            x_idx_multiplier *= x->shape[i];
        }
        for(i = 1; i < result->dim; i++){
            result_idx_multiplier *= result->shape[i];
        }

        // Pad before
        for(i = 0; i < padding[0][0] * result_idx_multiplier; i++){
            ((float *) result->data)[i] = 0.0f;
        }

        // Copy values of x
        x_i.dtype = aif32;
        x_i.dim = x->dim - 1;
        x_i.shape = x->shape + 1;
        result_i.dtype = aif32;
        result_i.dim = result->dim - 1;
        result_i.shape = result->shape + 1;
        for(i = 0; i < x->shape[0]; i++){
            x_i.data = (float *) x->data + i*x_idx_multiplier;
            result_i.data = (float *) result->data + (i+padding[0][0])*result_idx_multiplier;
            aimath_f32_default_pad_zeros(&x_i,
                                           padding + 1,
                                           &result_i);
        }

        // Pad afterwards
        for(i = 0; i < padding[0][1] * result_idx_multiplier; i++){
            ((float *) result->data)[i + (x->shape[0]+padding[0][0])*result_idx_multiplier] = 0.0f;
        }
    }
    return;
}
