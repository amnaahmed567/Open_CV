import cv2
import datetime


cap = cv2.VideoCapture(0)
#cap.set(3,1340)
#cap.set(4,569)

fourcc = cv2.VideoWriter_fourcc(*'XVID')
out = cv2.VideoWriter('output.avi', fourcc, 20.0, (640, 480))

print(cap.isOpened())
while cap.isOpened():
    ret, frame = cap.read()
    if ret:
        font=cv2.FONT_HERSHEY_COMPLEX
        text='width'+str(cap.get(3)) + ' height' + str(cap.get(4))
        date=str(datetime.datetime.now())
        frame=cv2.putText(frame,date,(10,50),font,1,(0,255,0),2,cv2.LINE_AA)

        #gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

        
        out.write(frame)

        
        cv2.imshow('frame', frame)

        
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
    else:
        break


cap.release()
out.release()
cv2.destroyAllWindows()
