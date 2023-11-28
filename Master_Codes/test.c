
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

    // Close the connection
    close(client_fd);

    return 0;
}

////////////////////////////////////////////////////////////////////////////////////
Current working code:


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

    // Close the connection
    //close(client_fd);

    return 0;
}


