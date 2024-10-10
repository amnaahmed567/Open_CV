import cv2
import numpy as np

# Create a black image
img = np.zeros((512, 512, 3), np.uint8)
#img=img=cv2.imread('murree.jpg',1)
# Define the mouse callback function
def click_event(event, x, y, flags, params):
    if event == cv2.EVENT_LBUTTONDOWN:
        print(x, '', y)
        strXY = str(x) + ',' + str(y)
        font = cv2.FONT_HERSHEY_COMPLEX
        cv2.putText(img, strXY, (x, y), font, 0.5, (255, 255, 0), 2)
        cv2.imshow('image', img)
    if event == cv2.EVENT_RBUTTONDOWN:
        blue = img[y, x, 0]
        green = img[y, x, 1]
        red = img[y, x, 2]
        font = cv2.FONT_HERSHEY_COMPLEX
        strBGR = str(blue) + ',' + str(green) + ',' + str(red)
        cv2.putText(img, strBGR, (x, y), font, 0.5, (0, 255, 255), 2)
        cv2.imshow('image', img)

cv2.namedWindow('image')
cv2.setMouseCallback('image', click_event)
cv2.imshow('image', img)
cv2.waitKey(0)
cv2.destroyAllWindows()
