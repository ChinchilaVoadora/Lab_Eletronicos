import tensorflow as tf
from tensorflow.keras import layers, models
from tensorflow.keras.datasets import mnist
import os

# -------------------------
# 1. Load MNIST dataset
# -------------------------
(x_train, y_train), (x_test, y_test) = mnist.load_data()
x_train = x_train.astype('float32') / 255.0  # normalize
x_test = x_test.astype('float32') / 255.0

# Add channel dimension for CNN
x_train = x_train[..., tf.newaxis]  # shape: (num_samples, 28,28,1)
x_test = x_test[..., tf.newaxis]

# -------------------------
# 2. Define the network
# -------------------------
model = models.Sequential([
    layers.Conv2D(32, (3,3), activation='relu', input_shape=(28,28,1)),
    layers.MaxPooling2D((2,2)),
    layers.Conv2D(64, (3,3), activation='relu'),
    layers.MaxPooling2D((2,2)),
    layers.Flatten(),
    layers.Dense(128, activation='relu'),
    layers.Dense(10, activation='softmax')
])

model.compile(optimizer='adam',
              loss='sparse_categorical_crossentropy',
              metrics=['accuracy'])

# -------------------------
# 3. Train the network
# -------------------------
model.fit(x_train, y_train, epochs=5, batch_size=64,
          validation_split=0.1)

# Evaluate accuracy
test_loss, test_acc = model.evaluate(x_test, y_test)
print(f"Test accuracy: {test_acc*100:.2f}%")

# -------------------------
# 4. Save the model as TensorFlow SavedModel (.pb)
# -------------------------
save_dir = "mnist_model_tf"
if not os.path.exists(save_dir):
    os.makedirs(save_dir)

model.save(save_dir, save_format='tf')
print(f"Saved TensorFlow model to folder: {save_dir}")
