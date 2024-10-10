import numpy as np
import cv2

# Read and resize the image
img = cv2.imread('images.png')  
img = cv2.resize(img, (200, 200))

# Define the region of interest (ROI)
roi = img[10:70, 90:180]  # Shape is (60, 90, 3)
img[71:131, 90:180] = roi  
cv2.imshow('image1', img)
cv2.waitKey(0)
cv2.destroyAllWindows()
