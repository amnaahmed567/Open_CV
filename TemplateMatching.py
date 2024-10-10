import cv2
import numpy as np

img = cv2.imread("avengers.jpg")

grey_img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
template = cv2.imread("temp.jpg", 0)
w, h = template.shape[::-1]

#this function accept prameters (img,template,method)
res = cv2.matchTemplate(grey_img, template, cv2.TM_CCORR_NORMED)
print("res==",res)
threshold = 0.999
loc = np.where(res >= threshold)  #find brightest point
print("bright pixels==",loc)
count = 0
for i in zip(*loc[::-1]):
    print("i===",i)
    cv2.rectangle(img, i, (i[0] + w, i[1] + h), (0, 0, 255), 2)
    count+=1
print("number of iterations==",count)
img = cv2.resize(img,(800,600))
res = cv2.resize(res,(800,600))
cv2.imshow("img", img)
cv2.imshow("match temp==",res)
cv2.waitKey(0)
cv2.destroyAllWindows