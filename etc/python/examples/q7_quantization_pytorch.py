import numpy as np
import torch
from torch import nn
from aifes.tools import quantize_model_q7, Layer, create_flatbuffer_q7, create_flatbuffer_f32, print_flatbuffer_c_style

# ------------------------------------------------- Create and train the model in PyTorch --------------------------------------------------------

class Net(nn.Module):
 
    def __init__(self):
        super(Net, self).__init__()
        
        self.dense_layer_1 = nn.Linear(3, 3)
        self.leaky_relu_layer = nn.LeakyReLU(0.01)
        self.dense_layer_2 = nn.Linear(3, 2)
        self.sigmoid_layer = nn.Sigmoid()
 
    def forward(self, x):
        x = self.dense_layer_1(x)
        x = self.leaky_relu_layer(x)
        x = self.dense_layer_2(x)
        x = self.sigmoid_layer(x)
        return x
        
        
X = np.array([[0., 0., 0.], 
              [1., 1., 1.], 
              [1., 0., 0.]])

Y = np.array([[1., 0.], 
              [0., 1.], 
              [0., 0.]])
              
              
X_tensor = torch.FloatTensor(X)
Y_tensor = torch.FloatTensor(Y)

model = Net()
criterion = nn.BCELoss()
optimizer = torch.optim.Adam(model.parameters(), lr=0.1)

epochs = 200
model.train()
for epoch in range(epochs):
    optimizer.zero_grad()
    pred = model(X_tensor)
    loss = criterion(pred, Y_tensor)
    loss.backward()
    optimizer.step()

# ------------------------------------------------- Convert and quantize the model to a flatbuffer --------------------------------------------------------

# Representation of the model for AIfES pytools
layers = [
    Layer.DENSE_WT,     # Dense / Fully connected layer with transposed weights (WT)
    Layer.LEAKY_RELU,   # Leaky ReLU layer. Add the alpha parameter to the act_params list
    Layer.DENSE_WT,     # Dense / Fully connected layer with transposed weights (WT)
    Layer.SIGMOID       # Sigmoid layer
]
act_params = [0.01]     # Append additional parameters fore the activation functions here (e.g. alpha value for Leaky ReLU)
weights = [param.detach().numpy().T for param in model.parameters()]

# Platform specific settings
ALIGNMENT = 4           # For example ALIGNMENT = 4 on ARM Cortex-M or ESP32 controllers and ALIGNMENT = 2 on AVR controllers (must equal AIFES_MEMORY_ALIGNMENT)
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
print_flatbuffer_c_style(flatbuffer_q7, elements_per_line=10, target_alignment=ALIGNMENT, byteorder=BYTEORDER, mutable=True)

print("\nF32:")
print_flatbuffer_c_style(flatbuffer_f32, elements_per_line=8)