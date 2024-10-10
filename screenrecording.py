import cv2
import pyautogui as p
import numpy as np

# Get screen size
rs = p.size()

# Input file name
fn = input("Enter the file path: ")
fps = 20
fourcc = cv2.VideoWriter_fourcc(*"mp4v")
output = cv2.VideoWriter(fn, fourcc, fps, rs)


cv2.namedWindow("window", cv2.WINDOW_NORMAL)
cv2.resizeWindow("window", 600, 400)

while True:
    # Take screenshot
    img = p.screenshot()
    
    array = np.array(img)

    array = cv2.cvtColor(array, cv2.COLOR_RGB2BGR)
    
    # Write to the video file
    output.write(array)
    cv2.imshow("window", array)
    if cv2.waitKey(1) == ord('q'):
        break

cv2.destroyAllWindows()
output.release()
