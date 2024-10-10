import cv2
import numpy as np

img=np.zeros((300,500,3),np.uint8)
cv2.namedWindow("Color Picker")
def none(x):
    pass
s="0:OFF\n1:ON"
cv2.createTrackbar(s,"Color Picker",0,1,none)

cv2.createTrackbar("R","Color Picker",0,255,none)
cv2.createTrackbar("G","Color Picker",0,255,none)
cv2.createTrackbar("B","Color Picker",0,255,none)

while True:
    cv2.imshow("Color Picker",img)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

s1=cv2.getTrackbarPos(s,"Color Picker")
r=cv2.getTrackbarPos("R","Color Picker")
g=cv2.getTrackbarPos("G","Color Picker")
b=cv2.getTrackbarPos("B","Color Picker")

if s1==0:
    img[:]=0
else:
    img[:]=[g,b,r]

cv2.destroyAllWindows()
