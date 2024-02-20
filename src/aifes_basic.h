/**
 * \file basic/aifes_basic.h
 * \internal
 * \date 23.02.2021
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
 * \brief Include all headers of the AIfES 2 basic module
 * \details
 */

// Include AIfES core headers
#include "core/aifes_math.h"
#include "core/aifes_core.h"

#ifdef __cplusplus
extern "C" {
#endif

// Include the datatypes
#include "basic/base/aimath/aimath_f32.h"
#include "basic/base/aimath/aimath_q31.h"
#include "basic/base/aimath/aimath_q7.h"
#include "basic/base/aimath/aimath_u8.h"

// Include basic datatype independent math functions
#include "basic/base/aimath/aimath_basic.h"

// ---------------------------- Module base implementations -----------------------
// ("abstract" super "classes". A hardware optimized implementation can "inherit" from these modules)

// Include the layer base implementations
#include "basic/base/ailayer/ailayer_dense.h"
#include "basic/base/ailayer/ailayer_input.h"
#include "basic/base/ailayer/ailayer_relu.h"
#include "basic/base/ailayer/ailayer_leaky_relu.h"
#include "basic/base/ailayer/ailayer_elu.h"
#include "basic/base/ailayer/ailayer_sigmoid.h"
#include "basic/base/ailayer/ailayer_tanh.h"
#include "basic/base/ailayer/ailayer_softmax.h"
#include "basic/base/ailayer/ailayer_softsign.h"

// Include the loss base implementations
#include "basic/base/ailoss/ailoss_mse.h"
#include "basic/base/ailoss/ailoss_crossentropy.h"

// Include the optimizer base implementations
#include "basic/base/aiopti/aiopti_sgd.h"
#include "basic/base/aiopti/aiopti_adam.h"

// ---------------------------- Module default implementations -----------------------
// (Fallback functions if no hardware optimized implementation available)

// Include the math in default implementation
#include "basic/default/aimath/aimath_f32_default.h"
#include "basic/default/aimath/aimath_q31_default.h"
#include "basic/default/aimath/aimath_q7_default.h"

// Include the layers in default implementation
#include "basic/default/ailayer/ailayer_dense_default.h"
#include "basic/default/ailayer/ailayer_input_default.h"
#include "basic/default/ailayer/ailayer_relu_default.h"
#include "basic/default/ailayer/ailayer_leaky_relu_default.h"
#include "basic/default/ailayer/ailayer_elu_default.h"
#include "basic/default/ailayer/ailayer_sigmoid_default.h"
#include "basic/default/ailayer/ailayer_tanh_default.h"
#include "basic/default/ailayer/ailayer_softmax_default.h"
#include "basic/default/ailayer/ailayer_softsign_default.h"

// Include the losses in default implementation
#include "basic/default/ailoss/ailoss_mse_default.h"
#include "basic/default/ailoss/ailoss_crossentropy_default.h"

// Include the optimizers in default implementation
#include "basic/default/aiopti/aiopti_sgd_default.h"
#include "basic/default/aiopti/aiopti_adam_default.h"

// ---------------------------- Algorithmic -----------------------

// Include the algorithmic
#include "basic/base/aialgo/aialgo_sequential_inference.h"
#include "basic/base/aialgo/aialgo_sequential_training.h"

// ---------------------------- AIfES express -----------------------

// Include AIfES express functions (high level api)

#include "basic/express/aifes_express_f32_fnn.h"
#include "basic/express/aifes_express_q7_fnn.h"


#ifdef __cplusplus
} // End extern "C"
#endif
