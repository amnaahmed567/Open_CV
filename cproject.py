import cv2

# Load the pre-trained Haar Cascade classifier for face detection
face_cascade = cv2.CascadeClassifier(cv2.data.haarcascades + 'haarcascade_frontalface_default.xml')

# Capture video from the default camera (usually the webcam)
cap = cv2.VideoCapture(0)

while True:
    # Read each frame from the camera
    ret, frame = cap.read()
    if not ret:
        break

    # Convert the frame to grayscale (Haar Cascade works on grayscale images)
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    # Detect faces
    faces = face_cascade.detectMultiScale(gray, scaleFactor=1.1, minNeighbors=5, minSize=(30, 30))

    # Draw rectangles around the detected faces
    for (x, y, w, h) in faces:
        cv2.rectangle(frame, (x, y), (x + w, y + h), (255, 0, 0), 2)

    # Display the result
    cv2.imshow('Face Detection', frame)

    # Break the loop on 'ESC' key press
    if cv2.waitKey(10) & 0xFF == 27:  # 27 is the ASCII code for the ESC key
        break

# Release the camera and close all windows
cap.release()
cv2.destroyAllWindows()
