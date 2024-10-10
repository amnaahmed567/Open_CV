import cv2
import numpy as np

# Load and resize images
img1 = cv2.imread("pasta.jpg")
img1 = cv2.resize(img1, (500, 700))
img2 = cv2.imread("northenlights.jpg")
img2 = cv2.resize(img2, (500, 700))

# Display the images
cv2.imshow("PASTAAA", img1)
cv2.imshow("NL", img2)

# Blend function for trackbar (no action needed)
def blend(x):
    pass

# Create a black image and a window with trackbars
img = np.zeros((400, 400, 3), np.uint8)
cv2.namedWindow("Trackbar")
cv2.createTrackbar("new", "Trackbar", 0, 100, blend)
switch = '0 : OFF \n 1 : ON'
cv2.createTrackbar(switch, "Trackbar", 0, 1, blend)

while True:
    # Get positions of the trackbars
    s = cv2.getTrackbarPos(switch, 'Trackbar')
    a = cv2.getTrackbarPos('new', 'Trackbar')
    n = float(a / 100)

    if s == 0:
        dst = img[:]
    else:
        dst = cv2.addWeighted(img1, 1 - n, img2, n, 0)
        cv2.putText(dst, str(a), (20, 50), cv2.FONT_HERSHEY_COMPLEX, 2, (0, 125, 255), 2)
        cv2.imshow('dst', dst)

    # Break the loop if 'ESC' key is pressed
    k = cv2.waitKey(1) & 0xFF
    if k == 27:
        break

# Destroy all windows
cv2.destroyAllWindows()
