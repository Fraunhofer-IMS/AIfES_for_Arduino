/**
 * \file core/aifes_core.h
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
 * \brief AIfES 2 core interface
 *
 * This interface defines the functional modules of AIfES 2 like model, layer, loss and optimizer.
 *
 * The structure of an aimodel and its components can look like in the following example for a simple multilayer perceptron - MLP:
 * \image html functional_concept.jpg width=800px
 */

#ifndef AIFES_CORE
#define AIFES_CORE

#include "aifes_math.h"
#include "aifes_config.h"

#define TRUE	1
#define FALSE	0

#define AILAYER_SETTINGS_TRAINING_MODE                  0
#define AILAYER_SETTINGS_TRAINABLE                      1
#define AILAYER_SETTINGS_BATCH_MODE                     2 // When true, a whole batch is processed in a single forward pass
#define AILAYER_SETTINGS_NO_INPUT_GRADIENT              3 // When true, no input gradient is calculated
#define AILAYER_SETTINGS_KEEP_INPUT_BUFFER_FOR_RESULT   4 // When true, no input gradient is calculated

#define AILAYER_SETTINGS_SET(settings, mask, selector, value)   (settings = ((settings) & ~(mask << (selector))) | ((value) << (selector)))
#define AILAYER_SETTINGS_IS(settings, mask, selector)           (((settings) >> (selector)) & mask)

typedef struct ailayer 	ailayer_t;
typedef struct ailoss 	ailoss_t;
typedef struct aimodel 	aimodel_t;
typedef struct aiopti 	aiopti_t;

typedef struct aicore_layertype aicore_layertype_t;
typedef struct aicore_losstype aicore_losstype_t;
typedef struct aicore_optitype aicore_optitype_t;


/** @brief Type indicator of the layer
 *
 * Every layer should have a constant global variable of this type which indicates the layer type.
 *
 * Example for the dense layer:\n
 * \code{.c}
 * const aicore_layertype_t ailayer_dense_type_s = {
 * #ifdef AIDEBUG_PRINT_MODULE_SPECS
 *     .name = "Dense",
 *     .print_specs = ailayer_dense_print_specs
 * #else
 *     .name = 0,
 *     .print_specs = 0
 * #endif
 * };
 * const aicore_layertype_t *ailayer_dense_type = &ailayer_dense_type_s;
 * \endcode
 *
 * Checks for the layer type can look like this:\n
 * \code{.c}
 * ailayer_t *example_layer;
 * if(example_layer->layer_type == ailayer_dense_type){
 *   ...
 * }
 * \endcode
 */
struct aicore_layertype {
    const char *name; /**< Name of the layer type (for example "Dense") */

    /** @brief Set a function to print specs of the layer (for example size, constants)
    *
    * This function should only be set in the debug mode when prints are required (to save memory otherwise)
    *
	* @param self           The layer
	* @param *print         A function for printing (for example printf)
	*/
	void (*print_specs)(const ailayer_t *self);
};

/** @brief Type indicator of the loss to check for the loss type
 *
 * Every loss should have a constant global variable of this type which indicates the loss type.\n
 * Example for the mse loss:\n
 *
 * \code{.c}
 * const aicore_losstype_t ailoss_mse_type_s = {
 * #ifdef AIDEBUG_PRINT_MODULE_SPECS
 *     .name = "Mean Squared Error",
 *     .print_specs = ailoss_mse_print_spec
 * #else
 *     .name = 0,
 *     .print_specs = 0
 * #endif
 * };
 * const aicore_losstype_t *ailoss_mse_type = &ailoss_mse_type_s;
 * \endcode
 *
 * Checks for the loss type can look like this:\n
 * \code{.c}
 * ailoss_t *example_loss;
 * if(example_loss->loss_type == ailoss_mse_type){
 *   ...
 * }
 * \endcode
 */
struct aicore_losstype {
    const char *name; /**< Name of the loss type (for example "Mean Squared Error") */

    /** @brief Set a function to print specs of the loss
    *
    * This function should only be set in the debug mode when prints are required (to save memory otherwise)
    *
	* @param self           The loss
	* @param *print         A function for printing (for example printf)
	*/
	void (*print_specs)(const ailoss_t *self);
};

/** @brief Type indicator of the optimizer to check for the optimizer type
 *
 * Every optimizer should have a constant global variable of this type which indicates the optimizer type.\n
 * Example for the adam optimizer:\n
 *
 * \code{.c}
 * const aicore_optitype_t aiopti_adam_type_s = {
 * #ifdef AIDEBUG_PRINT_MODULE_SPECS
 *     .name = "ADAM",
 *     .print_specs = aiopti_adam_print_specs
 * #else
 *     .name = 0,
 *     .print_specs = 0
 * #endif
 * };
 * const aicore_optitype_t *aiopti_adam_type = &aiopti_adam_type_s;
 * \endcode
 *
 * Checks for the optimizer type can look like this:\n
 * \code{.c}
 * aiopti_t *example_optimizer;
 * if(example_optimizer->optimizer_type == aiopti_adam_type){
 *   ...
 * }
 * \endcode
 */
struct aicore_optitype {
    const char *name; /**< Name of the optimizer type (for example "ADAM") */

    /** @brief Set a function to print specs of the optimizer
    *
    * This function should only be set in the debug mode when prints are required (to save memory otherwise)
    *
	* @param self           The optimizer
	* @param *print         A function for printing (for example printf)
	*/
	void (*print_specs)(const aiopti_t *self);
};

/** @brief AIfES artificial neural network model
*
* \image html aimodel.png width=500px
*
* This structure holds the model structure and compute graph of the ANN. (Refer to aifes_core.h for a structural overview)\n
* A aimodel contains several layers that are connected in a list or graph structure.\n
* When the model should be trained, a loss function is required. Otherwise it can be set to NULL.
*/
struct aimodel {
	ailayer_t *input_layer; /**< Input layer of the model that gets the input data. */
	ailayer_t *output_layer; /**< Output layer of the model. The output of a forwad pass is available in the result tensor of this layer. */

	uint16_t layer_count; /**< Total number of layers of the model (usually autogenerated). */
	uint16_t trainable_params_count; /**< Total number of trainable parameter tensors */

	ailoss_t *loss; /**< The loss or cost function of the model (only for training). */
};


/** @brief AIfES layer interface
*
* \image html ailayer.png width=200px
*
* The interface contains the necessary functions and parameters for inference and training on the model.
* (Refer to aifes_core.h for a structural overview)
*
* The call order of the functions for inference:\n
* \code{.c}
*
* for each layer in the model
*     calc_result_shape()
* endfor
*
* for each layer in the model
*     forward()
* endfor
* // The result of the inference is now in output_layer.result tensor
* \endcode
*
* The call order of the functions for training:\n
* \code{.c}
*
* for each layer in the model
*     calc_result_shape()
* endfor
*
* // If the parameters are not already pretrained, a new parameter memory block can be created
* for each layer in the model
*     allocate memory of size sizeof_paramem()
*     set_paramem()
* endfor
* init_trainable_parameters_model() // Do some weight initialization
*
* for each layer in the model
*     allocate memory of size sizeof_trainmem()
*     set_trainmem()
* endfor
*
* init_trainmem_model() // For example set the optimem to zero
* for iterations
*     for each batch in dataset
*         zero_gradients_model()
*
*         for each sample in the batch
*             for each layer in the model
*                 forward()
*             endfor
*
*             calc_delta()
*             for each layer in the model (reverse)
*                 backward()
*             endfor
*         endfor
*
*         update_params_model()
*     endfor
* endfor
* \endcode
*/
struct ailayer {
	const aicore_layertype_t *layer_type; /**< Type of the layer (for example ailayer_dense_type) */
	void *layer_configuration; /**< Layer specific configurations (back-link from abstract layer class to implementation) */

    /** @brief General layer settings like freezing weights or switching between training and evaluation mode.
	*
	* **Example:** Read a value from the settings
    * \code{c}
    * if(AILAYER_SETTINGS_IS(layer->settings, 0b1, AILAYER_SETTINGS_TRAINABLE)){
    *   ...
    * }
    * \endcode
    *
	* **Example:** Write a value to the settings
    * \code{c}
    * AILAYER_SETTINGS_SET(layer->settings, 0b1, AILAYER_SETTINGS_TRAINABLE, FALSE);
    * \endcode
	*
	*/
	uint32_t settings; /**< General layer settings like freezing weights or switching between training and evaluation mode. */

	/** @name Layer connections
	* Defines the model graph.
	*/
	///@{
	ailayer_t *input_layer;
	//ailayer_t *brother_input_layer; /**< (NOT_IN_USE) Chained list if multiple input layer are present else NULL. */

	ailayer_t *output_layer;
	//ailayer_t *brother_output_layer; /**< (NOT_IN_USE) Chained list if multiple output layer are present else NULL. */
	///@}

    /** @name Inference and training scheduling order (Not in use yet)
	* @brief The scheduler can execute the layers along this path.
	*/
	///@{
	ailayer_t *next_scheduled;
	ailayer_t *prev_scheduled;
	///@}

	aitensor_t result; /**< The result of the forward function is stored here */

	/** @brief Calculate and write the shape to the result tensor.
	*
	* Made for easy creation of the model (layers can be connected to each other without worrying about the shapes).
	*
	* @param self           The layer
	*/
	void (*calc_result_shape)(ailayer_t *self);

	/** @brief If available, calculate and set the tensor_params of the result tensor.
	*
	* Some layers (like some activation functions) have pre-defined tensor_params that can be set to the result tensor with this function.
	* Set this function to 0 if the tensor_params calculation is not trivial (e.g. dependent on input data).
	*
	* @param self           The layer
	*/
	void (*calc_result_tensor_params)(ailayer_t *self);

	/** @brief Calculate the forward pass and write the result to the result tensor.
	*
	* @param self           The layer
	*/
	void (*forward)(ailayer_t *self);

	// Maybe for later purpose
	//void (*sizeof_infmem)(struct aifes_layer_t *, void *);
	//void (*set_infmem)(struct aifes_layer_t *, void *);

	// ------------------ Used for training only: -----------------------

	aitensor_t deltas; /**< The result of the backward function is stored here. */

	/** @name Training memory API
	* @brief Makes the memory of the trainable params, the gradients and optimizer stuff accessible.
	*
	* This is used, for example, for the optimiser or debugging purposes.
	*/
	///@{
	uint8_t trainable_params_count; /**< Number of trainable parameter tensors. */
	aitensor_t **trainable_params; /**< Array of tensor pointers with length trainable_params_count. */
	aitensor_t **gradients; /**< Array of tensor pointers with length trainable_params_count. */
	void **optimem; /**< Array of memory pointers with length trainable_params_count. */
	///@}

	/** @brief Calculate the backward pass and write the result to the deltas tensor.
	*
	* @param self           The layer
	*/
	void (*backward)(ailayer_t *self);

	/** @name Parameter memory
	* @brief Calculate the size and set the memory for the parameter.
	*
	* This memory  (for example for weights, bias, ...) will last through all the lifetime of the model.\n
	* This is only intended for training when no initial weights are available. If the parameters are already known,
	* set the parameter directly to the layer.
	*/
	///@{
	uint32_t (*sizeof_paramem)(const ailayer_t *self); /**< Size of required memory (in bytes). */
	void (*set_paramem)(ailayer_t *self, void* memory_ptr); /**< Set and distribute the memory block internally. */
	void (*init_params)(ailayer_t *self); /**< Initialize the (trainable and not trainable) parameters of the layer with default initializers. */
	///@}

	/** @name Temporary memory for forward and backward pass
	* @brief Calculate the size of the required memory for temporary result in forward and backward pass
	*/
	///@{
	uint32_t (*sizeof_fwdmem)(const ailayer_t *self); /**< Size of required memory for the forward pass (in bytes). */
	uint32_t (*sizeof_bwdmem)(const ailayer_t *self); /**< Size of required memory for the backward pass (in bytes). */
	void *tempmem; /**< Pointer to the memory for the forward pass, backward pass and the optimizer. */
	///@}

	/** @name Training memory
	* @brief Calculate the size and set the working memory for the training
	*
	* This memory  (for example for gradients, momentums, ...) is needed during the whole training process.
	* If the training is finished, it can be deleted.
	*/
	///@{
	uint32_t (*sizeof_trainmem)(const ailayer_t *self); /**< Size of required memory (in bytes). */
	void (*set_trainmem)(ailayer_t *self, void* memory_ptr); /**< Set and distribute the memory block internally. */
	///@}
};


/** @brief AIfES loss interface.
*
* \image html ailoss.png width=300px
*
* The interface contains the necessary functions and parameters for loss and error calculation on the target data.
* (Refer to aifes_core.h for a structural overview)
*/
struct ailoss {
	const aicore_losstype_t *loss_type; /**< Type of the loss (for example ailoss_mse_type) */
	void *loss_configuration; /**< Loss specific configurations (back-link from abstract loss class to implementation) */

	ailayer_t connection_layer; /**< Dummy layer for docking to the layer structure */

	/** @brief Calculate the loss / cost for the model on the given targets.
    *
	* @param self           The layer
	* @param target_data    Tensor containing the target data / labels
	* @param result         Scalar in which the output can be written (aiscalar of same type as layer type).
	*/
	void (*calc_loss)(ailoss_t *self, const aitensor_t *target_data, void *result);

    /** @brief Calculate the error on the target data and write it to the deltas tensor of connection layer
    *
	* @param self           The layer
	* @param target_data    Tensor containing the target data / labels
	*/
	void (*calc_delta)(ailoss_t *self, const aitensor_t *target_data);
};


/** @brief AIfES optimizer interface.
*
* \image html aiopti.png width=300px
*
* The interface contains the necessary functions and parameters for parameter optimizers in backpropagation training.
* (Refer to aifes_core.h for a structural overview)\n
* Optimizers are responsible for updating the trainable parameters of the model with the gradients
* calculated in the backward pass.
*
* The call order of the functions is:\n
* \code{.c}
* allocate memory of size sizeof_optimem()
* init_optimem()
*
* for batch in dataset
*     for each trainable parameter tensor in the model
*         zero_gradients()
*     endfor
*
*     forward_model(batch)
*     backward_model(batch)
*
*     begin_step()
*     for each trainable parameter tensor in the model
*         update_params()
*     endfor
*     end_step()
* endfor
* \endcode
*/
struct aiopti {
	const aicore_optitype_t *optimizer_type; /**< Type of the optimizer (for example aiopti_sgd_type) */
	void *optimizer_configuration; /**< Optimizer specific configurations (back-link from abstract aiopti class to implementation) */
	const aimath_dtype_t *dtype; /**< The data-type of the parameter that the optimizer can optimize and the learning rate */

	void *learning_rate; /**< The learning rate configures the training speed. The learning rate is an aiscalar_t value of given dtype. */

    /** @brief Calculates the optimization memory size for a trainable parameter tensor.
    *
	* @param self           The layer
	* @param params         The trainable parameter tensor
	*/
	uint32_t (*sizeof_optimem)(aiopti_t *self, const aitensor_t *params);

    /** @brief Initialize the optimization memory for a trainable parameter tensor.
    *
	* @param self           The layer
	* @param params         The trainable parameter tensor
	* @param gradients      The associated gradients tensor
	* @param optimem        The associated optimization memory to initialize
	*/
	void (*init_optimem)(aiopti_t *self, const aitensor_t *params, const aitensor_t *gradients, void *optimem);

    /** @brief Set the gradient tensor to zero.
    *
	* @param self           The layer
	* @param params         The gradient tensor
	*/
	void (*zero_gradients)(aiopti_t *self, aitensor_t *gradients);

    /** @brief Called in the beginning of every model optimization step for parameter initialization.
    *
	* @param self           The layer
	*/
	void (*begin_step)(aiopti_t *self);

    /** @brief Performs an optimization step on the given tensor.
    *
	* @param self           The layer
	* @param params         The trainable parameter tensor
	* @param gradients      The associated gradients tensor
	* @param optimem        The associated optimization memory to initialize
	*/
	void (*update_params)(aiopti_t *self, aitensor_t *params, const aitensor_t *gradients, void *optimem);

    /** @brief Called in the end of every model optimization step
    *
	* @param self           The layer
	*/
	void (*end_step)(aiopti_t *self);
};


#endif // AIFES_CORE
