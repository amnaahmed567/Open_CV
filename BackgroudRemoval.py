import cv2
import numpy as np

# Load and convert the original image to HSV
original_image = cv2.imread("imageman.jpg")
original_image = cv2.resize(original_image, (600, 650))
hsv_original = cv2.cvtColor(original_image, cv2.COLOR_BGR2HSV)

# Load and convert the ROI image to HSV
roi = cv2.imread("green.jpg")
hsv_roi = cv2.cvtColor(roi, cv2.COLOR_BGR2HSV)

# Calculate the histogram for the ROI
roi_hist = cv2.calcHist([hsv_roi], [0, 1], None, [180, 256], [0, 180, 0, 256])

# Backprojection to create the mask
mask = cv2.calcBackProject([hsv_original], [0, 1], roi_hist, [0, 180, 0, 256], 1)

# Apply a convolution with an elliptical kernel to smooth the mask
kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (5, 5))
mask = cv2.filter2D(mask, -1, kernel)

# Apply a threshold to get a binary mask
_, mask = cv2.threshold(mask, 200, 255, cv2.THRESH_BINARY)

# Merge the mask into three channels
mask = cv2.merge((mask, mask, mask))

# Combine the original image with the mask using bitwise OR
result = cv2.bitwise_or(original_image, mask)

# Display the results
cv2.imshow("Mask", mask)
cv2.imshow("Original Image", original_image)
cv2.imshow("Result", result)
cv2.imshow("ROI", hsv_original)

cv2.waitKey(0)
cv2.destroyAllWindows()
