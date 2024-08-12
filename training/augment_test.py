from rivers import LoadData, PreprocessData
from tensorflow.keras.preprocessing.image import ImageDataGenerator, load_img, img_to_array
import numpy as np

import matplotlib.pyplot as plt

image_paths = LoadData()

target_shape_img = [128, 128, 3]
target_shape_mask = [128, 128, 1]

# Create an instance of ImageDataGenerator for augmentation
data_gen_args = dict(
        rotation_range=40,
        height_shift_range=0.2,
        width_shift_range=0.2,
        shear_range=0.2,
        zoom_range=0.2,
        horizontal_flip=True,
        fill_mode='nearest'
)

image_datagen = ImageDataGenerator(**data_gen_args)
mask_datagen = ImageDataGenerator(**data_gen_args)

# Load the image and mask
x_img, x_mask = PreprocessData(image_paths, target_shape_img, target_shape_mask)

# Expand dimensions to create batch of size 1
x_img = np.expand_dims(x_img[20], axis=0)
x_mask = np.expand_dims(x_mask[20], axis=0)

# Provide the same seed and keyword arguments to the flow methods
seed = 1
image_gen = image_datagen.flow(x_img, batch_size=1, seed=seed)
mask_gen = mask_datagen.flow(x_mask, batch_size=1, seed=seed)

# Combine generators into one which yields image and masks
train_generator = zip(image_gen, mask_gen)

# Generate augmented images and masks
num_augmented_images = 5  # Number of augmented images to generate

# Visualize original image and mask
plt.figure(figsize=(10, 5))
plt.subplot(2, num_augmented_images + 1, 1)
plt.title('Original Image')
plt.imshow(x_img[0])
plt.axis('off')

plt.subplot(2, num_augmented_images + 1, num_augmented_images + 2)
plt.title('Original Mask')
plt.imshow(x_mask[0].astype(np.uint8)[:,:,0], cmap='gray')
plt.axis('off')

# Generate augmented images and masks
for i, (img_batch, mask_batch) in enumerate(train_generator):
    if i >= num_augmented_images:
        break
    # Display augmented image
    plt.subplot(2, num_augmented_images + 1, i + 2)
    plt.title(f'Augmented Image {i + 1}')
    plt.imshow(img_batch[0])
    plt.axis('off')
    
    # Display corresponding augmented mask
    plt.subplot(2, num_augmented_images + 1, num_augmented_images + i + 3)
    plt.title(f'Augmented Mask {i + 1}')
    plt.imshow(mask_batch[0].astype(np.uint8)[:,:,0], cmap='gray')
    plt.axis('off')

plt.tight_layout()
plt.show()
