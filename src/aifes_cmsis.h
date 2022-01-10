/**
 * \file basic/aifes_basic_cmsis.h
 * \internal
 * \date 23.11.2021
 * \endinternal
 * \version 2.0alpha
 * \copyright  Copyright (C) 2020-2021  Fraunhofer Institute for Microelectronic Circuits and Systems.
    All rights reserved.

    AIfES is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * \brief Include all headers of the AIfES 2 basic module with ARM CMSIS implementations
 * \details
 */

#define AIFES_WITH_CMSIS

#ifdef __cplusplus
extern "C" {
#endif

// Include the math in cmsis implementation
#include "basic/cmsis/aimath/aimath_f32_cmsis.h"
#include "basic/cmsis/aimath/aimath_q31_cmsis.h"
#include "basic/cmsis/aimath/aimath_q7_cmsis.h"

// Include the layers in cmsis implementation
#include "basic/cmsis/ailayer/ailayer_dense_cmsis.h"

#ifdef __cplusplus
} // End extern "C"
#endif
