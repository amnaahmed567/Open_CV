import cv2
import numpy as np

benten=cv2.imread("benten.jpg",0)
cv2.imshow("BEN10",benten)
#laplace
lap=cv2.Laplacian(benten,cv2.CV_64F,ksize=1)
lap=np.uint8(np.absolute(lap))
#Sobel
SobelX=cv2.Sobel(benten,cv2.CV_64F,1,0,ksize=3)
SobelY=cv2.Sobel(benten,cv2.CV_64F,0,1,ksize=3)
SobelX=np.uint8(np.absolute(SobelX))
SobelY=np.uint8(np.absolute(SobelY))
SobelCombine=cv2.bitwise_or(SobelX,SobelY)
cv2.imshow("laplace",lap)
cv2.imshow("SobelX",SobelX)
cv2.imshow("SobelY",SobelY)
cv2.imshow("SobelCombine",SobelCombine)
cv2.waitKey(0)
cv2.destroyAllWindows()