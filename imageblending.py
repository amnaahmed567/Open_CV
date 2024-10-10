import cv2
import numpy as np

img1=cv2.imread("pasta.jpg")
img1=cv2.resize(img1,(500,700))
img2=cv2.imread("northenlights.jpg")
img2=cv2.resize(img2,(500,700))
cv2.imshow("PASTAAA",img1)
cv2.imshow("NL",img2)
result=img1+img2
cv2.imshow("result",result)
result1=cv2.add(img1,img2)
cv2.imshow("result1",result1)


result2=cv2.addWeighted(img1,0.3,img2,0.7,0)
cv2.imshow("thisispastawithnorthenlights",result2)

cv2.waitKey(0)
cv2.destroyAllWindows()