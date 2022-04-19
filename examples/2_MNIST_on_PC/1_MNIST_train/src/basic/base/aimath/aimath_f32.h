/**
 * \file basic/base/aimath/aimath_f32.h
 * \internal
 * \date 28.10.2020
 * \endinternal
 * \version 2.0alpha
 * \copyright  Copyright (C) 2020-2021  Fraunhofer Institute for Microelectronic Circuits and Systems.
    All rights reserved.

    AIfES is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * \brief 	Definition of the F32 (aif32) data-type
 *
 * The F32 (aif32) data-type stores data as 32 bit single precision floating point values.
 * It does not require any additional parameter to define the values.
 *
 * **Example: Create a F32 tensor**\n
 * The tensor
 * @f[
 * \left( \begin{array}{rrr} 0 & 1 & 2 \\ 3 & 4 & 5 \end{array}\right)
 * @f]
 * can be created with
 * \code{.c}
 * float example_data[] = {0.0f, 1.0f, 2.0f,
 *                         3.0f, 4.0f, 5.0f};
 * uint16_t example_shape[] = {2, 3};
 * aitensor_t example_tensor = {
 *     .dtype = aif32,
 *     .dim = 2,
 *     .shape = example_shape,
 *     .data = example_data
 * };
 * \endcode
 *
 * **Example: Create a F32 scalar**\n
 * Either create it as a normal float value
 * \code{.c}
 * float scalar = 42.0f;
 * \endcode
 * or with the alias
 * \code{.c}
 * aiscalar_f32_t scalar = 42.0f;
 * \endcode
 *
 * **Example: Print a F32 tensor to the console**
 * \code{.c}
 * print_aitensor(&example_tensor);
 * \endcode
 *
 * **Example: Print a F32 scalar to the console**
 * \code{.c}
 * print_aiscalar(&scalar, aif32);
 * \endcode
 */

#ifndef AIMATH_F32
#define AIMATH_F32

#include "core/aifes_math.h"
#include "basic/base/aimath/aimath_basic.h"

/** @brief Initialize a 2 dimensional F32 tensor
 *
 * @param shape A uint16_t array of length 2 for the shape
 * @param data  A float array for the tensor data
 */
#define AITENSOR_2D_F32(shape, data)    {aif32, 2, shape, 0, data}

/** @brief Scalar for F32 (aif32) data-type
 *
 * This is only an alias for a float value for consistency.\n
 * You can create a F32 scalar either as a normal float value
 * \code{.c}
 * float scalar = 42.0f;
 * \endcode
 * or with the alias
 * \code{.c}
 * aiscalar_f32_t scalar = 42.0f;
 * \endcode
 *
 * You can print the scalar to the console with
 * \code{.c}
 * print_aiscalar(&scalar, aif32);
 * \endcode
 * or by using
 * \code{.c}
 * aimath_f32_print_aiscalar(&scalar, printf);
 * \endcode
 */
typedef float				aiscalar_f32_t;

/** @brief Printing a F32 tensor to console
 *
 * For users the function
 * \code{.c}
 * print_aitensor(&tensor);
 * \endcode
 * is prefered.
 *
 * @param *tensor	The tensor to print.
 */
void aimath_f32_print_aitensor(const aitensor_t *tensor);

/** @brief Printing a F32 scalar to console
 *
 * For users the function
 * \code{.c}
 * print_aiscalar(&scalar, aif32);
 * \endcode
 * is prefered.
 *
 * @param *scalar	The scalar (type: float) to print.
 * @param *print	The print function to use
 */
void aimath_f32_print_aiscalar(const void *scalar, int (*print)(const char *format, ...));

/** @brief The F32 data-type indicator
 *
 * Use this variable to configure some element with the \link aimath_f32.h F32 \endlink data-type,
 */
extern const aimath_dtype_t *aif32;

#endif // AIMATH_F32
