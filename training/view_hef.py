import tensorflow as tf
import numpy as np
from tensorflow.python.framework.convert_to_constants import convert_variables_to_constants_v2
from tensorflow.keras.saving import load_model
from hailo_sdk_client import ClientRunner, InferenceContext
from rivers import LoadData, PreprocessData
from sklearn.model_selection import train_test_split
import matplotlib.pyplot as plt


# Initialize the Hailo runner
runner = ClientRunner(hw_arch="hailo8")


# Load the HEF file from disk
file_name = "river_segment.hef"
with open(file_name, "rb") as f:
    hef_data = f.read()
    
# Load the HEF into the runner
# Using load_model_from_hef instead of load_hef
runner.load_model_from_hef(hef_data)


# Prepare data for validation
image_paths = LoadData()
target_shape_img = [64, 64, 3]
target_shape_mask = [64, 64, 1]
X, y = PreprocessData(image_paths, target_shape_img, target_shape_mask)
X_train, X_valid, y_train, y_valid = train_test_split(
    X, y, test_size=0.7, random_state=345
)


# Model validation
with runner.infer_context(InferenceContext.SDK_NATIVE) as ctx:
  native_res = runner.infer(ctx, X_train)
  results = [np.squeeze(result) for result in native_res]
  i=0
  for result in results:
      pred_mask = tf.argmax(result, axis=-1)
      pred_mask = pred_mask[..., tf.newaxis]
      fig, arr = plt.subplots(1, 2, figsize=(15, 15))
      arr[0].imshow(X_train[i])
      arr[0].set_title("Processed Image")
      arr[1].imshow(pred_mask[:, :, 0])
      arr[1].set_title("Predicted Masked Image ")
      i=i+1
      plt.show()

