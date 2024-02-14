/**
 * \file basic/avr_pgm/aimath/aimath_q7_avr_pgm.c
 * \internal
 * \date 16.11.2021
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
 */

#include "aifes_config.h"

#if __AVR__
#ifdef AIFES_WITH_AVR_PGM

#include "basic/avr_pgm/aimath/aimath_q7_avr_pgm.h"
#include <avr/pgmspace.h>

#include "basic/base/aimath/aimath_q31.h"


AISTRING_STORAGE_WRAPPER(aistring_error_q7_linear32_avr_pgm_1, "[aimath_q7_avr_pgm_linear32] MatMul input shapes doesn't match.\n");
AISTRING_STORAGE_WRAPPER(aistring_error_q7_linear32_avr_pgm_2, "[aimath_q7_avr_pgm_linear32] MatMul output shape doesn't match.\n");
AISTRING_STORAGE_WRAPPER(aistring_error_q7_linear32_avr_pgm_3, "[aimath_q7_avr_pgm_linear32] Third operand shift does not match.\n");

void aimath_q7_avr_pgm_linear32_1(const aitensor_t *a, const aitensor_t *b, const aitensor_t *c, aitensor_t *result)
    {
    uint16_t i, j, k;
    int32_t sum, acc; // 16-bit accumulator
    uint16_t a_shift = pgm_read_word(&(((aimath_q7_params_t *) a->tensor_params)->shift));
    uint16_t b_shift = pgm_read_word(&(((aimath_q7_params_t *) b->tensor_params)->shift));
    uint16_t c_shift = pgm_read_word(&(((aimath_q31_params_t *) c->tensor_params)->shift));
    uint16_t result_shift = pgm_read_word(&(((aimath_q7_params_t *) result->tensor_params)->shift));

    int8_t z_a = pgm_read_byte(&(((aimath_q7_params_t *) a->tensor_params)->zero_point));
    int8_t z_b = pgm_read_byte(&(((aimath_q7_params_t *) b->tensor_params)->zero_point));
    int8_t z_result = pgm_read_byte(&(((aimath_q7_params_t *) result->tensor_params)->zero_point));

    // Output scaling factor M = (S_1 * S_2) / S_3
    uint16_t output_shift = a_shift + b_shift - result_shift;

    int8_t *a_data = (int8_t *) a->data;
    int8_t *b_data = (int8_t *) b->data;
    int32_t *c_data;
    if(c != 0) c_data = (int32_t *) c->data;
    int8_t *result_data = (int8_t *) result->data;


    #ifdef AIDEBUG_SHAPE_CHECKS
        if(a->shape[1] != b->shape[0])
        {
            AILOG_E(aistring_error_q7_linear32_avr_pgm_1);
            return;
        }
        if(a->shape[0] != result->shape[0] || b->shape[1] != result->shape[1])
        {
            AILOG_E(aistring_error_q7_linear32_avr_pgm_2);
            return;
        }
    #endif
    #ifdef AIDEBUG_GENERAL_CHECKS
        if(c_shift != a_shift + b_shift)
        {
            AILOG_E(aistring_error_q7_linear32_avr_pgm_3);
            return;
        }
    #endif // AIDEBUG_GENERAL_CHECKS

    for(i = 0; i < a->shape[0]; i++)
    {
        for(j = 0; j < b->shape[1]; j++)
        {
            sum = 0;
            for(k = 0; k < a->shape[1]; k++)
            {
                // uint32 += uint8 * uint8
                sum += (int32_t) a_data[i*a->shape[1] + k] * (int32_t) (int8_t) pgm_read_byte(&(b_data[k*b->shape[1] + j]));
            }
            // Add the zero point correction terms if zero point != 0
            if(z_a != 0){
                // a_2 = sum(q_{2,jk}) from j=1 to N
                acc = 0;
                for(k = 0; k < a->shape[1]; k++){
                    acc += (int32_t) (int8_t) pgm_read_byte(&(b_data[k*b->shape[1] + j]));
                }
                sum -= z_a * acc;
            }
            if(z_b != 0){
                // a_1 = sum(q_{1,ij}) from j=1 to N
                acc = 0;
                for(k = 0; k < a->shape[1]; k++){
                    acc += (int32_t) a_data[i*a->shape[1] + k];
                }
                sum -= z_b * acc;
            }
            if(z_a != 0 && z_b != 0){
                // N * Z_1 * Z_2
                sum += a->shape[1] * z_a * z_b;
            }
            if(c != 0){
                // Bias add
                sum += (int32_t) pgm_read_dword(&c_data[j]);
            }

            result_data[i*b->shape[1] + j] = (int8_t)((sum >> output_shift) + (int16_t) z_result);
        }
    }
    return;
}

void aimath_q7_avr_pgm_linear32_2(const aitensor_t *a, const aitensor_t *b, const aitensor_t *c, aitensor_t *result)
{
    uint16_t i, j, k;
    int32_t sum, acc; // 16-bit accumulator
    uint16_t a_shift = ((aimath_q7_params_t *) a->tensor_params)->shift;
    uint16_t b_shift = pgm_read_word(&(((aimath_q7_params_t *) b->tensor_params)->shift));
    uint16_t c_shift = pgm_read_word(&(((aimath_q31_params_t *) c->tensor_params)->shift));
    uint16_t result_shift = pgm_read_word(&(((aimath_q7_params_t *) result->tensor_params)->shift));

    int8_t z_a = ((aimath_q7_params_t *) a->tensor_params)->zero_point;
    int8_t z_b = pgm_read_byte(&(((aimath_q7_params_t *) b->tensor_params)->zero_point));
    int8_t z_result = pgm_read_byte(&(((aimath_q7_params_t *) result->tensor_params)->zero_point));

    // Output scaling factor M = (S_1 * S_2) / S_3
    uint16_t output_shift = a_shift + b_shift - result_shift;

    int8_t *a_data = (int8_t *) a->data;
    int8_t *b_data = (int8_t *) b->data;
    int32_t *c_data;
    if(c != 0) c_data = (int32_t *) c->data;
    int8_t *result_data = (int8_t *) result->data;


    #ifdef AIDEBUG_SHAPE_CHECKS
        if(a->shape[1] != b->shape[0])
        {
            AILOG_E(aistring_error_q7_linear32_avr_pgm_1);
            return;
        }
        if(a->shape[0] != result->shape[0] || b->shape[1] != result->shape[1])
        {
            AILOG_E(aistring_error_q7_linear32_avr_pgm_2);
            return;
        }
    #endif
    #ifdef AIDEBUG_GENERAL_CHECKS
        if(c_shift != a_shift + b_shift)
        {
            AILOG_E(aistring_error_q7_linear32_avr_pgm_3);
            return;
        }
    #endif // AIDEBUG_GENERAL_CHECKS

    for(i = 0; i < a->shape[0]; i++) {
        for(j = 0; j < b->shape[1]; j++) {
            sum = 0;
            for(k = 0; k < a->shape[1]; k++) {
                // uint32 += uint8 * uint8
                sum += (int32_t) a_data[i*a->shape[1] + k] * (int32_t) (int8_t) pgm_read_byte(&(b_data[k*b->shape[1] + j]));
            }
            // Add the zero point correction terms if zero point != 0
            if(z_a != 0){
                // a_2 = sum(q_{2,jk}) from j=1 to N
                acc = 0;
                for(k = 0; k < a->shape[1]; k++){
                    acc += (int32_t) (int8_t) pgm_read_byte(&(b_data[k*b->shape[1] + j]));
                }
                sum -= z_a * acc;
            }
            if(z_b != 0){
                // a_1 = sum(q_{1,ij}) from j=1 to N
                acc = 0;
                for(k = 0; k < a->shape[1]; k++){
                    acc += (int32_t) a_data[i*a->shape[1] + k];
                }
                sum -= z_b * acc;
            }
            if(z_a != 0 && z_b != 0){
                // N * Z_1 * Z_2
                sum += a->shape[1] * z_a * z_b;
            }
            if(c != 0){
                // Bias add
                sum += (int32_t) pgm_read_dword(&c_data[j]);
            }

            result_data[i*b->shape[1] + j] = (int8_t)((sum >> output_shift) + (int16_t) z_result);
        }
    }
    return;
}


void aimath_q7_avr_pgm_linear32_bt_1(const aitensor_t *a, const aitensor_t *b, const aitensor_t *c, aitensor_t *result)
    {
    uint16_t i, j, k;
    int32_t sum, acc; // 16-bit accumulator
    uint16_t a_shift = pgm_read_word(&(((aimath_q7_params_t *) a->tensor_params)->shift));
    uint16_t b_shift = pgm_read_word(&(((aimath_q7_params_t *) b->tensor_params)->shift));
    uint16_t c_shift = pgm_read_word(&(((aimath_q31_params_t *) c->tensor_params)->shift));
    uint16_t result_shift = pgm_read_word(&(((aimath_q7_params_t *) result->tensor_params)->shift));

    int8_t z_a = pgm_read_byte(&(((aimath_q7_params_t *) a->tensor_params)->zero_point));
    int8_t z_b = pgm_read_byte(&(((aimath_q7_params_t *) b->tensor_params)->zero_point));
    int8_t z_result = pgm_read_byte(&(((aimath_q7_params_t *) result->tensor_params)->zero_point));

    // Output scaling factor M = (S_1 * S_2) / S_3
    uint16_t output_shift = a_shift + b_shift - result_shift;

    int8_t *a_data = (int8_t *) a->data;
    int8_t *b_data = (int8_t *) b->data;
    int32_t *c_data;
    if(c != 0) c_data = (int32_t *) c->data;
    int8_t *result_data = (int8_t *) result->data;


    #ifdef AIDEBUG_SHAPE_CHECKS
        if(a->shape[1] != b->shape[1])
        {
            AILOG_E(aistring_error_q7_linear32_avr_pgm_1);
            return;
        }
        if(a->shape[0] != result->shape[0] || b->shape[0] != result->shape[1])
        {
            AILOG_E(aistring_error_q7_linear32_avr_pgm_2);
            return;
        }
    #endif
    #ifdef AIDEBUG_GENERAL_CHECKS
        if(c_shift != a_shift + b_shift)
        {
            AILOG_E(aistring_error_q7_linear32_avr_pgm_3);
            return;
        }
    #endif // AIDEBUG_GENERAL_CHECKS

    for(i = 0; i < a->shape[0]; i++)
    {
        for(j = 0; j < b->shape[0]; j++)
        {
            sum = 0;
            for(k = 0; k < a->shape[1]; k++)
            {
                // uint32 += uint8 * uint8
                sum += (int32_t) a_data[i*a->shape[1] + k] * (int32_t) (int8_t) pgm_read_byte(&(b_data[j*b->shape[1] + k]));
            }
            // Add the zero point correction terms if zero point != 0
            if(z_a != 0){
                // a_2 = sum(q_{2,jk}) from j=1 to N
                acc = 0;
                for(k = 0; k < a->shape[1]; k++){
                    acc += (int32_t) (int8_t) pgm_read_byte(&(b_data[j*b->shape[1] + k]));
                }
                sum -= z_a * acc;
            }
            if(z_b != 0){
                // a_1 = sum(q_{1,ij}) from j=1 to N
                acc = 0;
                for(k = 0; k < a->shape[1]; k++){
                    acc += (int32_t) a_data[i*a->shape[1] + k];
                }
                sum -= z_b * acc;
            }
            if(z_a != 0 && z_b != 0){
                // N * Z_1 * Z_2
                sum += a->shape[1] * z_a * z_b;
            }
            if(c != 0){
                // Bias add
                sum += (int32_t) pgm_read_dword(&c_data[j]);
            }

            result_data[i*b->shape[0] + j] = (int8_t)((sum >> output_shift) + (int16_t) z_result);
        }
    }
    return;
}

void aimath_q7_avr_pgm_linear32_bt_2(const aitensor_t *a, const aitensor_t *b, const aitensor_t *c, aitensor_t *result)
{
    uint16_t i, j, k;
    int32_t sum, acc; // 16-bit accumulator
    uint16_t a_shift = ((aimath_q7_params_t *) a->tensor_params)->shift;
    uint16_t b_shift = pgm_read_word(&(((aimath_q7_params_t *) b->tensor_params)->shift));
    uint16_t c_shift = pgm_read_word(&(((aimath_q31_params_t *) c->tensor_params)->shift));
    uint16_t result_shift = pgm_read_word(&(((aimath_q7_params_t *) result->tensor_params)->shift));

    int8_t z_a = ((aimath_q7_params_t *) a->tensor_params)->zero_point;
    int8_t z_b = pgm_read_byte(&(((aimath_q7_params_t *) b->tensor_params)->zero_point));
    int8_t z_result = pgm_read_byte(&(((aimath_q7_params_t *) result->tensor_params)->zero_point));

    // Output scaling factor M = (S_1 * S_2) / S_3
    uint16_t output_shift = a_shift + b_shift - result_shift;

    int8_t *a_data = (int8_t *) a->data;
    int8_t *b_data = (int8_t *) b->data;
    int32_t *c_data;
    if(c != 0) c_data = (int32_t *) c->data;
    int8_t *result_data = (int8_t *) result->data;


    #ifdef AIDEBUG_SHAPE_CHECKS
        if(a->shape[1] != b->shape[1])
        {
            AILOG_E(aistring_error_q7_linear32_avr_pgm_1);
            return;
        }
        if(a->shape[0] != result->shape[0] || b->shape[0] != result->shape[1])
        {
            AILOG_E(aistring_error_q7_linear32_avr_pgm_2);
            return;
        }
    #endif
    #ifdef AIDEBUG_GENERAL_CHECKS
        if(c_shift != a_shift + b_shift)
        {
            AILOG_E(aistring_error_q7_linear32_avr_pgm_3);
            return;
        }
    #endif // AIDEBUG_GENERAL_CHECKS

    for(i = 0; i < a->shape[0]; i++) {
        for(j = 0; j < b->shape[0]; j++) {
            sum = 0;
            for(k = 0; k < a->shape[1]; k++) {
                // uint32 += uint8 * uint8
                sum += (int32_t) a_data[i*a->shape[1] + k] * (int32_t) (int8_t) pgm_read_byte(&(b_data[j*b->shape[1] + k]));
            }
            // Add the zero point correction terms if zero point != 0
            if(z_a != 0){
                // a_2 = sum(q_{2,jk}) from j=1 to N
                acc = 0;
                for(k = 0; k < a->shape[1]; k++){
                    acc += (int32_t) (int8_t) pgm_read_byte(&(b_data[j*b->shape[1] + k]));
                }
                sum -= z_a * acc;
            }
            if(z_b != 0){
                // a_1 = sum(q_{1,ij}) from j=1 to N
                acc = 0;
                for(k = 0; k < a->shape[1]; k++){
                    acc += (int32_t) a_data[i*a->shape[1] + k];
                }
                sum -= z_b * acc;
            }
            if(z_a != 0 && z_b != 0){
                // N * Z_1 * Z_2
                sum += a->shape[1] * z_a * z_b;
            }
            if(c != 0){
                // Bias add
                sum += (int32_t) pgm_read_dword(&c_data[j]);
            }

            result_data[i*b->shape[0] + j] = (int8_t)((sum >> output_shift) + (int16_t) z_result);
        }
    }
    return;
}


void aimath_q7_avr_pgm_relu(const aitensor_t *x, aitensor_t *result) {
    // Create new tensor with loaded quantization parameters from progmem
    aimath_q7_params_t x_q_params;
    x_q_params.shift = pgm_read_word(&(((aimath_q7_params_t *) x->tensor_params)->shift));
    x_q_params.zero_point = (int8_t) pgm_read_byte(&(((aimath_q7_params_t *) x->tensor_params)->zero_point));
    aitensor_t x_new;
    x_new.dtype = aiq7;
    x_new.dim = x->dim;
    x_new.shape = x->shape;
    x_new.tensor_params = &x_q_params;
    x_new.data = x->data;

    aimath_q7_default_relu(&x_new, result);
    return;
}


void aimath_q7_avr_pgm_leaky_relu(const aitensor_t *x, const void *alpha, aitensor_t *result) {
    // Create new tensor with loaded quantization parameters from progmem
    aimath_q7_params_t x_q_params;
    x_q_params.shift = pgm_read_word(&(((aimath_q7_params_t *) x->tensor_params)->shift));
    x_q_params.zero_point = (int8_t) pgm_read_byte(&(((aimath_q7_params_t *) x->tensor_params)->zero_point));
    aitensor_t x_new;
    x_new.dtype = aiq7;
    x_new.dim = x->dim;
    x_new.shape = x->shape;
    x_new.tensor_params = &x_q_params;
    x_new.data = x->data;
    aimath_q7_default_leaky_relu(&x_new, alpha, result);
    return;
}

void aimath_q7_avr_pgm_elu(const aitensor_t *x, const void *alpha, aitensor_t *result) {
    // Create new tensor with loaded quantization parameters from progmem
    aimath_q7_params_t x_q_params;
    x_q_params.shift = pgm_read_word(&(((aimath_q7_params_t *) x->tensor_params)->shift));
    x_q_params.zero_point = (int8_t) pgm_read_byte(&(((aimath_q7_params_t *) x->tensor_params)->zero_point));
    aitensor_t x_new;
    x_new.dtype = aiq7;
    x_new.dim = x->dim;
    x_new.shape = x->shape;
    x_new.tensor_params = &x_q_params;
    x_new.data = x->data;

    aimath_q7_default_elu(&x_new, alpha, result);
    return;
}

void aimath_q7_avr_pgm_sigmoid(const aitensor_t *x, aitensor_t *result) {
    // Create new tensor with loaded quantization parameters from progmem
    aimath_q7_params_t x_q_params;
    x_q_params.shift = pgm_read_word(&(((aimath_q7_params_t *) x->tensor_params)->shift));
    x_q_params.zero_point = (int8_t) pgm_read_byte(&(((aimath_q7_params_t *) x->tensor_params)->zero_point));
    aitensor_t x_new;
    x_new.dtype = aiq7;
    x_new.dim = x->dim;
    x_new.shape = x->shape;
    x_new.tensor_params = &x_q_params;
    x_new.data = x->data;

    aimath_q7_default_sigmoid(&x_new, result);
    return;
}


void aimath_q7_avr_pgm_tanh(const aitensor_t *x, aitensor_t *result) {
    // Create new tensor with loaded quantization parameters from progmem
    aimath_q7_params_t x_q_params;
    x_q_params.shift = pgm_read_word(&(((aimath_q7_params_t *) x->tensor_params)->shift));
    x_q_params.zero_point = (int8_t) pgm_read_byte(&(((aimath_q7_params_t *) x->tensor_params)->zero_point));
    aitensor_t x_new;
    x_new.dtype = aiq7;
    x_new.dim = x->dim;
    x_new.shape = x->shape;
    x_new.tensor_params = &x_q_params;
    x_new.data = x->data;

    aimath_q7_default_tanh(&x_new, result);
    return;
}

void aimath_q7_avr_pgm_softsign(const aitensor_t *x, aitensor_t *result) {
    // Create new tensor with loaded quantization parameters from progmem
    aimath_q7_params_t x_q_params;
    x_q_params.shift = pgm_read_word(&(((aimath_q7_params_t *) x->tensor_params)->shift));
    x_q_params.zero_point = (int8_t) pgm_read_byte(&(((aimath_q7_params_t *) x->tensor_params)->zero_point));
    aitensor_t x_new;
    x_new.dtype = aiq7;
    x_new.dim = x->dim;
    x_new.shape = x->shape;
    x_new.tensor_params = &x_q_params;
    x_new.data = x->data;

    aimath_q7_default_softsign(&x_new, result);
    return;
}

void aimath_q7_avr_pgm_softmax(const aitensor_t *x, aitensor_t *result) {
    // Create new tensor with loaded quantization parameters from progmem
    aimath_q7_params_t x_q_params;
    x_q_params.shift = pgm_read_word(&(((aimath_q7_params_t *) x->tensor_params)->shift));
    x_q_params.zero_point = (int8_t) pgm_read_byte(&(((aimath_q7_params_t *) x->tensor_params)->zero_point));
    aitensor_t x_new;
    x_new.dtype = aiq7;
    x_new.dim = x->dim;
    x_new.shape = x->shape;
    x_new.tensor_params = &x_q_params;
    x_new.data = x->data;

    aimath_q7_default_softmax(&x_new, result);
    return;
}

#endif // AIFES_WITH_AVR_PGM
#endif // __AVR__
