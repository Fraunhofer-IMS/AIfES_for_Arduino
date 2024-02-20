/**
 * \file basic/avr_pgm/aimath/aimath_f32_avr_pgm.c
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

#include "basic/avr_pgm/aimath/aimath_f32_avr_pgm.h"
#include <avr/pgmspace.h>


AISTRING_STORAGE_WRAPPER(aistring_error_f32_linear_avr_pgm_1, "[aimath_f32_avr_pgm_linear] MatMul input shapes doesn't match.\n");
AISTRING_STORAGE_WRAPPER(aistring_error_f32_linear_avr_pgm_2, "[aimath_f32_avr_pgm_linear] MatMul output shape doesn't match.\n");

void aimath_f32_avr_pgm_linear(const aitensor_t *a, const aitensor_t *b, const aitensor_t *c, aitensor_t *result)
{
    uint16_t i, j, k;
    float sum;

    float *a_data = (float *) a->data;
    float *b_data = (float *) b->data;
    float *c_data = c != 0 ? (float *) c->data : 0;
    float *result_data = (float *) result->data;

    #ifdef AIDEBUG_SHAPE_CHECKS
        if(a->shape[1] != b->shape[0])
        {
            AILOG_E(aistring_error_f32_linear_avr_pgm_1);
            return;
        }
        if(a->shape[0] != result->shape[0] || b->shape[1] != result->shape[1])
        {
            AILOG_E(aistring_error_f32_linear_avr_pgm_2);
            return;
        }
    #endif

    for(i = 0; i < a->shape[0]; i++)
    {
        for(j = 0; j < b->shape[1]; j++)
        {
            sum = 0.0f;
            for(k = 0; k < a->shape[1]; k++)
            {
                sum += a_data[i*a->shape[1] + k] * pgm_read_float(&(b_data[k*b->shape[1] + j]));
            }
            if(c != 0){
                // Bias add
                sum += pgm_read_float(&c_data[j]);
            }
            result_data[i*b->shape[1] + j] = sum;
        }
    }
    return;
}

#endif // AIFES_WITH_AVR_PGM
#endif // __AVR__
