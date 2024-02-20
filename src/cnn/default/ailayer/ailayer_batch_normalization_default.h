/**
 * \file cnn/default/ailayer/ailayer_batch_normalization_default.h
 * \internal
 * \date 31.01.2022
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
 * \brief Default implementation of the \link ailayer_batch_normalization.h Batch Normalization layer \endlink
 *
 * Hardware independent implementations of the Batch Normalization layer in \link aimath_f32.h F32 \endlink data-type.
 *
 * For more information about the Batch Normalization layer refer to ailayer_batch_normalization.h.
 */

#ifndef AILAYER_BATCH_NORM_DEFAULT
#define AILAYER_BATCH_NORM_DEFAULT

#include "cnn/base/ailayer/ailayer_batch_normalization.h"
#include "basic/default/aimath/aimath_f32_default.h"
#include "cnn/default/aimath/aimath_cnn_f32_default.h"

#define AILAYER_BATCH_NORM_F32_M(momentum, eps, moving_mean, moving_variance, beta, gamma) \
 {{{0,},0,0,0,{0,0,0,0,(float *) beta},{0,0,0,0,(float *) gamma}, \
 {0,0,0,0,(float *) moving_mean},{0,0,0,0,(float *) moving_variance} }, momentum, eps}
#define AILAYER_BATCH_NORM_F32_A(momentum, eps)   {{{0,}}, momentum, eps}


typedef struct ailayer_batch_norm_f32 	ailayer_batch_norm_f32_t;

/** @brief Data-type specific \link ailayer_batch_normalization.h Batch Normalization layer \endlink struct for \link aimath_f32.h F32 \endlink
 *
 * Adds data fields for the momentum and epsilon value in \link aimath_f32.h F32 \endlink to the base implementation.
 */
struct ailayer_batch_norm_f32 {
    ailayer_batch_norm_t base; /**< Inherited field members from general layer struct. */

	aiscalar_f32_t momentum; /**< Storage for ailayer_batch_norm.momentum scalar in F32 */
	aiscalar_f32_t eps; /**< Storage for ailayer_batch_norm.eps scalar in F32 */
};

/// @brief Initializes and connect a \link ailayer_batch_normalization.h Batch Normalization layer \endlink with the \link aimath_f32.h F32 \endlink default implementation
///
/// **Example:** Create the layer structure with pretrained weights, means and variances:\n
/// In C:
/// \code{.c}
/// // Use constant data only for inference. For training remove the const qualifier!!
/// const float means_data_bn[3] = {-0.743f, 1.133f, -0.05f};
/// const float vars_data_bn[3] = {0.9f, 1.0f, 0.87f};
/// const float betas_data_bn[3] = {0.1f, -0.1f, 0.0f};
/// const float gammas_data_bn[3] = {4.0f, 2.0f, 1.0f};
/// ailayer_batch_norm_f32_t bn_layer = {
///     .momentum =                 0.9f,
///     .eps =                      1e-6f,
///     .base.moving_means =        means_data_bn,
///     .base.moving_variances =    vars_data_bn,
///     .base.betas =               betas_data_bn,
///     .base.gammas =              gammas_data_bn
/// };
/// \endcode
/// In C, C++ and on Arduino:
/// \code{.c}
/// // Use constant data only for inference. For training remove the const qualifier!!
/// const float means_data_bn[3] = {-0.743f, 1.133f, -0.05f};
/// const float vars_data_bn[3] = {0.9f, 1.0f, 0.87f};
/// const float betas_data_bn[3] = {0.1f, -0.1f, 0.0f};
/// const float gammas_data_bn[3] = {4.0f, 2.0f, 1.0f};
/// ailayer_batch_norm_f32_t bn_layer = AILAYER_BATCH_NORM_F32_M(
///                                                                 /* momentum =*/         0.9f,
///                                                                 /* eps =*/              1e-6f,
///                                                                 /* means =*/            means_data_bn,
///                                                                 /* variances =*/        vars_data_bn,
///                                                                 /* betas (offset) =*/   betas_data_bn,
///                                                                 /* gammas (scale) =*/   gammas_data_bn
///                                                              );
/// \endcode
///
/// **Example:** Create the layer structure for automatic parameter distribution (e.g. for training):\n
/// In C:
/// \code{.c}
/// ailayer_batch_norm_f32_t bn_layer = {
///     .momentum =                 0.9f,
///     .eps =                      1e-6f
/// };
/// \endcode
/// In C, C++ and on Arduino:
/// \code{.c}
/// ailayer_batch_norm_f32_t bn_layer = AILAYER_BATCH_NORM_F32_A(
///                                                                 /* momentum =*/         0.9f,
///                                                                 /* eps =*/              1e-6f
///                                                              );
/// \endcode
///
/// **Example:** Initialize and connect the layer for data with channels first (all options are equivalent):\n
/// \code{.c}
/// bn_layer.channel_axis = 1;
/// x = ailayer_batch_norm_f32_default(&bn_layer, x);
/// \endcode
/// or
/// \code{.c}
/// x = ailayer_batch_norm_cfirst_f32_default(&bn_layer, x);
/// \endcode
/// or
/// \code{.c}
/// x = ailayer_batch_norm_chw_f32_default(&bn_layer, x);
/// \endcode
/// or
/// \code{.c}
/// x = ailayer_batch_norm_cl_f32_default(&bn_layer, x);
/// \endcode
///
/// **Example:** Initialize and connect the layer for data with channels last (all options are equivalent):\n
/// \code{.c}
/// bn_layer.channel_axis = -1;
/// x = ailayer_batch_norm_f32_default(&bn_layer, x);
/// \endcode
/// or
/// \code{.c}
/// x = ailayer_batch_norm_clast_f32_default(&bn_layer, x);
/// \endcode
/// or
/// \code{.c}
/// x = ailayer_batch_norm_hwc_f32_default(&bn_layer, x);
/// \endcode
/// or
/// \code{.c}
/// x = ailayer_batch_norm_lc_f32_default(&bn_layer, x);
/// \endcode
///
/// @param *layer        The layer structure to initialize.
/// @param *input_layer  The prior layer.
/// @return              The (successfully) initialized layer structure.
///
ailayer_t *ailayer_batch_norm_f32_default(ailayer_batch_norm_f32_t *layer, ailayer_t *input_layer);

/** @brief Initializes and connect a \link ailayer_batch_normalization.h Batch Normalization layer \endlink (channels first) with the \link aimath_f32.h F32 \endlink default implementation
 *
 * Code examples are given in the description of ailayer_batch_norm_f32_default().
 *
 * @param *layer        The layer structure to initialize.
 * @param *input_layer  The prior layer.
 * @return              The (successfully) initialized layer structure.
 */
ailayer_t *ailayer_batch_norm_cfirst_f32_default(ailayer_batch_norm_f32_t *layer, ailayer_t *input_layer);

/** @brief Initializes and connect a \link ailayer_batch_normalization.h Batch Normalization layer \endlink (channels first) with the \link aimath_f32.h F32 \endlink default implementation
 *
 * Code examples are given in the description of ailayer_batch_norm_f32_default().
 *
 * @param *layer        The layer structure to initialize.
 * @param *input_layer  The prior layer.
 * @return              The (successfully) initialized layer structure.
 */
ailayer_t *ailayer_batch_norm_chw_f32_default(ailayer_batch_norm_f32_t *layer, ailayer_t *input_layer);

/** @brief Initializes and connect a \link ailayer_batch_normalization.h Batch Normalization layer \endlink (channels first) with the \link aimath_f32.h F32 \endlink default implementation
 *
 * Code examples are given in the description of ailayer_batch_norm_f32_default().
 *
 * @param *layer        The layer structure to initialize.
 * @param *input_layer  The prior layer.
 * @return              The (successfully) initialized layer structure.
 */
ailayer_t *ailayer_batch_norm_cl_f32_default(ailayer_batch_norm_f32_t *layer, ailayer_t *input_layer);


/** @brief Initializes and connect a \link ailayer_batch_normalization.h Batch Normalization layer (channels last) \endlink with the \link aimath_f32.h F32 \endlink default implementation
 *
 * Code examples are given in the description of ailayer_batch_norm_f32_default().
 *
 * @param *layer        The layer structure to initialize.
 * @param *input_layer  The prior layer.
 * @return              The (successfully) initialized layer structure.
 */
ailayer_t *ailayer_batch_norm_clast_f32_default(ailayer_batch_norm_f32_t *layer, ailayer_t *input_layer);

/** @brief Initializes and connect a \link ailayer_batch_normalization.h Batch Normalization layer (channels first) \endlink with the \link aimath_f32.h F32 \endlink default implementation
 *
 * Code examples are given in the description of ailayer_batch_norm_f32_default().
 *
 * @param *layer        The layer structure to initialize.
 * @param *input_layer  The prior layer.
 * @return              The (successfully) initialized layer structure.
 */
ailayer_t *ailayer_batch_norm_hwc_f32_default(ailayer_batch_norm_f32_t *layer, ailayer_t *input_layer);

/** @brief Initializes and connect a \link ailayer_batch_normalization.h Batch Normalization layer (channels first) \endlink with the \link aimath_f32.h F32 \endlink default implementation
 *
 * Code examples are given in the description of ailayer_batch_norm_f32_default().
 *
 * @param *layer        The layer structure to initialize.
 * @param *input_layer  The prior layer.
 * @return              The (successfully) initialized layer structure.
 */
ailayer_t *ailayer_batch_norm_lc_f32_default(ailayer_batch_norm_f32_t *layer, ailayer_t *input_layer);


/** @brief \link aimath_f32.h F32 \endlink default implementation of the ailayer.init_params function for the Batch Normalization layer
 *
 * *Implementation of ailayer.init_params.*
 *
 * The function will initialize the tensors for moving_means and betas with zeros and for moving_variances and gammas with ones.
 *
 * @param *self  The layer structure
 */
void ailayer_batch_norm_init_params_f32_default(ailayer_t *self);

#endif // AILAYER_BATCH_NORM_DEFAULT
