/**
 * \file basic/base/aimath/aimath_basic.h
 * \internal
 * \date 28.10.2020
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
 * \brief	Basic data-type independent math operations
 *
 */

#ifndef AIMATH_BASIC
#define AIMATH_BASIC

#include "core/aifes_math.h"

/** @brief Printing a tensor to console
 *
 * Calls the corresponding print function of the used aimath_dtype.
 *
 * Example:
 * \code{.c}
 * uint16_t tensor_shape[2] = {2, 3};
 * float tensor_data[2*3] = {1.0f, 2.0f, 3.0f,
 *                           4.0f, 5.0f, 6.0f};
 * aitensor_t tensor = AITENSOR_2D_F32(tensor_shape, tensor_data);
 *
 * print_aitensor(&tensor);
 * \endcode
 *
 * @param *tensor	The tensor to print.
 */
void print_aitensor(const aitensor_t *tensor);

/** @brief Printing a scalar to console
 *
 * Calls the corresponding print function of the used aimath_dtype.
 *
 * Example:
 * \code{.c}
 * aiscalar_f32_t scalar = 42.0f;
 *
 * print_aiscalar(&scalar, aif32);
 * \endcode
 *
 * @param *scalar	The scalar to print.
 * @param *dtype    The data type of the scalar
 */
void print_aiscalar(const void *scalar, const aimath_dtype_t *dtype);

/** @brief Transposes a vector
 *
 * @f[
 *   vector \leftarrow vector^T
 * @f]
 *
 * Make sure that the given tensor is a vector (with shape [1 x N] or [N x 1])
 *
 * @param *vector	The vector to transpose (2D tensor of shape [1 x N] or [N x 1])
 */
void aimath_transpose_vector(aitensor_t *vector);

/** @brief Calculates the number of elements in a tensor
 *
 * @param *tensor	The tensor to count the elements of
 * @return Number of elements in tensor
 */
uint32_t aimath_tensor_elements(const aitensor_t *tensor);

/** @brief Returns the size of one value in the given data type in bytes
 *
 * @param *dtype	The data type to get the size of
 * @return Size of one value in the given data type in bytes
 */
uint32_t aimath_sizeof_dtype(const aimath_dtype_t *dtype);

/** @brief Returns size of the data array of the tensor in bytes
 *
 * The size is calculated by:
 *
 * \code aimath_tensor_elements(tensor) * aimath_sizeof_dtype(tensor.dtype) \endcode
 *
 * @param *tensor	The tensor to get the data size of
 * @return Size of tensor data in bytes
 */
uint32_t aimath_sizeof_tensor_data(const aitensor_t *tensor);

/** @brief Returns size of the aitensor.tensor_params for the data type of the tensor
 *
 * @param *tensor	The tensor to get the parameters size of
 * @return Size of tensor parameters in bytes
 */
uint32_t aimath_sizeof_tensor_params(const aitensor_t *tensor);

/** @brief Returns size of tensor in bytes
 *
 * The size is calculated by:
 *
 * \code sizeof(aitensor_t) + aimath_sizeof_tensor_data(tensor) + aimath_sizeof_tensor_params(tensor) \endcode
 *
 * @param *tensor	The tensor to get the size of
 * @return Size of tensor in bytes
 */
uint32_t aimath_sizeof_tensor(const aitensor_t *tensor);

//void aimath_create_tensor_2d(uint16_t shape_0, uint16_t shape_1, aimath_dtype_t dtype, void *memory_ptr);

#endif // AIMATH_BASIC
