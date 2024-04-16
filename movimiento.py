import os
import cv2
import requests
import numpy as np
import time
from skimage.metrics import structural_similarity as compare_ssim

def compare_images(img1, img2):
    gray_img1 = cv2.cvtColor(img1, cv2.COLOR_BGR2GRAY)
    gray_img2 = cv2.cvtColor(img2, cv2.COLOR_BGR2GRAY)
    score, _ = compare_ssim(gray_img1, gray_img2, full=True)
    return score < 0.95  # Si el score es menor a 0.95, consideramos que hay un cambio

# La url se debe cambiar segun la que de la app
url = 'http://192.168.100.76:8080/video'

# La ubicación del folder puede ser asignada a la que se guste almacenar los screens
folder = r'C:\Users\Usuario\Documents\Arduino\ESP32\Screenshots'
if not os.path.exists(folder):
    os.makedirs(folder)

interval = 5
prev_frame = None

while True:
    try:
        response = requests.get(url, stream=True)

        if response.status_code == 200:
            bytes_ = bytes()
            for chunk in response.iter_content(chunk_size=1024):
                bytes_ += chunk
                a = bytes_.find(b'\xff\xd8')
                b = bytes_.find(b'\xff\xd9')
                if a != -1 and b != -1:
                    jpg = bytes_[a:b + 2]
                    bytes_ = bytes_[b + 2:]
                    if len(jpg) > 0:
                        frame = cv2.imdecode(np.frombuffer(jpg, dtype=np.uint8), cv2.IMREAD_COLOR)
                        if frame is not None:
                            new_frame = cv2.resize(frame, (800, 456), interpolation=cv2.INTER_AREA)

                            if prev_frame is not None:
                                if compare_images(prev_frame, new_frame):
                                    print("Cambio detectado.")

                            prev_frame = new_frame.copy()

                            cv2.imshow('Video Stream', new_frame)
                            cv2.waitKeyEx(1)  # Espera y captura eventos de la ventana

                            timestamp = int(time.time())
                            filename = os.path.join(folder, f'imagen_{timestamp}.jpg')
                            cv2.imwrite(filename, new_frame)
                    else:
                        continue

        time.sleep(interval)

    except requests.exceptions.RequestException as e:
        print(f"Request Exception: {e}")
        continue

    except KeyboardInterrupt:
        break

cv2.destroyAllWindows()
