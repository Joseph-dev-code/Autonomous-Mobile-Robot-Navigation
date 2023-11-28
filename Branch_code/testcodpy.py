import socket
import cv2
import numpy as np
from tensorflow import keras
import struct

IP = "192.168.1.4"  # Listen on all available network interfaces
PORT = 5544
SIZE = 1024

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

if __name__ == "__main__":
    image_data, client_fd, client_addr = receive_image()
