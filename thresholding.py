import cv2
import numpy as np

# Load and resize the image
img = cv2.imread("black_white.jpg", cv2.IMREAD_GRAYSCALE)
img = cv2.resize(img, (300, 300))

# Display the original image
cv2.imshow("Original", img)

# Load the second image
img1 = cv2.imread("phool.png", cv2.IMREAD_GRAYSCALE)

# Apply different thresholding techniques
_, ths1 = cv2.threshold(img, 50, 255, cv2.THRESH_BINARY)
_, ths2 = cv2.threshold(img, 50, 255, cv2.THRESH_BINARY_INV)
_, ths3 = cv2.threshold(img, 127, 255, cv2.THRESH_TRUNC)
_, ths4 = cv2.threshold(img, 127, 255, cv2.THRESH_TOZERO)
_, ths5 = cv2.threshold(img, 127, 255, cv2.THRESH_TOZERO_INV)
ad = cv2.adaptiveThreshold(img1, 255, cv2.ADAPTIVE_THRESH_MEAN_C, cv2.THRESH_BINARY, 11, 2)
ad2 = cv2.adaptiveThreshold(img1, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C, cv2.THRESH_BINARY, 11, 2)
# Display the thresholded images
cv2.imshow("Threshold Binary", ths1)
cv2.imshow("Threshold Binary Inverse", ths2)
cv2.imshow("Threshold Truncate", ths3)
cv2.imshow("Threshold ToZero", ths4)
cv2.imshow("Threshold ToZero Inverse", ths5)
cv2.imshow("Adaptive Threshold_mean", ad)
cv2.imshow("Adaptive Threshold_gaussian", ad2)

# Wait for a key press indefinitely or for a specified amount of time
cv2.waitKey(0)

# Close all OpenCV windows
cv2.destroyAllWindows()
