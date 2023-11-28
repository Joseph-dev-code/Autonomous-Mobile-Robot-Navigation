import os
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'
import tensorflow as tf

import socket
import cv2
import numpy as np
from tensorflow import keras
import struct

IP = "192.168.1.4"  # Listen on all available network interfaces
PORT = 5544
SIZE = 1024

# Load pre-trained classifier
model = keras.models.load_model('/home/laboras/Downloads/imageclassifier.h5')
# Global variable for the server socket
server_fd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

def receive_image():

    # Configure the server address
    server_addr = (IP, PORT)
    server_fd.bind(server_addr)
    server_fd.listen(5)

    print(f"[LISTENING] Port Number: {PORT}")

    while True:
        client_fd, client_addr = server_fd.accept()
        print("[CONNECTED] New Connection")

        image_data = b''
        received_size = 0



        while True:
            data = client_fd.recv(SIZE)
            if not data:
                break
            image_data += data
            received_size += len(data)

        if received_size > 0:
            with open("received_image.jpg", "wb") as image_file:
                image_file.write(image_data)
            print(f"[CLIENT] Received {received_size} bytes and saved as 'received_image.jpg'.")

        #client_fd.close()
        #print("[DISCONNECTED] Connection closed")

        return image_data, client_fd, client_addr

def Image_processing(image_data):
    # Load and preprocess the received image
    #image = cv2.imread("/home/laboras/VS_workspace/received_image.jpg")
    image = cv2.imdecode(np.frombuffer(image_data, dtype=np.uint8), cv2.IMREAD_COLOR)
    if image is not None:
        print("[PROCESSING] Image loaded successfully")
        image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
        image = cv2.resize(image, (256, 256)) / 255.0  # Resize and normalize
        yhat = model.predict(np.expand_dims(image, axis=0))

        # Make a prediction
        prediction = 0 if yhat > 0.5 else 1
        print(f'Prediction: {prediction}')

        
        return prediction
    

def send_prediction(prediction, client_fd, client_addr):
    client_fd, client_addr = server_fd.accept()
    print("[2nd Server CONNECTED] New Connection")
        # Send the prediction back to the client
    try:
        #prediction_as_integer = int(prediction)
        #prediction_bytes = struct.pack('>d', prediction)  # 'd' represents double-precision floating-point format
        #prediction_bytes = prediction.to_bytes(4, byteorder='big')  # Assuming a 32-bit integer
        prediction_bytes = struct.pack('>f', prediction)
        client_fd.send(prediction_bytes)
        print(f"[SENT] Prediction {prediction} successfully sent.")
    except Exception as e:
        print(f"[ERROR] Failed to send prediction: {e}")  

    client_fd.close()
    print("[DISCONNEDTED] Connection closed")

if __name__ == "__main__":
    image_data, client_fd, client_addr = receive_image()
    prediction = Image_processing(image_data)
    send_prediction(prediction, client_fd, client_addr)


