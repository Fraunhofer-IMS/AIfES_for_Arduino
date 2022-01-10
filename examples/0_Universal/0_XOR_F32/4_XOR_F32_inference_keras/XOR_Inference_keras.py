#XOR_Inference_keras.py
#author: Fraunhofer IMS

#This example shows how to extract the weights of a trained model and prepare them for import into AIfES.
#The sketch "3_XOR_Inference_keras.ino" is needed for this example.
#Copy the printed weights into the sketch.


import numpy as np
from tensorflow import keras as ke
#from keras.models import Sequential
#from keras.layers.core import Dense


# the four different states of the XOR gate
training_data = np.array([[0,0],[0,1],[1,0],[1,1]], "float32")

# the four expected results in the same order
target_data = np.array([[0],[1],[1],[0]], "float32")

# -- network structure --
#   2 input neurons
#   3 one hidden layer with 3 neurons
#   1 output neuron
network_structure = [2,3,1]

# 1x2 input -> 2x3 hidden sigmoid -> 3x1 sigmoid output
model = ke.Sequential()
model.add(ke.layers.Dense(network_structure[1], input_dim=network_structure[0], activation='sigmoid'))
model.add(ke.layers.Dense(network_structure[2], activation='sigmoid'))

# configure the optimizer
opti = ke.optimizers.Adam(lr=0.1, beta_1=0.9, beta_2=0.999, epsilon=None, decay=0.0, amsgrad=False)

# compile the model and use the mean_squared_error
model.compile(loss='mean_squared_error',
              optimizer=opti,
              metrics=['accuracy'])

# training
model.fit(training_data, target_data,epochs=100, verbose=2)

print("")

# predict
pred = model.predict(training_data)

print("predicted:")
print(pred[0,0])
print(pred[1,0])
print(pred[2,0])
print(pred[3,0])

# get the weights
weights = model.get_weights()

# get the weights from the different layers
hidden_weights = weights[0]
hidden_bias = weights[1]
output_weights = weights[2]
output_bias = weights[3]

print("")
print("copy the weights in the (3_XOR_Inference_keras.ino) example:")
print("")

#print the weights for AIfES
#copy the weights in the "3_XOR_Inference_keras.ino" example
print("float weights_data_dense_1[] = {")
print(str(hidden_weights[0, 0]) + "f,")
print(str(hidden_weights[0, 1]) + "f,")
print(str(hidden_weights[0, 2]) + "f,")
print(str(hidden_weights[1, 0]) + "f,")
print(str(hidden_weights[1, 1]) + "f,")
print(str(hidden_weights[1, 2]) + "f")
print("};")

print("")

print("float bias_data_dense_1[] = {")
print(str(hidden_bias[0]) + "f,")
print(str(hidden_bias[1]) + "f,")
print(str(hidden_bias[2]) + "f")
print("};")

print("")

print("float weights_data_dense_2[] = {")
print(str(output_weights[0, 0]) + "f,")
print(str(output_weights[1, 0]) + "f,")
print(str(output_weights[2, 0]) + "f")
print("};")

print("")

print("float bias_data_dense_2[] = {")
print(str(output_bias[0]) + "f")
print("};")


