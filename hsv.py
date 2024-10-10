import cv2
import numpy as np

northernLights=cv2.imread("green.png")
northernLights = cv2.resize(northernLights, (500, 700))

while True:
    hsv=cv2.cvtColor(northernLights,cv2.COLOR_BGR2HSV)
    upper=np.array([35, 100, 100])
    lower=np.array([85, 255, 255])
    mask=cv2.inRange(hsv,upper,lower)

    res=cv2.bitwise_and(northernLights,northernLights,mask=mask)

    cv2.imshow("frame",northernLights)
    cv2.imshow("mask",mask)
    cv2.imshow("res",res)

    k = cv2.waitKey(1) & 0xFF
    if k == 27:
        break

# Destroy all windows
cv2.destroyAllWindows()
