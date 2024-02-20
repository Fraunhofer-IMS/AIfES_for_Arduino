/**
 * \file basic/base/aimath/aimath_u8.h
 * \internal
 * \date 19.01.2021
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
 * \brief Definition of the U8 (aiu8) data-type
 *
 * The U8 (aiu8) data-type stores data as 8 bit unsigned integer values.
 * It does not require any additional parameter to define the values.
 * This data-type is used for example for classification labels.
 *
 * **Example: Create a U8 tensor**\n
 * The tensor
 * @f[
 * \left( \begin{array}{rrr} 0 & 1 & 2 \\ 3 & 4 & 5 \end{array}\right)
 * @f]
 * can be created with
 * In C:
 * \code{.c}
 * uint8_t example_data[] = {0, 1, 2,
 *                           3, 4, 5};
 * uint16_t example_shape[] = {2, 3};
 * aitensor_t example_tensor = {
 *     .dtype = aiu8,
 *     .dim = 2,
 *     .shape = example_shape,
 *     .data = example_data
 * };
 * \endcode
 * In C, C++ and on Arduino:
 * \code{.c}
 * uint8_t example_data[] = {0, 1, 2,
 *                           3, 4, 5};
 * uint16_t example_shape[] = {2, 3};
 * aitensor_t example_tensor = AITENSOR_2D_U8(example_shape, example_data);
 * \endcode
 *
 * **Example: Create a U8 scalar**\n
 * Either create it as a normal uint8_t value
 * \code{.c}
 * uint8_t scalar = 42;
 * \endcode
 * or with the alias
 * \code{.c}
 * aiscalar_u8_t scalar = 42;
 * \endcode
 *
 * **Example: Print a U8 tensor to the console**
 * \code{.c}
 * print_aitensor(&example_tensor);
 * \endcode
 *
 * **Example: Print a U8 scalar to the console**
 * \code{.c}
 * print_aiscalar(&scalar, aiu8);
 * \endcode
 */

#ifndef AIMATH_U8
#define AIMATH_U8

#include "core/aifes_math.h"
#include "basic/base/aimath/aimath_basic.h"

/** @brief Initialize a 2 dimensional U8 tensor
 *
 * @param shape A uint16_t array of length 2 for the shape
 * @param data  A uint8_t array for the tensor data
 */
#define AITENSOR_2D_U8(shape, data)    {aiu8, 2, shape, 0, data}

typedef uint8_t				aiscalar_u8_t;

/** @brief Printing a U8 tensor to console
 *
 * For users the function
 * \code{.c}
 * print_aitensor(&tensor);
 * \endcode
 * is prefered.
 *
 * @param *tensor	The tensor to print.
 */
void aimath_u8_print_aitensor(const aitensor_t *tensor);

/** @brief Printing a U8 scalar to console
 *
 * For users the function
 * \code{.c}
 * print_aiscalar(&scalar, aiu8);
 * \endcode
 * is prefered.
 *
 * @param *scalar	The scalar (type: uint8_t) to print.
 */
void aimath_u8_print_aiscalar(const void *scalar);

/** @brief The U8 data-type indicator
 *
 * Use this variable to configure some element with the \link aimath_u8.h U8 \endlink data-type,
 */
extern const aimath_dtype_t *aiu8;

#endif // AIMATH_U8

