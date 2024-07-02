'''
Holds functions related to training the river model

This is the main model of this project
'''

# for data load
import os
import numpy as np
from PIL import Image

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



def PreprocessData(image_paths, target_shape_img, target_shape_mask):
    """
    Processes the images and mask present in the shared list and path
    Returns a NumPy dataset with images as 3-D arrays of desired size
    Please note the masks in this dataset have only one channel
    """
    # Pull the relevant dimensions for image and mask
    m = len(image_paths)  # number of images
    i_h, i_w, i_c = target_shape_img  # pull height, width, and channels of image
    m_h, m_w, m_c = target_shape_mask  # pull height, width, and channels of mask

    # Define X and Y as number of images along with shape of one image
    X = np.zeros((m, i_h, i_w, i_c), dtype=np.float32)
    Y = np.zeros((m, m_h, m_w, m_c), dtype=np.int32)

    # Resize images and masks
    for i, (img, mask) in enumerate(image_paths):
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
    return X, Y