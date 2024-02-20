/**
 * \file core/aifes_math.h
 * \internal
 * \date 20.10.2020
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
 * \brief 	AIfES 2 math interface
 *
 * This interface describes the basic structures to perform data type independent math in AIfES.
*/

#ifndef AIFES_MATH
#define AIFES_MATH

#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "aifes_config.h"


typedef struct aimath_dtype aimath_dtype_t;

typedef struct aitensor 	aitensor_t;

/** @brief Indicator for the used datatype
 *
 * This structure is used to indicate the type of variables like aitensors or aiscalars.
 */
struct aimath_dtype {
    const char *name; /**< Name of the data type (for example "F32") */
	uint32_t size; /**< The size of one data value of this type in bytes. */
	uint32_t tensor_params_size; /**< Size of the tensor params (for example for quantization parameters) of this type in bytes. */

    /** @brief Set a function to print the given tensor for debugging purposes
     *
     * This function should only be set in the debug mode when prints are required (to save memory otherwise)
     *
	 * @param tensor           The tensor to print
	 */
	void (*print_aitensor)(const aitensor_t *tensor);

    /** @brief Set a function to print the given scalar for debugging purposes
     *
     * This function should only be set in the debug mode when prints are required (to save memory otherwise)
     *
	 * @param tensor    The scalar to print
	 * @param *print    The print function to use (for example printf)
	 */
	void (*print_aiscalar)(const void *scalar);
};

/** @brief A tensor in AIfES
*
* This is the main structure to do the calculations with.
* It represets a tensor (a N-dimensional array) of data values of the given type.
*
* For example the matrix
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
*/
struct aitensor {
	const aimath_dtype_t *dtype; /**< The datatype of the tensor, e.g. aif32, aiq7, aiq31, aiu8 */
	uint8_t dim; /**< The number of dimensions */
	uint16_t *shape; /**< An array of dim elements with the shape of the tensor for example [2, 3] */
	void *tensor_params; /**< Parameters to describe some data properties (for example quantization parameters like zero_point and shift) defined by the dtype*/
	void *data; /**< Pointer to the actual tensor data */
};


#endif // AIFES_MATH
