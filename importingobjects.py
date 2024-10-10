import cv2

# Load the image
img = cv2.imread('murree.jpg', 1)

# Get image dimensions
height, width, channel = img.shape

# Set maximum dimensions
max_width = 800
max_height = 600

# Calculate scaling factor
scaling_factor = min(max_width / width, max_height / height)

# Calculate new dimensions
new_width = int(width * scaling_factor)
new_height = int(height * scaling_factor)

# Resize the image
resized_img = cv2.resize(img, (new_width, new_height))

# Display the resized image in a window
cv2.imshow('MURRREEEE', resized_img)

# Define the mouse callback function
def click_event(event, x, y, flags, params):
    if event == cv2.EVENT_LBUTTONDOWN:
        print(x, '', y)
        strXY = str(x) + ',' + str(y)
        font = cv2.FONT_HERSHEY_COMPLEX
        cv2.putText(resized_img, strXY, (x, y), font, 0.5, (255, 255, 0), 2)
        cv2.imshow('MURRREEEE', resized_img)
    if event == cv2.EVENT_RBUTTONDOWN:
        blue = resized_img[y, x, 0]
        green = resized_img[y, x, 1]
        red = resized_img[y, x, 2]
        font = cv2.FONT_HERSHEY_COMPLEX
        strBGR = str(blue) + ',' + str(green) + ',' + str(red)
        cv2.putText(resized_img, strBGR, (x, y), font, 0.5, (0, 255, 255), 2)
        cv2.imshow('MURRREEEE', resized_img)

# Create a window named 'MURRREEEE'
cv2.namedWindow('MURRREEEE')

# Set the mouse callback function to the window
cv2.setMouseCallback('MURRREEEE', click_event)

# Wait for a key press indefinitely or for a delay (in milliseconds)
cv2.waitKey(0)

# Destroy all windows
cv2.destroyAllWindows()
