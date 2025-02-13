import tensorflow as tf
import numpy as np
from tensorflow.python.framework.convert_to_constants import convert_variables_to_constants_v2
from tensorflow.keras.saving import load_model
from hailo_sdk_client import ClientRunner, InferenceContext
from rivers import LoadData, PreprocessData
from sklearn.model_selection import train_test_split
import matplotlib.pyplot as plt

converter = tf.lite.TFLiteConverter.from_saved_model("savemodel")
# converter.optimizations = [tf.lite.Optimize.DEFAULT]
converter.target_spec.supported_ops = [
  tf.lite.OpsSet.TFLITE_BUILTINS, # enable TensorFlow Lite ops.
  tf.lite.OpsSet.SELECT_TF_OPS # enable TensorFlow ops.
]
tflite_model = converter.convert()

# Save the model.
with open("model.tflite", "wb") as f:
  f.write(tflite_model)

# Parsing the model to Hailo format
runner = ClientRunner(hw_arch="hailo8")
hn, npz = runner.translate_tf_model("model.tflite", "river_segment")


# Quantization of the Hailo model
image_paths = LoadData()
target_shape_img = [64, 64, 3]
target_shape_mask = [64, 64, 1]
X, y = PreprocessData(image_paths, target_shape_img, target_shape_mask)
X_train, X_valid, y_train, y_valid = train_test_split(
    X, y, test_size=0.7, random_state=345
)

# alls = "normalization1 = normalization([123.675, 116.28, 103.53], [58.395, 57.12, 57.375])\n"

# # Load the model script to ClientRunner so it will be considered on optimization
# runner.load_model_script(alls)

runner.optimize(X_train)
quantized_model_har_path = f"river_segment_quantized_model.har"
runner.save_har(quantized_model_har_path)


hef = runner.compile()
file_name = f"river_segment.hef"
with open(file_name, "wb") as f:
    f.write(hef)

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

      