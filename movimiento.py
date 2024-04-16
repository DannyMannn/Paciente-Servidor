import os
import cv2
import requests
import numpy as np
import time

def compare_images(img1, img2):
    # Calcula la diferencia y aplica un umbral.
    difference = cv2.absdiff(img1, img2)
    _, thresh = cv2.threshold(cv2.cvtColor(difference, cv2.COLOR_BGR2GRAY), 25, 255, cv2.THRESH_BINARY)
    return np.sum(thresh) > 0

# URL del stream de vídeo desde la cámara IP (Cambiar a la url que de)
url = 'http://192.168.100.76:8080/video'

# Carpeta para guardar capturas de pantalla (Especificar la ruta)
folder = r'C:\Users\Usuario\Pictures\Screenshots'
if not os.path.exists(folder):
    os.makedirs(folder)

# Intervalo para capturar pantallas(segundos)
# Puede llegar a ser más si no se desea almacenar muchas capturas
interval = 5

# Frame previo para comparación
prev_frame = None

# Comienza la captura de pantallas
while True:
    try:
        # Solicita el stream de vídeo
        response = requests.get(url, stream=True)

        # Verifica si la respuesta fue exitosa
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

                            if prev_frame is not None and new_frame is not None:
                                if compare_images(prev_frame, new_frame):
                                    print("Cambio detectado.")

                            prev_frame = new_frame.copy()

                            # Muestra el frame (opcional)
                            cv2.imshow('Video Stream', new_frame)

                            # Guarda el screenshot en la carpeta
                            timestamp = int(time.time())
                            filename = os.path.join(folder, f'imagen_{timestamp}.jpg')
                            cv2.imwrite(filename, new_frame)
                    else:
                        continue

        # Espera el intervalo especificado
        time.sleep(interval)

        # Comprueba si se presionó 'q' para salir
        if cv2.waitKey(1) == ord('q'):
            break

    except requests.exceptions.RequestException as e:
        print(f"Request Exception: {e}")
        continue

    except KeyboardInterrupt:
        break

# Cierra todas las ventanas abiertas
cv2.destroyAllWindows()
