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

// Function to create a socket
int createSocket() {
    int client_fd;
    struct sockaddr_in server_addr;

    // Create a socket
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        return -1;
    }

    // Configure the server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(IP);

    // Connect to the server
    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        return -1;
    }

    return client_fd;  // Return the socket file descriptor
}

int main() {
    char buffer[SIZE];
    int prediction;

    // ... (robot initialization and other code)
    if (kh4_init(0, NULL) != 0) {
        printf("\nERROR: could not initiate the libkhepera!\n\n");
        return -1;
    }

    dsPic = knet_open("Khepera4:dsPic", KNET_BUS_I2C, 0, NULL);

    if (dsPic == NULL) {
        printf("\nERROR: could not initiate communication with Kh4 dsPic\n\n");
        return -2;
    }
    // Loop for continuous image capture and transmission
    while (1) {
        //Function to capture image
        int result = captureImages();
        if (result != 0) {
            // Handle errors or take appropriate action based on the return code.
        }

        // Create a socket using the createSocket function
        int client_fd = createSocket();
        if (client_fd < 0) {
            // Handle socket creation failure
            return 1;
        }

        // Open and read the image file from the root directory
        FILE *image_file = fopen("/home/root/image.jpg", "rb");
        if (image_file == NULL) {
            perror("Failed to open image file");
            return 1;
        }

        // ... (send the image file to the server)
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

        // Wait for 2 seconds before capturing the next image
        sleep(2);
    }

    return 0;
}
