import cv2
import numpy as np

benten=cv2.imread("benten.jpg",0)

cv2.imshow("benten",benten)

def nothing(x):
    pass
cv2.namedWindow("Canny")
cv2.createTrackbar("Threshold","Canny",0,255,nothing)

while True:
    a=cv2.getTrackbarPos("Threshold","Canny")
    print(a)
    res=cv2.Canny(benten,a,255)
    cv2.imshow("Canny",res)
    k=cv2.waitKey(1) & 0xFF
    if k==27:
        break
cv2.destroyAllWindows()