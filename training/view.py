from core import *
from PIL import Image
import tensorflow as tf
import keras
from tensorflow.keras.saving import load_model
import matplotlib.pyplot as plt
from tkinter import filedialog

model = tf.keras.saving.load_model("/home/FWALMSL/personal/Navigator/training/models.keras")

# Need the get the dimensions from the saved model
height   = model.layers[0].batch_shape[1]
width    = model.layers[0].batch_shape[2]
channels = model.layers[0].batch_shape[3]
print(model.layers[0].batch_shape)

while (1):
    # image_path = "C:\\Users\\fletc\\Pictures\\CNN\\comb_exp_mask\\e5648316-39f9-11ef-b6c2-00155d3bf750\\img.png"
    image_path = filedialog.askopenfilename()
    img = Image.open(image_path).convert("RGB")
    img = img.resize((height, width))
    img = np.reshape(img, (1, height, width, channels))
    img = img / 256.0

    pred = model.predict(img)
    pred_mask = tf.argmax(pred[0], axis=-1)
    pred_mask = pred_mask[..., tf.newaxis]

    fig, arr = plt.subplots(1, 2, figsize=(15, 15))
    arr[0].imshow(img[0])
    arr[0].set_title("Processed Image")
    arr[1].imshow(pred_mask[:, :, 0])
    arr[1].set_title("Predicted Masked Image ")
    plt.show()