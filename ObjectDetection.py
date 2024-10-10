import cv2
import numpy as np

# Initialize the camera
cap = cv2.VideoCapture(0, cv2.CAP_DSHOW)

def nothing(x):
    pass

# Create window for color adjustments
window_name = "Color Adjustments"
cv2.namedWindow(window_name, cv2.WINDOW_NORMAL)
cv2.resizeWindow(window_name, (300, 300)) 

# Create trackbars for adjusting HSV range and threshold
cv2.createTrackbar("Thresh", window_name, 0, 255, nothing)
cv2.createTrackbar("Lower_H", window_name, 0, 255, nothing)
cv2.createTrackbar("Lower_S", window_name, 0, 255, nothing)
cv2.createTrackbar("Lower_V", window_name, 0, 255, nothing)
cv2.createTrackbar("Upper_H", window_name, 255, 255, nothing)
cv2.createTrackbar("Upper_S", window_name, 255, 255, nothing)
cv2.createTrackbar("Upper_V", window_name, 255, 255, nothing)

while True:
    ret, frame = cap.read()
    if not ret:
        break

    frame = cv2.resize(frame, (400, 400))
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

    # Get trackbar positions
    l_h = cv2.getTrackbarPos("Lower_H", window_name)
    l_s = cv2.getTrackbarPos("Lower_S", window_name)
    l_v = cv2.getTrackbarPos("Lower_V", window_name)
    u_h = cv2.getTrackbarPos("Upper_H", window_name)
    u_s = cv2.getTrackbarPos("Upper_S", window_name)
    u_v = cv2.getTrackbarPos("Upper_V", window_name)
    arg = cv2.getTrackbarPos("Thresh", window_name)
    
    lower_bound = np.array([l_h, l_s, l_v])
    upper_bound = np.array([u_h, u_s, u_v])
    
    # Create mask and filter
    mask = cv2.inRange(hsv, lower_bound, upper_bound)
    filtr = cv2.bitwise_and(frame, frame, mask=mask)

    mask1 = cv2.bitwise_not(mask)
    ret, thres = cv2.threshold(mask1, arg, 255, cv2.THRESH_BINARY)
    dilate = cv2.dilate(thres, (1, 1), iterations=6)

    cnts, hier = cv2.findContours(dilate, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    for c in cnts:
        epsilon = 0.0001*cv2.arcLength(c,True)
        data= cv2.approxPolyDP(c,epsilon,True)
    
        hull = cv2.convexHull(data)
        cv2.drawContours(frame, [c], -1, (50, 50, 150), 2)
        cv2.drawContours(frame, [hull], -1, (0, 255, 0), 2)

    # Display the result
    cv2.imshow("Thresh", thres)
    cv2.imshow("Mask", mask)
    cv2.imshow("Filter", filtr)
    cv2.imshow("Result", frame)

    key = cv2.waitKey(25) & 0xFF
    if key == 27:  # Press 'ESC' to exit
        break

cap.release()
cv2.destroyAllWindows()
