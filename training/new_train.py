from core import *
from rivers import LoadData, PreprocessData

def VisualizeResults(index):
    img = X_valid[index]
    img = img[np.newaxis, ...]
    pred_y = unet.predict(img)
    pred_mask = tf.argmax(pred_y[0], axis=-1)
    pred_mask = pred_mask[..., tf.newaxis]
    fig, arr = plt.subplots(1, 3, figsize=(15, 15))
    arr[0].imshow(X_valid[index])
    arr[0].set_title("Processed Image")
    arr[1].imshow(y_valid[index, :, :, 0])
    arr[1].set_title("Actual Masked Image ")
    arr[2].imshow(pred_mask[:, :, 0])
    arr[2].set_title("Predicted Masked Image ")
    plt.show()


if __name__ == "__main__":
    """Load Train Set and view some examples"""
    image_paths = LoadData()

    # Define the desired shape
    target_shape_img = [256, 256, 3]
    target_shape_mask = [256, 256, 1]

    # Process data using apt helper function
    X, y = PreprocessData(image_paths, target_shape_img, target_shape_mask)

    X_train, X_valid, y_train, y_valid = train_test_split(
        X, y, test_size=0.01, random_state=123
    )


    unet = UNetCompiled(input_size=(256, 256, 3), n_filters=32, n_classes=3)

    # unet.summary()

    unet.compile(
        optimizer=tf.keras.optimizers.Adam(),
        loss=tf.keras.losses.SparseCategoricalCrossentropy(from_logits=True),
        metrics=["accuracy"],
    )

    checkpoint_path = "weights/heathcoat.weights.h5"
    # checkpoint_dir = os.path.dirname(checkpoint_path)
    # unet.load_weights(checkpoint_path)

    # Training check point create 
    cp_callback = tf.keras.callbacks.ModelCheckpoint(
        filepath=checkpoint_path, save_weights_only=True, verbose=1
    )

    results = unet.fit(
        X_train,
        y_train,
        batch_size=10,
        epochs=30,
        validation_data=(X_valid, y_valid),
        callbacks=[cp_callback],
    )

    unet.save("models.keras")
    unet.export("savemodel")

    fig, axis = plt.subplots(1, 2, figsize=(20, 5))
    axis[0].plot(results.history["loss"], color="r", label="train loss")
    axis[0].plot(results.history["val_loss"], color="b", label="val loss")
    axis[0].set_title("Loss Comparison")
    axis[0].legend()
    axis[1].plot(results.history["accuracy"], color="r", label="train accuracy")
    axis[1].plot(results.history["val_accuracy"], color="b", label="val accuracy")
    axis[1].set_title("Accuracy Comparison")
    axis[1].legend()
    plt.show()

    unet.evaluate(X_valid, y_valid)

    index = 0
    for i in range(len(X_train)):
        VisualizeResults(i)
