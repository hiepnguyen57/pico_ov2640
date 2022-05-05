/**
 * @file get_image.c
 * @author Hiep Nguyen (hoahiepk10@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-04-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>

#define IMAGE_SIZE (352*288*2)
#define NORMAL_CMD (32)

//
// gcc get_image.c -o get_image
//
int modem_transfer(int fd, char *cmd) {
    char buffer[IMAGE_SIZE + 1] = {0};
    char ack[32] = {0};
    char *bufptr;
    int nbytes;
    int total_bytes = 0;
    char file_name[40] = {0};
    time_t now;
    FILE *fh;

    // send an AT command followed by a CR
    if (write(fd, cmd, strlen(cmd)) < 0) {
        perror("failed to write");
        return -1;
    }

    // wait for result
    bufptr = buffer;
    total_bytes = 0;
    while ((nbytes = read(fd, bufptr, buffer + sizeof(buffer) - bufptr - 1)) > 0) {
        bufptr += nbytes;
        total_bytes += nbytes;
        printf("Received %d bytes\n", total_bytes);

        if (total_bytes >= IMAGE_SIZE) {
            // Create and save file
            now = time(NULL);
            sprintf(file_name, "image_%d.bin", (int)now);
            fh = fopen(file_name, "wb");
            if (fh == NULL) {
                printf("Unable to create file.\n");
                exit(EXIT_FAILURE);
            }

            fwrite(buffer, total_bytes, 1, fh);
            fclose(fh);

            printf("\n");
            printf("\n=========== IMAGE FILE ==============\n");
            printf("File %s has created successfully. File size: %d\n", file_name, total_bytes);
            break;
        }
    }

    return 0;
}

int resolve_baud_rate(int baudRate) {
    switch (baudRate) {
        case 9600:
            return B9600;
        case 19200:
            return B19200;
        case 38400:
            return B38400;
        case 57600:
            return B57600;
        case 115200:
            return B115200;
        default:
            break;
    }

    return B57600;
}

int main(int argc, char *argv[]) {
    char device[] = "/dev/ttyACM0";
    char options[] = "d:c:b:n";
    int fd;
    struct termios tio;
    char *dev = device;
    char *cmd = NULL;
    int opt;
    int baud_rate = 57600;
    int wait_for_response = 1;

    if (argc == 1) {
        printf("Usage: %s\n", argv[0]);
        printf("  -c <cmd>\n");
        printf("  -d <device> (device. default: %s)\n", device);
        printf("  -b <baud rate>\n");
        return 0;
    }

    while ((opt = getopt(argc, argv, options)) != -1) {
        switch (opt) {
            case 'd':
                dev = optarg;
                break;
            case 'c':
                cmd = optarg;
                break;
            case 'b':
                baud_rate = strtol(optarg, NULL, 0);
                break;
        }
    }

    // open the port
    fd = open(dev, O_RDWR | O_NOCTTY | O_NDELAY);
    fcntl(fd, F_SETFL, 0);

    // get the current options
    tcgetattr(fd, &tio);

    // set raw input, 1 second timeout
    tio.c_cflag |= (CLOCAL | CREAD);
    tio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tio.c_oflag &= ~OPOST;
    tio.c_cc[VMIN] = 0;
    tio.c_cc[VTIME] = 10;

    cfsetospeed(&tio, resolve_baud_rate(baud_rate));
    cfsetispeed(&tio, resolve_baud_rate(baud_rate));

    // set the options
    tcsetattr(fd, TCSANOW, &tio);

    printf("device: %s\n", dev);
    printf("cmd: %s\n", cmd);
    printf("\n");

    // send at command
    modem_transfer(fd, "OV+CAPTURE?");

    return 0;
}