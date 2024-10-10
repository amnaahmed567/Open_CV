import cv2
import numpy as np
#drawing a line

height, width,channel = img.shape
max_width = 800
max_height = 600
scaling_factor = min(max_width / width, max_height / height)
new_width = int(width * scaling_factor)
new_height = int(height * scaling_factor)
resized_img = cv2.resize(img, (new_width, new_height))
resized_img=cv2.line(resized_img,(0,0),(255,255),(245, 66, 169),5)
resized_img=cv2.arrowedLine(resized_img,(0,60),(255,255),(255,0,0),5)
resized_img=cv2.rectangle(resized_img,(345,245),(908,765),(0,255,0),2)
resized_img=cv2.circle(resized_img,(34,56),78,(255,0,0),-1)
font=cv2.FONT_HERSHEY_COMPLEX
resized_img=cv2.putText(resized_img,'hellllo',(34,56),font,4, (255,0,0),3,cv2.LINE_4)
cv2.imshow('MURREE1',resized_img)
cv2.waitKey(0)
cv2.destroyAllWindows()