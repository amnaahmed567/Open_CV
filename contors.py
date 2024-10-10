import cv2
import numpy as np

img=cv2.imread("OpenCVLogo.jpg")
img1=cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)

ret,thres=cv2.threshold(img1,20,255,0)
cnts,hier=cv2.findContours(img1,cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)

print(cnts,len(cnts))
img=cv2.drawContours(img,cnts,-1,(25,100,15),4)

cv2.imshow("original",img)
cv2.imshow("gray",img1)
cv2.imshow("threshold",thres)

cv2.waitKey(0)
cv2.destroyAllWindows()