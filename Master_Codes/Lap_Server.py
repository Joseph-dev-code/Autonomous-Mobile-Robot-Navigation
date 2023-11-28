#Working code to send an text ver1.0

import socket

PORT = 4455

def main():
    # Variables and structures
    server_fd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_addr = ('192.168.1.4', PORT)

    # Server socket
    server_fd.bind(server_addr)
    server_fd.listen(5)
    print(f"[LISTENING] Port Number: {PORT}")

    while True:
        client_fd, client_addr = server_fd.accept()
        print("[CONNECTED] New Connection")

        buffer = "Hello, This is a test message"
        client_fd.send(buffer.encode())

        buffer = client_fd.recv(1024).decode()
        print(f"[CLIENT] {buffer}")

        client_fd.close()
        print("[DISCONNECTED] Connection closed")

    server_fd.close()

if __name__ == "__main__":
    main()

#####################      ver1.1    #############
#######code to receive an image from robot######## Working #######

import socket

IP = "192.168.1.4"  # Listen on all available network interfaces
PORT = 4455
SIZE = 1024

def receive_image():
    # Create a socket
    server_fd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

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

        client_fd.close()
        print("[DISCONNECTED] Connection closed")

if __name__ == "__main__":
    receive_image()

###################################################
##make the above code to receive image and do image processing-classification#####

#step1 receive image
#step2 do image classification on the received image



import os
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3' 
import tensorflow as tf
# Above lines will remove the warnings from terminals, only your program output is printed
# 0 = all messages are logged (default behavior)
# 1 = INFO messages are not printed
# 2 = INFO and WARNING messages are not printed
# 3 = INFO, WARNING, and ERROR messages are not printed


import os
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'
import tensorflow as tf
import socket
import cv2
import numpy as np
from tensorflow import keras

IP = "192.168.1.4"  # Listen on all available network interfaces
PORT = 4455
SIZE = 1024

# Load the pre-trained classifier from an HDF5 file
model = keras.models.load_model('/home/laboras/Downloads/imageclassifier.h5')

def receive_and_process_image():
    # Create a socket
    server_fd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

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

            # Load and preprocess the image
            image = cv2.imread("received_image.jpg")
            if image is not None:
                image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
                image = cv2.resize(image, (256, 256))  # Resize to match the input size of your classifier
                image = image / 255.0  # Normalize the image

                # Make a prediction with the classifier
                yhat = model.predict(np.expand_dims(image, axis=0))

                if yhat > 0.5:
                    print('0')
                else:
                    print('1')
            else:
                print("Failed to load the image.")

        client_fd.close()
        print("[DISCONNECTED] Connection closed")

if __name__ == "__main__":
    receive_and_process_image()








########################################################################################################
#*****Working code to receive the image and process it and show the output 0(empty) and 1(left is detected)

import os
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'
import tensorflow as tf
import socket
import cv2
import numpy as np
from tensorflow import keras

IP = "192.168.1.4"  # Listen on all available network interfaces
PORT = 4455
SIZE = 1024

# Load the pre-trained classifier from an HDF5 file
model = keras.models.load_model('/home/laboras/Downloads/imageclassifier.h5')

def receive_and_process_image():
    # Create a socket
    server_fd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

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

            # Load and preprocess the image
            image = cv2.imread("received_image.jpg")
            if image is not None:
                image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
                image = cv2.resize(image, (256, 256))  # Resize to match the input size of your classifier
                image = image / 255.0  # Normalize the image

                # Make a prediction with the classifier
                yhat = model.predict(np.expand_dims(image, axis=0))

                if yhat > 0.5:
                    print('0')
                else:
                    print('1')
            else:
                print("Failed to load the image.")

        client_fd.close()
        print("[DISCONNECTED] Connection closed")

if __name__ == "__main__":
    receive_and_process_image()
#########################################################################################################
# NOT-WORKING CODE - RECEIVE IMAGE PROCESS IT AND GET THE VALUE 0 OR 1
import os
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'
import tensorflow as tf
import socket
import cv2
import numpy as np
from tensorflow import keras

IP = "192.168.1.4"
PORT = 4455
SIZE = 1024

model = keras.models.load_model('/home/laboras/Downloads/imageclassifier.h5')

def receive_and_process_image():
    server_fd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
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

            image = cv2.imread("received_image.jpg")
            if image is not None:
                image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
                image = cv2.resize(image, (256, 256))
                image = image / 255.0

                yhat = model.predict(np.expand_dims(image, axis=0))

                result = '0' if yhat > 0.5 else '1'
                print(result)

                client_fd.send(result.encode())  # Send the result back to the client

            else:
                print("Failed to load the image.")

        client_fd.close()
        print("[DISCONNECTED] Connection closed")

if __name__ == "__main__":
    receive_and_process_image()

############################################################################



import os
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'
import tensorflow as tf
import socket
import cv2
import numpy as np
from tensorflow import keras

IP = "192.168.1.4"
PORT = 4455
SIZE = 1024

model = keras.models.load_model('/home/laboras/Downloads/imageclassifier.h5')

def receive_and_process_image():
    server_fd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_addr = (IP, PORT)
    server_fd.bind(server_addr)
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

            image = cv2.imread("/home/laboras/VS_workspace/received_image.jpg")
            if image is not None:
                print("[PROCESSING] Image loaded successfully")

                image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
                image = cv2.resize(image, (256, 256))
                image = image / 255.0

                yhat = model.predict(np.expand_dims(image, axis=0))

                result = '0' if yhat > 0.5 else '1'
                print(f"[RESULT] Predicted result: {result}")

                client_fd.send(result.encode())  # Send the result back to the client
                print("[CLIENT] Sent the result back")

            else:
                print("Failed to load the image.")

        client_fd.close()
        print("[DISCONNECTED] Connection closed")

if __name__ == "__main__":
    receive_and_process_image()
#############################################################################
## NOT WORKING CODE BUT CHANGES CANGES CAN BE MADE
import os
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'
import tensorflow as tf

import os

import socket
import cv2
import numpy as np
from tensorflow import keras


# Server configuration
IP = "192.168.1.4"
PORT = 4455
SIZE = 1024

# Load pre-trained classifier
model = keras.models.load_model('/home/laboras/Downloads/imageclassifier.h5')

# Create a socket
server_fd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_addr = (IP, PORT)
server_fd.bind(server_addr)
server_fd.listen(5)
print(f"[LISTENING] Port Number: {PORT}")

while True:
    # Accept client connection
    client_fd, client_addr = server_fd.accept()
    print("[CONNECTED] New Connection")




    # Receive image data
    print("Trying to receive image")  # Print a message indicating the intention to receive an image
    image_data = b''  # Initialize an empty byte string to store the received image data
    received_size = 0  # Initialize a variable to keep track of the received data size

    # Continuously receive data until there's no more data
    while True:
        try:
            image_data = b''  # Clear the buffer before each iteration
            received_size = 0  # Reset the total received data size

            data = client_fd.recv(SIZE)  # Receive data from the client in chunks of SIZE
            print("Received data chunk:", data)  # Print the received data chunk
            if not data:  # Check if there is no more data
                print("No more data. Exiting the loop.")  # Print a message indicating no more data
                break  # Exit the loop if there's no more data
            image_data += data  # Append the received data to the image data
            received_size += len(data)  # Update the total received data size
            print("Total received size:", received_size)  # Print the total received data size
        except socket.timeout:
            break  # Break the loop if a timeout occurs

    # Process received image
    if received_size > 0:
        with open("received_image.jpg", "wb") as image_file:
            image_file.write(image_data)
        print(f"[CLIENT] Received {received_size} bytes and saved as 'received_image.jpg'.")

        # Load and preprocess the image
        image = cv2.imread("/home/laboras/VS_workspace/received_image.jpg")
        if image is not None:
            print("[PROCESSING] Image loaded successfully")
            image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
            image = cv2.resize(image, (256, 256)) / 255.0  # Resize and normalize
            yhat = model.predict(np.expand_dims(image, axis=0))

            # Make a prediction
            prediction = 0 if yhat > 0.5 else 1
            print(f'Prediction: {prediction}')

            # Send the prediction back to the client
            client_fd.send(str(prediction).encode())
        else:
            print("Failed to load the image.")
            client_fd.send(b"Error")

    # Close client connection
    client_fd.close()
    print("[DISCONNECTED] Connection closed")
#########################################################################
#NOT WORKING CODE - CHANGES ARE NOT POSSIBLE.
import os
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'
import tensorflow as tf
import socket
import cv2
import numpy as np
from tensorflow import keras
import struct 

# Server configuration
IP = "192.168.1.4"
PORT = 4455
SIZE = 1024

# Load pre-trained classifier
model = keras.models.load_model('/home/laboras/Downloads/imageclassifier.h5')

# Create a socket
server_fd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_addr = (IP, PORT)
server_fd.bind(server_addr)
server_fd.listen(5)
print(f"[LISTENING] Port Number: {PORT}")

client_fd, client_addr = server_fd.accept()
print("[CONNECTED] New Connection")



try:
        # Step 1: Receive the size of the image
    size_data = client_fd.recv(4)
    image_size = struct.unpack("!I", size_data)[0]
    print(f"Received image size: {image_size} long")

        # Step 2: Send a signal that it received the size and is waiting for the image
    #client_fd.send(b"Ready for Image")

        # Step 3: Start receiving the image
    received_data = b""
    while len(received_data) < image_size:
        data = client_fd.recv(1024)
        if not data:
            break
        received_data += data

    # Step 4: Compare the size of the received image with the expected size
    if len(received_data) == image_size:
        print("Image received successfully")

            # Step 5: Perform further processing here

    #else:
        #print("Error: Received image size does not match the expected size")


except Exception as e:
    print(f"An error occurred: {e}")


'''
# Receive image data
print("Trying to receive image")

image_data = b''  # Initialize an empty byte string to store the received image data
received_size = 0  # Initialize a variable to keep track of the received data size

# Continuously receive data until there's no more data
while True:
    try:
        data = client_fd.recv(SIZE)  # Receive data from the client in chunks of SIZE
        print("Received data chunk:", data)  # Print the received data chunk
        if not data:  # Check if there is no more data
            print("No more data. Exiting the loop.")  # Print a message indicating no more data
            break  # Exit the loop if there's no more data
        if data == b'EOF':
            print("Received end-of-file signal. Exiting the loop.")
            break
        image_data += data  # Append the received data to the image data
        received_size += len(data)  # Update the total received data size
        print("Total received size:", received_size)  # Print the total received data size
    except socket.timeout:
        break  # Break the loop if a timeout occurs
'''

'''while True:
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
        print(f"[CLIENT] Received {received_size} bytes and saved as 'received_image.jpg'.")'''


    # Load and preprocess the received image
image = cv2.imread("/home/laboras/VS_workspace/received_image.jpg")
if image is not None:
        print("[PROCESSING] Image loaded successfully")
        image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
        image = cv2.resize(image, (256, 256)) / 255.0  # Resize and normalize
        yhat = model.predict(np.expand_dims(image, axis=0))

        # Make a prediction
        prediction = 0 if yhat > 0.5 else 1
        print(f'Prediction: {prediction}')

        # Send the prediction back to the client
        client_fd.send(str(prediction).encode())
else:
        print("Failed to load the image.")
        client_fd.send(b"Error")

    # Close client connection
client_fd.close()
print("[DISCONNECTED] Connection closed")

####################################################
#********  LAST WORKING SOURCE CODE **********************#####
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
        #prediction_bytes = prediction.to_bytes(4, byteorder='big')  # Assuming a 32-bit integer
        client_fd.send(prediction)
        print(f"[SENT] Prediction {prediction} successfully sent.")
    except Exception as e:
        print(f"[ERROR] Failed to send prediction: {e}")  

    client_fd.close()
    print("[DISCONNEDTED] Connection closed")

if __name__ == "__main__":
    image_data, client_fd, client_addr = receive_image()
    prediction = Image_processing(image_data)
    send_prediction(prediction, client_fd, client_addr)





