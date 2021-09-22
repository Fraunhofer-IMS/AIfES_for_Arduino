''' 
Author: Hahn-Schickard-Gesellschaft für angewandte Forschung e.V., Daniel Konegen + Marcus Rueb
'''

import tensorflow as tf
from tensorflow.keras.utils import to_categorical

from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Dense

import cv2


def train_model():
    '''
    Load and preprocess the MNIST data set and train a DNN.
    '''

    (x_train, y_train), (x_test, y_test) = tf.keras.datasets.mnist.load_data()

    x_train = x_train.astype(float)/255.0
    x_test = x_test.astype(float)/255.0

    x_train = x_train.reshape(60000, 784)
    x_test = x_test.reshape(10000, 784)

    y_train = to_categorical(y_train, 10)
    y_test = to_categorical(y_test, 10)

    print(x_train.shape)
    print(x_test.shape)


    model = Sequential()
    model.add(Dense(10, activation='relu', input_shape=(784,)))
    model.add(Dense(10, activation='relu'))
    model.add(Dense(10, activation='softmax'))

    model.summary()

    model.compile(optimizer="Adam", loss="categorical_crossentropy", metrics=["acc"])

    model.fit(x_train, y_train, validation_split=0.2, epochs=1, batch_size=100)

    print("Accuracy on test data:")
    model.evaluate(x_test, y_test)

    return model


def write_weights(model):
    '''
    Load weights of the passed neural network. For each layer, the weights and the 
    bias are saved in a header file as an array.
    '''
    
    # Get the weights
    weights = model.get_weights()

    # Write the weights for AIfES in a .h file
    with open("MNIST_weights.h", "w") as f: 
        for k in range(0,len(weights)):
            if (k%2) == 0:
                f.write("float weights_data_dense_" + str(int(k/2)+1) + "[] = {")
            else:
                f.write("float bias_data_dense_" + str(int(k/2)+1) + "[] = {")
            
            if len(weights[k].shape) == 2:
                for i in range(0,weights[k].shape[0]):
                    for j in range(0,weights[k].shape[1]):
                        if i == (weights[k].shape[0]-1) and j == (weights[k].shape[1]-1):
                            f.write(str(weights[k][i, j]) + "f")
                        else:
                            f.write(str(weights[k][i, j]) + "f, ")
                f.write("};\n")
            else:
                for i in range(0,weights[k].shape[0]):
                    if i == (weights[k].shape[0]-1):
                        f.write(str(weights[k][i]) + "f")
                    else:
                        f.write(str(weights[k][i]) + "f, ")
                f.write("};\n")

            f.write("\n")


def show_MNIST_test_img(i):
    '''
    An image of the test data set can be displayed for verification.
    '''
    
    (_, _), (x_test, y_test) = tf.keras.datasets.mnist.load_data()
    x_test = x_test.astype(float)/255.0
    img = cv2.resize(x_test[i], (200, 200)) 
    print(y_test[i])
    cv2.imshow('image',img)
    cv2.waitKey(0)


mnist_model = train_model()
write_weights(mnist_model)
# show_MNIST_test_img(18)