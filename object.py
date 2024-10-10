import cv2

img = cv2.imread('murree.jpg', 1)

height, width, channel = img.shape

max_width = 800
max_height = 600

scaling_factor = min(max_width / width, max_height / height)

new_width = int(width * scaling_factor)
new_height = int(height * scaling_factor)

resized_img = cv2.resize(img, (new_width, new_height))

#print(resized_img.shape)
#print(resized_img.size)
#print(resized_img.dtype)

build=resized_img[258:192,234:382]
resized_img[345:234,345:234]=build #wrong

cv2.imshow('image',resized_img)
cv2.waitKey(0)
cv2.destroyAllWindows()