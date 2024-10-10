import cv2
#opening video from your laptop

cap = cv2.VideoCapture("hi.MOV")

while True:
    ret, frame = cap.read()
    frame=cv2.resize(frame,(300,500))
    gray=cv2.cvtColor(frame,cv2.COLOR_BGR2GRAY)
    cv2.imshow("frame", frame)
    cv2.imshow("gray",gray)
    if cv2.waitKey(100) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()


#opening webcam
cap = cv2.VideoCapture(0)

while cap.isOpened():
    ret, frame = cap.read()
    if ret==True:
        frame=cv2.resize(frame,(300,500))
        frame=cv2.flip(frame,0)
        gray=cv2.cvtColor(frame,cv2.COLOR_BGR2GRAY)
        cv2.imshow("frame", frame)
        cv2.imshow("gray",gray)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

cap.release()
cv2.destroyAllWindows()

#saving the video generated from your webcam

cap=cv2.VideoCapture(0)
fourcc=cv2.VideoWriter_fourcc(*"XVID")
output=cv2.VideoWriter('output1.avi',fourcc,20,(300,500),0)
while cap.isOpened():
    ret,fram=cap.read()
    if ret==True:
        #frame=cv2.resize(frame,(300,500))
        gray=cv2.cvtColor(frame,cv2.COLOR_BGR2GRAY)
        #cv2.imshow("frame", frame)
        cv2.imshow("gray",gray)
        output.write(gray)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break