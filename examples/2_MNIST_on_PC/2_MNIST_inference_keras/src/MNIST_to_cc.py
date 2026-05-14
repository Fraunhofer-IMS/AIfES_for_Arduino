''' 
Author: Hahn-Schickard-Gesellschaft für angewandte Forschung e.V., Daniel Konegen + Marcus Rueb
'''

import tensorflow as tf
from tensorflow.keras.utils import to_categorical


# Load and preprocess the MNIST data set
(x_train, y_train), (x_test, y_test) = tf.keras.datasets.mnist.load_data()

x_train = x_train.astype(float)/255.0
x_test = x_test.astype(float)/255.0

y_train = to_categorical(y_train)
y_test = to_categorical(y_test)

print(x_train.shape)
print(x_test.shape)

NUM_TRAINING_DATA = 30000   # Max 60000
NUM_TEST_DATA = 10000       # Max 10000

def generate_train_data():
    '''
    Generates two header files. The first one contains as many training data according to the 
    number defined above. The second header file contains the labels belonging to the training 
    data. The training data and labels are both stored in a two-dimensional array.
    '''

    with open("MNIST_training_data.h", "w") as f:    
        f.write("float MNIST_training_data[" + str(NUM_TRAINING_DATA) + "][784] = {\n")
        for i in range(0,NUM_TRAINING_DATA):
            if i != 0:
                f.write("},\n")
            x_train_flatten = x_train[i].flatten()
            f.write("{" + str(x_train_flatten[0]) + "f")
            for j in range(1,784):
                f.write(", " + str(x_train_flatten[j]) + "f")
        f.write("}\n};")


    with open("MNIST_training_data_label.h", "w") as f:    
        f.write("float MNIST_training_data_label[" + str(NUM_TRAINING_DATA) + "][10] = {\n")
        for i in range(0,NUM_TRAINING_DATA):
            if i != 0:
                f.write("},\n")
            f.write("{" + str(y_train[i][0]) + "f")
            for j in range(1,10):
                f.write(", " + str(y_train[i][j]) + "f")
        f.write("}\n};")


def generate_test_data():
    '''
    Generates two header files. The first one contains as many test data according to the 
    number defined above. The second header file contains the labels belonging to the test 
    data. The test data and labels are both stored in a two-dimensional array.
    '''

    with open("MNIST_test_data.h", "w") as f:    
        f.write("float MNIST_test_data[" + str(NUM_TEST_DATA) + "][784] = {\n")
        for i in range(0,NUM_TEST_DATA):
            if i != 0:
                f.write("},\n")
            x_test_flatten = x_test[i].flatten()
            f.write("{" + str(x_test_flatten[0]) + "f")
            for j in range(1,784):
                f.write(", " + str(x_test_flatten[j]) + "f")
        f.write("}\n};")


    with open("MNIST_test_data_label.h", "w") as f:    
        f.write("float MNIST_test_data_label[" + str(NUM_TEST_DATA) + "][10] = {\n")
        for i in range(0,NUM_TEST_DATA):
            if i != 0:
                f.write("},\n")
            f.write("{" + str(y_test[i][0]) + "f")
            for j in range(1,10):
                f.write(", " + str(y_test[i][j]) + "f")
        f.write("}\n};")


# generate_train_data()
generate_test_data()