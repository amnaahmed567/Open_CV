import cv2
import numpy as np

ColorBalls=cv2.imread("ColorBalls.jpg",0)
_,mask=cv2.threshold(ColorBalls,230,255,cv2.THRESH_BINARY_INV)
kernel=np.ones((5,5),np.uint8)
er=cv2.erode(mask,kernel)
dr=cv2.dilate(mask,kernel)
O=cv2.morphologyEx(mask,cv2.MORPH_OPEN,kernel)
C=cv2.morphologyEx(mask,cv2.MORPH_CLOSE,kernel)
BH=cv2.morphologyEx(mask,cv2.MORPH_BLACKHAT,kernel)
cv2.imshow("color",ColorBalls)
cv2.imshow("mask",mask)
cv2.imshow("kernel",kernel)
cv2.imshow("erosion",er)
cv2.imshow("dilation",dr)
cv2.imshow("opening",O)
cv2.imshow("closing",C)
cv2.imshow("BlackHat",BH)


cv2.waitKey(0)
cv2.destroyAllWindows