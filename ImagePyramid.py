import cv2
import numpy as np
benten=cv2.imread("benten.jpg",)
cv2.imshow("benten",benten)

pd1=cv2.pyrDown(benten)
pd2=cv2.pyrDown(pd1)
pd3=cv2.pyrDown(pd2)

pu1=cv2.pyrUp(benten)
pu2=cv2.pyrUp(pu1)
pu3=cv2.pyrUp(pu2)
cv2.imshow("1",pd1)
cv2.imshow("2",pd2)
cv2.imshow("3",pd3)
cv2.imshow("4",pu1)
cv2.imshow("5",pu2)
cv2.imshow("5",pu3)

cv2.waitKey(0)
cv2.destroyAllWindows()