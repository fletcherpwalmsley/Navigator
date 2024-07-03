'''
Holds functions related to training the river model

This is the main model of this project
'''

# for data load
import os
import numpy as np
from PIL import Image
from tensorflow.keras.preprocessing.image import ImageDataGenerator

def LoadData():
    """
    Looks for relevant filenames in the shared path
    Returns 2 lists for original and masked files respectively

    """

    path = "C:\\Users\\fletc\\Pictures\\CNN\\rivers\\converted_mask"
    folder_list = os.listdir(path)

    images = []
    for folder in folder_list:
        append_tuple = os.path.join(path, folder, 'img.png'), os.path.join(path, folder, 'label.png')
        images.append(append_tuple)
    return images



data_gen_args = dict(
        rotation_range=40,
        height_shift_range=0.2,
        width_shift_range=0.2,
        shear_range=0.2,
        zoom_range=0.2,
        horizontal_flip=True,
        fill_mode='nearest'
)

def Augment(img, mask, num_of_images):
    image_datagen = ImageDataGenerator(**data_gen_args)
    mask_datagen = ImageDataGenerator(**data_gen_args)

    # Expand dimensions to create batch of size 1
    img = np.expand_dims(img, axis=0)
    mask = np.expand_dims(mask, axis=0)

    seed = 1
    image_datagen.fit(img, augment=True, seed=seed)
    mask_datagen.fit(mask, augment=True, seed=seed)

    # Combine generators into one which yields image and masks
    train_generator = zip(image_datagen.flow(img, batch_size=1, seed=seed),
                        mask_datagen.flow(mask, batch_size=1, seed=seed))
    

    # Collect augmented images and masks in lists
    augmented_images = []
    augmented_masks = []

    # Generate augmented images and masks
    for i, (img_batch, mask_batch) in enumerate(train_generator):
        augmented_images.append(img_batch[0])
        augmented_masks.append(mask_batch[0])
        if i >= num_of_images - 1:
            break
    
    augmented_images_np = np.array(augmented_images)
    augmented_masks_np = np.array(augmented_masks)

    return augmented_images_np, augmented_masks_np
    

def PreprocessData(image_paths, target_shape_img, target_shape_mask):
    """
    Processes the images and mask present in the shared list and path
    Returns a NumPy dataset with images as 3-D arrays of desired size
    Please note the masks in this dataset have only one channel
    """
    # Pull the relevant dimensions for image and mask
    m = len(image_paths)  # number of images
    print(m)
    aug = 3 # Number of augmented images to generate from one source image 
    m = m + (aug * m)
    print(m)
    i_h, i_w, i_c = target_shape_img  # pull height, width, and channels of image
    m_h, m_w, m_c = target_shape_mask  # pull height, width, and channels of mask

    # Define X and Y as number of images along with shape of one image
    X = np.zeros((m, i_h, i_w, i_c), dtype=np.float32)
    Y = np.zeros((m, m_h, m_w, m_c), dtype=np.int32)

    # Resize images and masks
    i = 0
    for (img, mask) in image_paths:
        # convert image into an array of desired shape (3 channels)
        single_img = Image.open(img).convert("RGB")
        single_img = single_img.resize((i_h, i_w))
        single_img = np.reshape(single_img, (i_h, i_w, i_c))
        single_img = single_img / 256.0
        X[i] = single_img

        # convert mask into an array of desired shape (1 channel)
        single_mask = Image.open(mask)
        single_mask = single_mask.resize((m_h, m_w))
        single_mask = np.reshape(single_mask, (m_h, m_w, m_c))
        # single_mask = single_mask - 1  # to ensure classes #s start from 0
        Y[i] = single_mask
        i = i + 1

        # Create Augmented images
        aug_x, aug_y = Augment(single_img, single_mask, aug)
        X[i:i+aug] = aug_x
        Y[i:i+aug] = aug_y
        i = i + aug
    print(i)

    return X, Y