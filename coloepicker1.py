import cv2
import numpy as np

# Create a black image
img = np.zeros((300, 500, 3), np.uint8)
cv2.namedWindow("Color Picker")

def nothing(x):
    pass

# Create trackbars for color change
cv2.createTrackbar("Switch", "Color Picker", 0, 1, nothing)
cv2.createTrackbar("R", "Color Picker", 0, 255, nothing)
cv2.createTrackbar("G", "Color Picker", 0, 255, nothing)
cv2.createTrackbar("B", "Color Picker", 0, 255, nothing)

while True:
    # Show the image
    cv2.imshow("Color Picker", img)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

    # Get the current positions of the trackbars
    s = cv2.getTrackbarPos("Switch", "Color Picker")
    r = cv2.getTrackbarPos("R", "Color Picker")
    g = cv2.getTrackbarPos("G", "Color Picker")
    b = cv2.getTrackbarPos("B", "Color Picker")

    if s == 0:
        img[:] = 0
    else:
        img[:] = [b, g, r]  

cv2.destroyAllWindows()
