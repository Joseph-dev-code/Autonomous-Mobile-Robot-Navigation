////////////// ver 1.0 ///////////
// Working code to receive an text

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define IP "192.168.1.4"
#define PORT 4455
#define SIZE 1024

int main() {
    int client_fd;
    struct sockaddr_in server_addr;
    char buffer[SIZE];

    // Create a socket
    client_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Configure the server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(IP);

    // Connect to the server
    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        return 1;
    }


    // Receive data from the server
    memset(buffer, 0, SIZE);
    if (recv(client_fd, buffer, SIZE, 0) < 0) {
        perror("Receive failed");
        return 1;
    }
    printf("[SERVER] %s\n", buffer);

    // Send data to the server
    strcat(buffer, " From CLIENT");
    if (send(client_fd, buffer, strlen(buffer), 0) < 0) {
        perror("Send failed");
        return 1;
    }

    // Close the connection
    close(client_fd);

    return 0;
}

///////////////////  ver1.1   /////////////////////
/// code to send an image from robot to laptop /// Working



#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

#define IP "192.168.1.4"
#define PORT 4455
#define SIZE 1024

int main() {
    int client_fd;
    struct sockaddr_in server_addr;
    char buffer[SIZE];

    // Create a socket
    client_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Configure the server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(IP);

    // Connect to the server
    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        return 1;
    }

    // Open and read the image file from the root directory
    FILE *image_file = fopen("/home/root/image.jpg", "rb");
    if (image_file == NULL) {
        perror("Failed to open image file");
        return 1;
    }

    // Send the image file to the server
    while (1) {
        ssize_t bytes_read = fread(buffer, 1, SIZE, image_file);
        if (bytes_read <= 0) {
            break;  // End of file
        }
        if (send(client_fd, buffer, bytes_read, 0) < 0) {
            perror("Send failed");
            return 1;
        }
    }

    // Close the image file
    fclose(image_file);

    // Close the connection
    close(client_fd);

    return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//Program to take a picture with sensor and send it over tcp ip protocol
//This program should allow us to do the following 
//1. take a picture/photo using integrated camera of robot 

 Write a function to take picture and store it in "image.jpg"
 
//2. save it in a "image.jpg" in /home/root/image.jpg
	check whether the "image.jpg" file is empty or not
	if empty retake the image
		again "check"
	else read the image

//3. read that image.jpg
//4. initialize to connect to the robot










#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

#define IP "192.168.1.4"
#define PORT 4455
#define SIZE 1024


static int quitReq = 0; //'quitReq' is a global variable.


int captureImages() {
    // Capture images once outside the loop
    int ret = system("media-pipes.sh");
    if (ret != 0) {
        perror("Failed to run media-pipes.sh");
        return -3;
    }

    ret = system("media-formats.sh 752 480");
    if (ret != 0) {
        perror("Failed to run media-formats.sh");
        return -4;
    }

    while (!quitReq) {
            if (quitReq) {  // to exit the function after it is executed only once
            break;  // Exit the loop if quitReq is set
        }
        printf("Taking images...\n");

        ret = system("v4l2grab -d /dev/video6 -o image.jpg -W 752 -H 480 -q 85 -I -1");
        if (ret != 0) {
            perror("Failed to capture an image with v4l2grab");
            return -5;
        }

        printf("Image captured and saved as 'image.jpg'.\n");
    }

    // Return a success code if needed.
    return 0;
}







int main() {
    int client_fd;
    struct sockaddr_in server_addr;
    char buffer[SIZE];
    
    
    
    //Funstion to capture image
    int result = captureImages();
    if (result != 0) {
        // Handle errors or take appropriate action based on the return code.
    }
    
    
    

    // Create a socket
    client_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Configure the server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(IP);

    // Connect to the server
    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        return 1;
    }

    // Open and read the image file from the root directory
    FILE *image_file = fopen("/home/root/image.jpg", "rb");
    if (image_file == NULL) {
        perror("Failed to open image file");
        return 1;
    }

    // Send the image file to the server
    while (1) {
        ssize_t bytes_read = fread(buffer, 1, SIZE, image_file);
        if (bytes_read <= 0) {
            break;  // End of file
        }
        if (send(client_fd, buffer, bytes_read, 0) < 0) {
            perror("Send failed");
            return 1;
        }
    }

    // Close the image file
    fclose(image_file);

    // Close the connection
    close(client_fd);

    return 0;
}




//The above code is taking pictures with out any interuptions because of while loop
//to stop it below code shoes that while loop is removed and the capture image
//function executes only once.
////////////////////////////////////////////////////////////////////////////////////////

// *******CAPTURE IMAGE AND SEND IT OVER TCP IP PROTOCOL***WORKING CODE****
//--------------VER 2.0---------------------



#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

#define IP "192.168.1.4"
#define PORT 4455
#define SIZE 1024


static int quitReq = 0; //'quitReq' is a global variable.


int captureImages() {
    // Capture images once outside the loop
    int ret = system("media-pipes.sh");
    if (ret != 0) {
        perror("Failed to run media-pipes.sh");
        return -3;
    }

    ret = system("media-formats.sh 752 480");
    if (ret != 0) {
        perror("Failed to run media-formats.sh");
        return -4;
    }

          printf("Taking images...\n");

        ret = system("v4l2grab -d /dev/video6 -o image.jpg -W 752 -H 480 -q 85 -I -1");
        if (ret != 0) {
            perror("Failed to capture an image with v4l2grab");
            return -5;
        }

        printf("Image captured and saved as 'image.jpg'.\n");
    

    // Return a success code if needed.
    return 0;
}

int main() {
    int client_fd;
    struct sockaddr_in server_addr;
    char buffer[SIZE];
    
    
    
    //Function to capture image
    int result = captureImages();
    if (result != 0) {
        // Handle errors or take appropriate action based on the return code.
    }
    
    
    

    // Create a socket
    client_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Configure the server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(IP);

    // Connect to the server
    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        return 1;
    }

    // Open and read the image file from the root directory
    FILE *image_file = fopen("/home/root/image.jpg", "rb");
    if (image_file == NULL) {
        perror("Failed to open image file");
        return 1;
    }

    // Send the image file to the server
    while (1) {
        ssize_t bytes_read = fread(buffer, 1, SIZE, image_file);
        if (bytes_read <= 0) {
            break;  // End of file
        }
        if (send(client_fd, buffer, bytes_read, 0) < 0) {
            perror("Send failed");
            return 1;
        }
    }

    // Close the image file
    fclose(image_file);

    // Close the connection
    close(client_fd);

    return 0;
}

-------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

#define IP "192.168.1.4"
#define PORT 4455
#define SIZE 1024

static knet_dev_t *dsPic; // robot pic microcontroller access
static int quitReq = 0; //'quitReq' is a global variable.

void turnLeft90Degrees() {
    int duration = 4000000;  // Adjust the duration as needed (in microseconds)
    int speedLeft = -20;   // Negative speed for turning left
    int speedRight = 40;   // Positive speed for the other wheel to turn left
    kh4_SetMode(kh4RegSpeed, dsPic);
    kh4_set_speed(speedLeft, speedRight, dsPic);
    usleep(duration);
    kh4_set_speed(0, 0, dsPic); // Stop the robot
}

int captureImages() {
    // Capture images once outside the loop
    int ret = system("media-pipes.sh");
    if (ret != 0) {
        perror("Failed to run media-pipes.sh");
        return -3;
    }

    ret = system("media-formats.sh 752 480");
    if (ret != 0) {
        perror("Failed to run media-formats.sh");
        return -4;
    }

          printf("Taking images...\n");

        ret = system("v4l2grab -d /dev/video6 -o image.jpg -W 752 -H 480 -q 85 -I -1");
        if (ret != 0) {
            perror("Failed to capture an image with v4l2grab");
            return -5;
        }

        printf("Image captured and saved as 'image.jpg'.\n");
    

    // Return a success code if needed.
    return 0;
}

int main() {
    int client_fd;
    struct sockaddr_in server_addr;
    char buffer[SIZE];

   // Initiate libkhepera and robot access
   
    if (kh4_init(0, NULL) != 0) {
        printf("\nERROR: could not initiate the libkhepera!\n\n");
        return -1;
    }

    /* Open robot socket and store the handle in its pointer */
    
    dsPic = knet_open("Khepera4:dsPic", KNET_BUS_I2C, 0, NULL);

    if (dsPic == NULL) {
        printf("\nERROR: could not initiate communication with Kh4 dsPic\n\n");
        return -2;
    }


    //Funstion to capture image
    int result = captureImages();
    if (result != 0) {
        // Handle errors or take appropriate action based on the return code.
    }
    
    
        // Create a socket
    client_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Configure the server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(IP);

    // Connect to the server
    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        return 1;
    }

    // Open and read the image file from the root directory
    FILE *image_file = fopen("/home/root/image.jpg", "rb");
    if (image_file == NULL) {
        perror("Failed to open image file");
        return 1;
    }

    // Send the image file to the server
    while (1) {
        ssize_t bytes_read = fread(buffer, 1, SIZE, image_file);
        if (bytes_read <= 0) {
            break;  // End of file
        }
        if (send(client_fd, buffer, bytes_read, 0) < 0) {
            perror("Send failed");
            return 1;
        }
    }
    
    
    
	char buffer[SIZE];
	memset(buffer, 0, sizeof(buffer));
	int n = read(sockfd, buffer, SIZE);
	if (n < 0) {
   	 perror("Failed to receive result");
    		return -4;
	} 
	else {
    		turnLeft90Degrees();
		}

    
    
        // Call the turnLeft90Degrees() function if the result is 1
    //if (atoi(buffer) == 1) {
    //    turnLeft90Degrees();
   // }

    return 0;
    
    

    // Close the image file
    fclose(image_file);
    
    
    

    // Close the robot
    knet_close(dsPic);

    return 0;
}


/////////////////////////////////////////////////////////////////////////////////
////**************start from here******************

int ret = system("media-pipes.sh");
if (ret != 0) {
    perror("Failed to run media-pipes.sh");
    return -3;
}

printf("media-pipes.sh completed successfully.\n");

ret = system("media-formats.sh 752 480");
if (ret != 0) {
    perror("Failed to run media-formats.sh");
    return -4;
}

printf("media-formats.sh completed successfully.\n");

printf("Taking images...\n");

ret = system("v4l2grab -d /dev/video6 -o image.jpg -W 752 -H 480 -q 85 -I -1");
if (ret != 0) {
    perror("Failed to capture an image with v4l2grab");
    return -5;
}

printf("Image captured and saved as 'image.jpg'.\n");




//main code is getting stuck at media pipeline trying to execute it in on eby one line

//THis code is getting stuck at media pipeline using above printf in each and every line trying to solve the problem by knowing where the pipeline is getting stuck.
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

#define IP "192.168.1.4"
#define PORT 4455
#define SIZE 1024

static knet_dev_t *dsPic; // robot pic microcontroller access
static int quitReq = 0; //'quitReq' is a global variable.

void turnLeft90Degrees() {
    int duration = 4000000;  // Adjust the duration as needed (in microseconds)
    int speedLeft = -20;   // Negative speed for turning left
    int speedRight = 40;   // Positive speed for the other wheel to turn left
    kh4_SetMode(kh4RegSpeed, dsPic);
    kh4_set_speed(speedLeft, speedRight, dsPic);
    usleep(duration);
    kh4_set_speed(0, 0, dsPic); // Stop the robot
}

int captureImages() {
    // Capture images once outside the loop
    int ret = system("media-pipes.sh");
    if (ret != 0) {
        perror("Failed to run media-pipes.sh");
        return -3;
    }

    ret = system("media-formats.sh 752 480");
    if (ret != 0) {
        perror("Failed to run media-formats.sh");
        return -4;
    }

          printf("Taking images...\n");

        ret = system("v4l2grab -d /dev/video6 -o image.jpg -W 752 -H 480 -q 85 -I -1");
        if (ret != 0) {
            perror("Failed to capture an image with v4l2grab");
            return -5;
        }

        printf("Image captured and saved as 'image.jpg'.\n");
    

    // Return a success code if needed.
    return 0;
}

int main() {
    int client_fd;
    struct sockaddr_in server_addr;
    char buffer[SIZE];

   // Initiate libkhepera and robot access
   
    if (kh4_init(0, NULL) != 0) {
        printf("\nERROR: could not initiate the libkhepera!\n\n");
        return -1;
    }

    /* Open robot socket and store the handle in its pointer */
    
    dsPic = knet_open("Khepera4:dsPic", KNET_BUS_I2C, 0, NULL);

    if (dsPic == NULL) {
        printf("\nERROR: could not initiate communication with Kh4 dsPic\n\n");
        return -2;
    }


    //Funstion to capture image
    int result = captureImages();
    if (result != 0) {
        // Handle errors or take appropriate action based on the return code.
    }
    
    
        // Create a socket
    client_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Configure the server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(IP);

    // Connect to the server
    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        return 1;
    }

    // Open and read the image file from the root directory
    FILE *image_file = fopen("/home/root/image.jpg", "rb");
    if (image_file == NULL) {
        perror("Failed to open image file");
        return 1;
    }

    // Send the image file to the server
    while (1) {
        ssize_t bytes_read = fread(buffer, 1, SIZE, image_file);
        if (bytes_read <= 0) {
            break;  // End of file
        }
        if (send(client_fd, buffer, bytes_read, 0) < 0) {
            perror("Send failed");
            return 1;
        }
    }
    
    
    
	char buffer[SIZE];
	memset(buffer, 0, sizeof(buffer));
	int n = read(sockfd, buffer, SIZE);
	if (n < 0) {
   	 perror("Failed to receive result");
    		return -4;
	} 
	else {
    		turnLeft90Degrees();
		}

    
    
        // Call the turnLeft90Degrees() function if the result is 1
    //if (atoi(buffer) == 1) {
    //    turnLeft90Degrees();
   // }

    return 0;
    
    

    // Close the image file
    fclose(image_file);
    
    
    

    // Close the robot
    knet_close(dsPic);

    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
last not working code back up - this code is stuck at taking pictures line-just back up

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <khepera/khepera.h>

#define IP "192.168.1.4"
#define PORT 4455
#define SIZE 1024

static knet_dev_t *dsPic;

void turnLeft90Degrees() {
    int duration = 4000000;
    int speedLeft = -20;
    int speedRight = 40;
    kh4_SetMode(kh4RegSpeed, dsPic);
    kh4_set_speed(speedLeft, speedRight, dsPic);
    usleep(duration);
    kh4_set_speed(0, 0, dsPic);
}

int captureImagesAndSend(int client_fd) {
    int ret = system("media-pipes.sh");
    if (ret != 0) {
        perror("Failed to run media-pipes.sh");
        return -3;
    }

    printf("media-pipes.sh completed successfully.\n");

    ret = system("media-formats.sh 752 480");
    if (ret != 0) {
        perror("Failed to run media-formats.sh");
        return -4;
    }

    printf("media-formats.sh completed successfully.\n");

    printf("Taking images...\n");

    FILE *v4l2grab = popen("v4l2grab -d /dev/video6 -o image.jpg -W 752 -H 480 -q 85 -I -1", "r");
    if (v4l2grab == NULL) {
        perror("Failed to execute v4l2grab");
        return -5;
    }

    FILE *image_file = fopen("/home/root/image.jpg", "wb");
    if (image_file == NULL) {
        perror("Failed to open image file");
        pclose(v4l2grab);
        return -6;
    }

    char buffer[SIZE];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, SIZE, v4l2grab)) > 0) {
        if (fwrite(buffer, 1, bytes_read, image_file) < 0) {
            perror("Write to image file failed");
            fclose(image_file);
            pclose(v4l2grab);
            return -7;
        }

        if (send(client_fd, buffer, bytes_read, 0) < 0) {
            perror("Send failed");
            fclose(image_file);
            pclose(v4l2grab);
            return -8;
        }
    }

    fclose(image_file);
    pclose(v4l2grab);

    return 0;
}





int main() {
    int client_fd;
    struct sockaddr_in server_addr;
    int value;

    if (kh4_init(0, NULL) != 0) {
        printf("\nERROR: could not initiate the libkhepera!\n\n");
        return -1;
    }

    dsPic = knet_open("Khepera4:dsPic", KNET_BUS_I2C, 0, NULL);

    if (dsPic == NULL) {
        printf("\nERROR: could not initiate communication with Kh4 dsPic\n\n");
        return -2;
    }

    client_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (client_fd < 0) {
        perror("Failed to create socket");
        return -9;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(IP);

    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(client_fd);
        return -10;
    }

    int captureResult = captureImagesAndSend(client_fd);
    if (captureResult != 0) {
        // Handle errors or take appropriate action based on the return code.
        close(client_fd);
        return captureResult;
    }

    // Receive a value from the server
    ssize_t bytes_received = recv(client_fd, &value, sizeof(value), 0);
    if (bytes_received != sizeof(value)) {
        perror("Failed to receive the value from the server");
        close(client_fd);
        return -11;
    }

    // Execute the function based on the received value
    if (value == 1) {
        turnLeft90Degrees();
    }

    // Close the robot and the socket
    knet_close(dsPic);
    close(client_fd);

    return 0;
}

//*************************************************************************//
Using openCV

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <opencv2/opencv.hpp>
#include <khepera/khepera.h>

#define IP "192.168.1.4"
#define PORT 4455
#define SIZE 1024

static knet_dev_t *dsPic;

void turnLeft90Degrees() {
    // Your turnLeft90Degrees function implementation
    // ...
}

int captureImagesAndSend(int client_fd) {
    // Capture an image using OpenCV
    cv::VideoCapture cap("/dev/video6");
    cv::Mat frame;

    if (!cap.isOpened()) {
        printf("Error: Cannot open camera\n");
        return -3;
    }

    cap >> frame;

    if (frame.empty()) {
        printf("Error: Captured frame is empty\n");
        return -4;
    }

    // Convert the image to a buffer
    std::vector<uchar> buf;
    cv::imencode(".jpg", frame, buf);

    // Send the image buffer to the server
    if (send(client_fd, buf.data(), buf.size(), 0) < 0) {
        perror("Send failed");
        return -5;
    }

    cap.release();
    return 0;
}

int main() {
    int client_fd;
    struct sockaddr_in server_addr;
    int value;

    if (kh4_init(0, NULL) != 0) {
        printf("\nERROR: could not initiate the libkhepera!\n\n");
        return -1;
    }

    dsPic = knet_open("Khepera4:dsPic", KNET_BUS_I2C, 0, NULL);

    if (dsPic == NULL) {
        printf("\nERROR: could not initiate communication with Kh4 dsPic\n\n");
        return -2;
    }

    client_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (client_fd < 0) {
        perror("Failed to create socket");
        return -6;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(IP);

    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(client_fd);
        return -7;
    }

    int captureResult = captureImagesAndSend(client_fd);
    if (captureResult != 0) {
        // Handle errors or take appropriate action based on the return code.
        close(client_fd);
        return captureResult;
    }

    // Receive a value from the server
    ssize_t bytes_received = recv(client_fd, &value, sizeof(value), 0);
    if (bytes_received != sizeof(value)) {
        perror("Failed to receive the value from the server");
        close(client_fd);
        return -8;
    }

    // Execute the function based on the received value
    if (value == 1) {
        turnLeft90Degrees();
    }

    // Close the robot and the socket
    knet_close(dsPic);
    close(client_fd);

    return 0;
}

//**********************************************************************************************
// Trying new method to execute the program - see and let it work.


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <khepera/khepera.h>

#define IP "192.168.1.4"
#define PORT 4455
#define SIZE 1024

static knet_dev_t *dsPic;

void turnLeft90Degrees() {
    int duration = 4000000;
    int speedLeft = -20;
    int speedRight = 40;
    kh4_SetMode(kh4RegSpeed, dsPic);
    kh4_set_speed(speedLeft, speedRight, dsPic);
    usleep(duration);
    kh4_set_speed(0, 0, dsPic);
}

int captureImage() {
    int ret = system("media-pipes.sh");
    if (ret != 0) {
        perror("Failed to run media-pipes.sh");
        return -3;
    }

    printf("media-pipes.sh completed successfully.\n");

    ret = system("media-formats.sh 752 480");
    if (ret != 0) {
        perror("Failed to run media-formats.sh");
        return -4;
    }

    printf("media-formats.sh completed successfully.\n");

    FILE *v4l2grab = popen("v4l2grab -d /dev/video6 -W 752 -H 480 -q 85 -I -1", "r");
    if (v4l2grab == NULL) {
        perror("Failed to execute v4l2grab");
        return -5;
    }

    char buffer[SIZE];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, SIZE, v4l2grab)) > 0) {
	int sendImage(int client_fd) {
    	FILE *image_file = fopen("/home/root/image.jpg", "rb");
        if (image_file == NULL) {
        perror("Failed to open image file");
        return -6;
    }

    char buffer[SIZE];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, SIZE, image_file)) > 0) {
        if (send(client_fd, buffer, bytes_read, 0) < 0) {
            perror("Send failed");
            fclose(image_file);
            return -7;
        }
    }

    fclose(image_file);

    pclose(v4l2grab);
    return 0;
}

int main() {
        int client_fd;
    struct sockaddr_in server_addr;
    int value;

    if (kh4_init(0, NULL) != 0) {
        printf("\nERROR: could not initiate the libkhepera!\n\n");
        return -1;
    }

    dsPic = knet_open("Khepera4:dsPic", KNET_BUS_I2C, 0, NULL);

    if (dsPic == NULL) {
        printf("\nERROR: could not initiate communication with Kh4 dsPic\n\n");
        return -2;
    }

    client_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (client_fd < 0) {
        perror("Failed to create socket");
        return -9;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(IP);

    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(client_fd);
        return -10;
    }

    int captureResult = captureImage();
    if (captureResult != 0) {
        // Handle errors or take appropriate action based on the return code
        return captureResult;
    }

    // Receive a value from the server
    ssize_t bytes_received = recv(client_fd, &value, sizeof(value), 0);
    if (bytes_received != sizeof(value)) {
        perror("Failed to receive the value from the server");
        close(client_fd);
        return -11;
    }

    // Execute the function based on the received value
    if (value == 1) {
        turnLeft90Degrees();
    }

    // Close the robot and the socket
    knet_close(dsPic);
    close(client_fd);

    return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////

CURRENT WORKING CODE - BACK UP DATE 14-NOV

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <khepera/khepera.h>
#include <signal.h>

int client_fd;  // Declare client socket file descriptor globally
#define SIZE 1024

void signal_handler(int signal) {
    if (signal == SIGINT) {
        printf("Received interrupt signal. Cleaning up...\n");

        // Close socket and perform other clean-up tasks
        close(client_fd);

        exit(EXIT_SUCCESS);  // Exit the program gracefully
    }
}

int main() {

	signal(SIGINT, signal_handler);

	char buffer[SIZE];
	
    // Robot and socket initialization
    static knet_dev_t *dsPic;
    if (kh4_init(0, NULL) != 0) {
        printf("\nERROR: could not initiate the libkhepera!\n\n");
        return -1;
    }

    dsPic = knet_open("Khepera4:dsPic", KNET_BUS_I2C, 0, NULL);
    if (dsPic == NULL) {
        printf("\nERROR: could not initiate communication with Kh4 dsPic\n\n");
        return -2;
    }

    // Socket creation and connection
    int client_fd;
    struct sockaddr_in server_addr;
    int value;

    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        perror("Failed to create socket");
        return -9;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(3000);
    server_addr.sin_addr.s_addr = inet_addr("192.168.1.4");

    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(client_fd);
        return -10;
    }

    // Capture and send the image
    int ret = system("media-pipes.sh");
    if (ret != 0) {
        perror("Failed to run media-pipes.sh");
        close(client_fd);
        return -3;
    }

    printf("media-pipes.sh completed successfully.\n");

    ret = system("media-formats.sh 752 480");
    if (ret != 0) {
        perror("Failed to run media-formats.sh");
        close(client_fd);
        return -4;
    }

    printf("media-formats.sh completed successfully.\n");
//Capture the image
    ret = system("v4l2grab -d /dev/video6 -o /home/root/image.jpg -W 752 -H 480 -q 85 -I -1");
    if (ret != 0) {
        perror("Failed to capture an image with v4l2grab");
        return -5;
    }

    //Write a code to downgrade the image to 192*116
    //print the results

    printf("Image captured and saved as 'image.jpg'.\n");

    printf("Trying to open the image.jpg and read it'.\n");

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Open and read the image file from the root directory
        FILE *image_file = fopen("/home/root/image.jpg", "rb");
        if (image_file == NULL) {
            perror("Failed to open image file");
            return 1;
        }

        printf("Success......\n");

        printf("Trying to send it to server.....\n");

        // Send the image file to the server
        while (1) {
            ssize_t bytes_read = fread(buffer, 1, SIZE, image_file);
            if (bytes_read <= 0) {
                // Send a special signal (e.g., a specific character) to indicate the end of the image
                send(client_fd, "EOF", 3, 0);
                break;  // End of file
            }
            if (send(client_fd, buffer, bytes_read, 0) < 0) {
                perror("Send failed");
                return 1;
            }
        }

        printf("Success.......\n");
        fclose(image_file);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Receive a value from the server

        printf("Waiting for value from server.......\n");
    ssize_t bytes_received = recv(client_fd, &value, sizeof(value), 0);
    if (bytes_received != sizeof(value)) {
        perror("Failed to receive the value from the server");
        close(client_fd);
        return -11;
    }

    // Execute the function based on the received value
    if (value == 1) {
        int duration = 4000000;
        int speedLeft = -20;
        int speedRight = 40;
        kh4_SetMode(kh4RegSpeed, dsPic);
        kh4_set_speed(speedLeft, speedRight, dsPic);
        usleep(duration);
        kh4_set_speed(0, 0, dsPic);
    }

    // Close the robot and the socket
    knet_close(dsPic);
    close(client_fd);

    return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
CURRENT WORKING CODE FOR ROBOT ON NOV 16 - BACK-UP.

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <khepera/khepera.h>
#include <signal.h>

int client_fd;  // Declare client socket file descriptor globally
#define SIZE 1024

void signal_handler(int signal) {
    if (signal == SIGINT) {
        printf("Received interrupt signal. Cleaning up...\n");

        // Close socket and perform other clean-up tasks
        close(client_fd);

        exit(EXIT_SUCCESS);  // Exit the program gracefully
    }
}

int main() {

    signal(SIGINT, signal_handler);

    //char buffer[SIZE];

    // Robot and socket initialization
    static knet_dev_t *dsPic;
    
    if (kh4_init(0, NULL) != 0) {
        printf("\nERROR: could not initiate the libkhepera!\n\n");
        return -1;
    }

    dsPic = knet_open("Khepera4:dsPic", KNET_BUS_I2C, 0, NULL);
    if (dsPic == NULL) {
        printf("\nERROR: could not initiate communication with Kh4 dsPic\n\n");
        return -2;
    }

    // Socket creation and connection
    int client_fd;
    struct sockaddr_in server_addr;
    int value;

    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        perror("Failed to create socket");
        return -9;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(3000);
    server_addr.sin_addr.s_addr = inet_addr("192.168.1.4");

    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(client_fd);
        return -10;
    }

    // Capture and send the image
    int ret = system("media-pipes.sh");
    if (ret != 0) {
        perror("Failed to run media-pipes.sh");
        close(client_fd);
        return -3;
    }

    printf("media-pipes.sh completed successfully.\n");

    ret = system("media-formats.sh 752 480");
    if (ret != 0) {
        perror("Failed to run media-formats.sh");
        close(client_fd);
        return -4;
    }

    printf("media-formats.sh completed successfully.\n");
//Capture the image
    ret = system("v4l2grab -d /dev/video6 -o /home/root/image.jpg -W 752 -H 480 -q 85 -I -1");
    if (ret != 0) {
        perror("Failed to capture an image with v4l2grab");
        return -5;
    }

    //Write a code to downgrade the image to 192*116
    //print the results

    printf("Image captured and saved as 'image.jpg'.\n");

    printf("Trying to open the image.jpg and read it'.\n");

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////

    FILE *image_file = fopen("/home/root/image.jpg", "rb");
        if (image_file == NULL) {
            perror("Failed to open image file");
            return 1;
        }

        // Get the size of the image
        fseek(image_file, 0, SEEK_END);
        long image_size = ftell(image_file);
        fseek(image_file, 0, SEEK_SET);

        // Print the size of the image
        printf("Image size: %ld bytes\n", image_size);

        // Create a buffer to hold the image data
        char buffer[SIZE];

        // Send the size of the image
        if (send(client_fd, &image_size, sizeof(long), 0) < 0) {
            perror("Send failed");
            fclose(image_file);
            return 1;
        }

        // Wait for the signal to receive the image
        char signal[15];
        if (recv(client_fd, signal, sizeof(signal), 0) < 0) {
            perror("Receive failed");
            fclose(image_file);
            return 1;
        }

        // Check if the signal is "Ready for Image"
        if (strcmp(signal, "Ready for Image") != 0) {
            fprintf(stderr, "Error: Unexpected signal received\n");
            fclose(image_file);
            return 1;
        }

        printf("Sending image...\n");

        // Send the image file to the server
        while (1) {
            ssize_t bytes_read = fread(buffer, 1, SIZE, image_file);
            if (bytes_read <= 0) {
                // Send a special signal (e.g., a specific character) to indicate the end of the image
                send(client_fd, "EOF", 3, 0);
                break;  // End of file
            }
            if (send(client_fd, buffer, bytes_read, 0) < 0) {
                perror("Send failed");
                fclose(image_file);
                return 1;
            }
        }

        printf("Image sent successfully\n");
        fclose(image_file);

        return 1;
    }

    // Open and read the image file from the root directory
    /*    FILE *image_file = fopen("/home/root/image.jpg", "rb");
        if (image_file == NULL) {
            perror("Failed to open image file");
            return 1;
        }

        printf("Success......\n");

        printf("Trying to send it to server.....\n");

        // Send the image file to the server
        while (1) {
            ssize_t bytes_read = fread(buffer, 1, SIZE, image_file);
            if (bytes_read <= 0) {
                // Send a special signal (e.g., a specific character) to indicate the end of the image
                send(client_fd, "EOF", 3, 0);
                break;  // End of file
            }
            if (send(client_fd, buffer, bytes_read, 0) < 0) {
                perror("Send failed");
                return 1;
            }
        }

        printf("Success.......\n");
        fclose(image_file);     */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Receive a value from the server

    printf("Waiting for value from server.......\n");
    ssize_t bytes_received = recv(client_fd, &value, sizeof(value), 0);
    if (bytes_received != sizeof(value)) {
        perror("Failed to receive the value from the server");
        close(client_fd);
        return -11;
    }

    // Execute the function based on the received value
    if (value == 1) {
        int duration = 4000000;
        int speedLeft = -20;
        int speedRight = 40;
        kh4_SetMode(kh4RegSpeed, dsPic);
        kh4_set_speed(speedLeft, speedRight, dsPic);
        usleep(duration);
        kh4_set_speed(0, 0, dsPic);
    }

    // Close the robot and the socket
    knet_close(dsPic);
    close(client_fd);

    return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <khepera/khepera.h>
#include <signal.h>

int client_fd;  // Declare client socket file descriptor globally
#define SIZE 1024

void signal_handler(int signal) {
    if (signal == SIGINT) {
        printf("Received interrupt signal. Cleaning up...\n");

        // Close socket and perform other clean-up tasks
        close(client_fd);

        exit(EXIT_SUCCESS);  // Exit the program gracefully
    }
}

int main() {

    signal(SIGINT, signal_handler);

    //char buffer[SIZE];

    // Robot and socket initialization
    static knet_dev_t *dsPic;
    
    if (kh4_init(0, NULL) != 0) {
        printf("\nERROR: could not initiate the libkhepera!\n\n");
        return -1;
    }

    dsPic = knet_open("Khepera4:dsPic", KNET_BUS_I2C, 0, NULL);
    if (dsPic == NULL) {
        printf("\nERROR: could not initiate communication with Kh4 dsPic\n\n");
        return -2;
    }

    // Socket creation and connection
    int client_fd;
    struct sockaddr_in server_addr;
    int value;

    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        perror("Failed to create socket");
        return -9;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(3000);
    server_addr.sin_addr.s_addr = inet_addr("192.168.1.4");

    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(client_fd);
        return -10;
    }

    // Capture and send the image
    int ret = system("media-pipes.sh");
    if (ret != 0) {
        perror("Failed to run media-pipes.sh");
        close(client_fd);
        return -3;
    }

    printf("media-pipes.sh completed successfully.\n");

    ret = system("media-formats.sh 752 480");
    if (ret != 0) {
        perror("Failed to run media-formats.sh");
        close(client_fd);
        return -4;
    }

    printf("media-formats.sh completed successfully.\n");
    //Capture the image
    ret = system("v4l2grab -d /dev/video6 -o /home/root/image.jpg -W 752 -H 480 -q 85 -I -1");
    if (ret != 0) {
        perror("Failed to capture an image with v4l2grab");
        return -5;
    }


    printf("Image captured and saved as 'image.jpg'.\n");

    printf("Trying to open the image.jpg and read it'.\n");

    //SENDING IMAGE AND RECEIVING A VALUE

    FILE *image_file = fopen("/home/root/image.jpg", "rb");
        if (image_file == NULL) {
            perror("Failed to open image file");
            return 1;
        }

        // Get the size of the image
        fseek(image_file, 0, SEEK_END);
        long image_size = ftell(image_file);
        fseek(image_file, 0, SEEK_SET);

        // Print the size of the image
        printf("Image size: %ld bytes\n", image_size);

        // Create a buffer to hold the image data
        char buffer[SIZE];

        // Send the size of the image
        if (send(client_fd, &image_size, sizeof(long), 0) < 0) {
            perror("Send failed");
            fclose(image_file);
            return 1;
        }

        // Wait for the signal to receive the image
        char signal[15];
        if (recv(client_fd, signal, sizeof(signal), 0) < 0) {
            perror("Receive failed");
            fclose(image_file);
            return 1;
        }

        // Check if the signal is "Ready for Image"
        if (strcmp(signal, "Ready for Image") != 0) {
            fprintf(stderr, "Error: Unexpected signal received\n");
            fclose(image_file);
            return 1;
        }

        printf("Sending image...\n");

        // Send the image file to the server
        while (1) {
            ssize_t bytes_read = fread(buffer, 1, SIZE, image_file);
            if (bytes_read <= 0) {
                // Send a special signal (e.g., a specific character) to indicate the end of the image
                send(client_fd, "EOF", 3, 0);
                break;  // End of file
            }
            if (send(client_fd, buffer, bytes_read, 0) < 0) {
                perror("Send failed");
                fclose(image_file);
                return 1;
            }
        }

        printf("Image sent successfully\n");
        fclose(image_file);

        return 1;
    }


// Receive a value from the server

    printf("Waiting for value from server.......\n");
    ssize_t bytes_received = recv(client_fd, &value, sizeof(value), 0);
    if (bytes_received != sizeof(value)) {
        perror("Failed to receive the value from the server");
        close(client_fd);
        return -11;
    }

    // Execute the function based on the received value
    if (value == 1) {
        int duration = 4000000;
        int speedLeft = -20;
        int speedRight = 40;
        kh4_SetMode(kh4RegSpeed, dsPic);
        kh4_set_speed(speedLeft, speedRight, dsPic);
        usleep(duration);
        kh4_set_speed(0, 0, dsPic);
    }

    // Close the robot and the socket
    knet_close(dsPic);
    close(client_fd);

    return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
//Last tried code not working. Nov 16.
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <khepera/khepera.h>
#include <signal.h>

#define SIZE 1024
typedef int TYPE_OF_VALUE;

int client_fd; // Declare client socket file descriptor globally

void signal_handler(int signal) {
    if (signal == SIGINT) {
        printf("Received interrupt signal. Cleaning up...\n");

        // Close socket and perform other clean-up tasks
        close(client_fd);

        exit(EXIT_SUCCESS); // Exit the program gracefully
    }
}

int main() {
    int value;

    signal(SIGINT, signal_handler);

    // ... (Robot and socket initialization code)
    static knet_dev_t *dsPic;

    if (kh4_init(0, NULL) != 0) {
        printf("\nERROR: could not initiate the libkhepera!\n\n");
        return -1;
    }

    dsPic = knet_open("Khepera4:dsPic", KNET_BUS_I2C, 0, NULL);
    if (dsPic == NULL) {
        printf("\nERROR: could not initiate communication with Kh4 dsPic\n\n");
        return -2;
    }

    // Socket creation and connection
    int client_fd;
    struct sockaddr_in server_addr;
    //int value;

    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        perror("Failed to create socket");
        return -9;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(4455);
    server_addr.sin_addr.s_addr = inet_addr("192.168.1.4");

    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(client_fd);
        return -10;
    }


    // ... (Code for capturing and saving the image)
    int ret = system("media-pipes.sh");
    if (ret != 0) {
        perror("Failed to run media-pipes.sh");
        close(client_fd);
        return -3;
    }

    printf("media-pipes.sh completed successfully.\n");

    ret = system("media-formats.sh 752 480");
    if (ret != 0) {
        perror("Failed to run media-formats.sh");
        close(client_fd);
        return -4;
    }

    printf("media-formats.sh completed successfully.\n");
    //Capture the image
    ret = system("v4l2grab -d /dev/video6 -o /home/root/image.jpg -W 752 -H 480 -q 85 -I -1");
    if (ret != 0) {
        perror("Failed to capture an image with v4l2grab");
        return -5;
    }


    printf("Image captured and saved as 'image.jpg'.\n");

    printf("Trying to open the image.jpg and read it'.\n");


    // Sending image and receiving a value
    FILE *image_file = fopen("/home/root/image.jpg", "rb");
    if (image_file == NULL) {
        perror("Failed to open image file");
        return 1;
    }

    // Get the size of the image
    fseek(image_file, 0, SEEK_END);
    long image_size = ftell(image_file);
    fseek(image_file, 0, SEEK_SET);

    // Print the size of the image
    printf("Image size: %ld bytes\n", image_size);

    // Create a buffer to hold the image data
    char buffer[SIZE];

    // Send the size of the image
    if (send(client_fd, &image_size, sizeof(long), 0) < 0) {
        perror("Send failed");
        fclose(image_file);
        return 1;
    }

    // Wait for the signal to receive the image
    char signal[15];
    if (recv(client_fd, signal, sizeof(signal), 0) < 0) {
        perror("Receive failed");
        fclose(image_file);
        return 1;
    }

    // Check if the signal is "Ready for Image"
    if (strcmp(signal, "Ready for Image") != 0) {
        fprintf(stderr, "Error: Unexpected signal received\n");
        fclose(image_file);
        return 1;
    }

    printf("Sending image...\n");

    // Send the image file to the server
    while (1) {
        ssize_t bytes_read = fread(buffer, 1, SIZE, image_file);
        if (bytes_read <= 0) {

            send(client_fd, "EOF", 3, 0);
            break; // End of file
        }
        if (send(client_fd, buffer, bytes_read, 0) < 0) {
            perror("Send failed");
            fclose(image_file);
            return 1;
        }
        if(bytes_read >= image_size) {
            break;
        }


    }
    printf("Image size read: %ld bytes\n", image_size);
    printf("Image size sent: %ld bytes\n", image_size);
    printf("Image sent successfully\n");
    fclose(image_file);

    // Receive a value from the server
    printf("Waiting for value from server.......\n");
    ssize_t bytes_received = recv(client_fd, &value, sizeof(value), 0);
    if (bytes_received != sizeof(value)) {
        perror("Failed to receive the value from the server");
        close(client_fd);
        return -11;
    }

    // Execute the function based on the received value
    if (value == 1) {
        int duration = 4000000;
        int speedLeft = -20;
        int speedRight = 40;
        kh4_SetMode(kh4RegSpeed, dsPic);
        kh4_set_speed(speedLeft, speedRight, dsPic);
        usleep(duration);
        kh4_set_speed(0, 0, dsPic);
    }

    // Close the robot and the socket
    knet_close(dsPic);
    close(client_fd);

    return 0;
}




//GO BACK TO BASICS

####################################################
/********  LAST WORKING SOURCE CODE **********************/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <khepera/khepera.h>

#define IP "192.168.1.4"
#define PORT 5544
#define SIZE 1024

static knet_dev_t *dsPic;
//static int quitReq = 0; //'quitReq' is a global variable.


int captureImages() {
    // Capture images once outside the loop
    int ret = system("media-pipes.sh");
    if (ret != 0) {
        perror("Failed to run media-pipes.sh");
        return -3;
    }

    ret = system("media-formats.sh 752 480");
    if (ret != 0) {
        perror("Failed to run media-formats.sh");
        return -4;
    }

          printf("Taking images...\n");

        ret = system("v4l2grab -d /dev/video6 -o image.jpg -W 752 -H 480 -q 85 -I -1");
        if (ret != 0) {
            perror("Failed to capture an image with v4l2grab");
            return -5;
        }

        printf("Image captured and saved as 'image.jpg'.\n");


    // Return a success code if needed.
    return 0;
}

void turnLeft90Degrees() {
    int duration = 4000000;
    int speedLeft = -20;
    int speedRight = 40;
    kh4_SetMode(kh4RegSpeed, dsPic);
    kh4_set_speed(speedLeft, speedRight, dsPic);
    usleep(duration);
    kh4_set_speed(0, 0, dsPic);
}



int main() {
    int client_fd;
    struct sockaddr_in server_addr;
    char buffer[SIZE];
    int prediction;

    // robot initialization
    if (kh4_init(0, NULL) != 0) {
        printf("\nERROR: could not initiate the libkhepera!\n\n");
        return -1;
    }

    dsPic = knet_open("Khepera4:dsPic", KNET_BUS_I2C, 0, NULL);

    if (dsPic == NULL) {
        printf("\nERROR: could not initiate communication with Kh4 dsPic\n\n");
        return -2;
    }

    //Function to capture image
    int result = captureImages();
    if (result != 0) {
        // Handle errors or take appropriate action based on the return code.
    }

    // Create a socket
    client_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Configure the server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(IP);

    // Connect to the server
    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        return 1;
    }

    // Open and read the image file from the root directory
    FILE *image_file = fopen("/home/root/image.jpg", "rb");
    if (image_file == NULL) {
        perror("Failed to open image file");
        return 1;
    }

    // Get the size of image
    fseek(image_file, 0, SEEK_END);
    long image_size = ftell(image_file);
    fseek(image_file, 0, SEEK_SET);

    //Print the size of the image
    printf("Image size: %ld bytes\n", image_size);

    // Send the image file to the server
    while (1) {
        ssize_t bytes_read = fread(buffer, 1, SIZE, image_file);
        if (bytes_read <= 0) {
            break;  // End of file
        }
        if (send(client_fd, buffer, bytes_read, 0) < 0) {
            perror("Send failed");
            return 1;
        }
    }

    // Close the image file
    fclose(image_file);
    //Close the connection
    close(client_fd);


    // Connect to the server SECOND TIME

    // Create a socket for the second connection
    int client_fd_receive = socket(AF_INET, SOCK_STREAM, 0);

    // Configure the server address for the second connection
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(IP);

    // Connect to the server for the second connection
    if (connect(client_fd_receive, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Second connection failed");
        return 1;
    }

    //Connect to server again to
    // Receive a value from the server
    ssize_t bytes_received = recv(client_fd, &prediction, sizeof(uint8_t), 0);

    if (bytes_received != sizeof(uint8_t)) {
        perror("Failed to receive the value from the server");
        close(client_fd);
        return -11;
    }

    // Convert from big-endian to host byte order
    prediction = be16toh(prediction);

    printf("Received prediction: %d\n", prediction);

    // Execute the function based on the received value
    if (prediction == 1) {
        turnLeft90Degrees();
    }


    //Close the connection
    close(client_fd);

    return 0;
}




