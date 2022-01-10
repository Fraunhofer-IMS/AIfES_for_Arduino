import tensorflow as tf
from tensorflow import keras
import numpy as np
from tensorflow.keras import layers
from aifes.tools import quantize_model_q7, Layer, create_flatbuffer_q7, create_flatbuffer_f32, print_flatbuffer_c_style

# ------------------------------------------------- Create and train the model in tf.keras --------------------------------------------------------

model = keras.Sequential()
model.add(keras.Input(shape=(3,)))
model.add(layers.Dense(3, activation="leaky_relu"))
model.add(layers.Dense(2, activation="sigmoid"))

optimizer = keras.optimizers.Adam(lr=0.1)
model.compile(optimizer=optimizer, loss="binary_crossentropy")

model.summary()

X = np.array([[0., 0., 0.], 
              [1., 1., 1.], 
              [1., 0., 0.]])

T = np.array([[1., 0.], 
              [0., 1.], 
              [0., 0.]])

model.fit(X, T, batch_size=4, epochs=5)

'''
# You may set the weights manually instead of training the model.

w1 = np.array([3.64540, -3.60981, 1.57631,
            -2.98952, -1.91465, 3.06150,
            -2.76578, -1.24335, 0.71257]).reshape(3, 3)
            
b1 = np.array([0.72655, 2.67281, -0.21291])

w2 = np.array([-1.09249, -2.44526,
                 3.23528, -2.88023,
                -2.51201,  2.52683]).reshape(3, 2)

b2 = np.array([0.14391, -1.34459])

weights = [w1, b1, w2, b2]
model.set_weights(weights)
'''

# ------------------------------------------------- Convert and quantize the model to a flatbuffer --------------------------------------------------------

# Representation of the model for AIfES pytools
layers = [
    Layer.DENSE_WT,     # Dense / Fully connected layer with transposed weights (WT)
    Layer.LEAKY_RELU,   # Leaky ReLU layer. Add the alpha parameter to the act_params list
    Layer.DENSE_WT,     # Dense / Fully connected layer with transposed weights (WT)
    Layer.SIGMOID       # Sigmoid layer
]
act_params = [0.01]     # Append additional parameters fore the activation functions here (e.g. alpha value for Leaky ReLU)
weights = model.get_weights()

# Platform specific settings
ALIGNMENT = 4           # For example ALIGNMENT = 4 on ARM Cortex-M or ESP32 controllers and ALIGNMENT = 2 on AVR controllers
BYTEORDER = 'little'    # 'little' for little-endian or 'big' for big-endian representation of the target system

result_q_params, weights_q_params, weights_q7 = quantize_model_q7(layers, weights, X, act_params=act_params)

# Print required parameters for a Q7 model to console
print()
print("Layer result quantization parameters (shift, zero point):")
print(result_q_params)
print("Weight and bias quantization parameters (shift, zero point):")
print(weights_q_params)
print("Q7 weights:")
print(weights_q7)
print()

flatbuffer_q7 = create_flatbuffer_q7(result_q_params, weights_q_params, weights_q7, target_alignment=ALIGNMENT, byteorder=BYTEORDER)
flatbuffer_f32 = create_flatbuffer_f32(weights)

# Print the parameter memory buffer to the console
print("\nQ7:")
print_flatbuffer_c_style(flatbuffer_q7, elements_per_line=12, target_alignment=ALIGNMENT, byteorder=BYTEORDER, mutable=True)

print("\nF32:")
print_flatbuffer_c_style(flatbuffer_f32, elements_per_line=8)