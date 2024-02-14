/**
 * \file cnn/aifes_cnn.h
 * \internal
 * \date 19.10.2021
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
 * \brief Include all headers of the aifes 2 - cnn module
 * \details
 */

#ifndef AIFES_CNN_H
#define AIFES_CNN_H

#ifdef __cplusplus
extern "C" {
#endif

// Include the layer base implementations
#include "cnn/base/ailayer/ailayer_conv2d.h"
#include "cnn/base/ailayer/ailayer_reshape.h"
#include "cnn/base/ailayer/ailayer_maxpool2d.h"
#include "cnn/base/ailayer/ailayer_batch_normalization.h"

// ---------------------------- Module default implementations -----------------------
// (Fallback functions if no hardware optimized implementation available)

// Include the math in default implementation
#include "cnn/default/aimath/aimath_cnn_f32_default.h"

// Include the layers in default implementation
#include "cnn/default/ailayer/ailayer_conv2d_default.h"
#include "cnn/default/ailayer/ailayer_reshape_default.h"
#include "cnn/default/ailayer/ailayer_maxpool2d_default.h"
#include "cnn/default/ailayer/ailayer_batch_normalization_default.h"


#ifdef __cplusplus
} // End extern "C"
#endif

#endif // AIFES_CNN_H
