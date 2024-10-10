import cv2
import numpy as np
img=cv2.imread('murree.jpg',1)
def click_event(event,x,y,flags,params):
    if event== cv2.EVENT_LBUTTONDBLCLK:
        blue=img[x,y,0]
        green=img[x,y,1]
        red=img[x,y,2]
        cv2.circle(img,(x,y),3,(255,0,0),-1)
        mycolorimage=np.zeros((512,512,3),np.uint8)
        mycolorimage[:]=[blue,green,red]
        cv2.imshow('color',mycolorimage)

cv2.namedWindow('image')
cv2.setMouseCallback('image', click_event)
cv2.imshow('image', img)
cv2.waitKey(0)
cv2.destroyAllWindows()
