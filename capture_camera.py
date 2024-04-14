import os
import cv2
import requests
import numpy as np
import time

# URL of the video stream from IP Webcam
# url = 'http://192.168.0.101:8080/video'
url = 'http://192.168.100.49:8080/video'

# Folder to save screenshots
folder = 'screenshots'

# Create the folder if it doesn't exist
if not os.path.exists(folder):
    os.makedirs(folder)

# Capture screenshots every 3 seconds
interval = 5  # seconds

# Start capturing screenshots
while True:
    try:
        # Request the video stream
        response = requests.get(url, stream=True)

        # Check if the response was successful
        if response.status_code == 200:
            # Read the streaming bytes
            bytes_ = bytes()
            for chunk in response.iter_content(chunk_size=1024):
                bytes_ += chunk
                a = bytes_.find(b'\xff\xd8')
                b = bytes_.find(b'\xff\xd9')
                if a != -1 and b != -1:
                    jpg = bytes_[a:b + 2]
                    bytes_ = bytes_[b + 2:]
                    try:
                        # Decode the frame
                        frame = cv2.imdecode(np.frombuffer(jpg, dtype=np.uint8), cv2.IMREAD_COLOR)
                        new_frame = cv2.resize(frame, (800, 456), interpolation=cv2.INTER_AREA)

                        # Check if the frame was successfully captured
                        if frame is not None:
                            # Display the frame (optional)
                            cv2.imshow('Video Stream', frame)

                            # Save the screenshot inside the folder
                            timestamp = int(time.time())
                            
                            filename = 'screenshots/imagen.jpg'
                            cv2.imwrite(filename, new_frame)
                    except cv2.error as e:
                        # Handle the OpenCV error
                        print(f"OpenCV Error: {e}")
                        continue

        # Wait for the specified interval
        time.sleep(interval)

        # Check for 'q' key press to exit
        if cv2.waitKey(1) == ord('q'):
            break

    except requests.exceptions.RequestException as e:
        # Handle the request exception
        print(f"Request Exception: {e}")
        continue

    except KeyboardInterrupt:
        break

# Close any open windows
cv2.destroyAllWindows()
