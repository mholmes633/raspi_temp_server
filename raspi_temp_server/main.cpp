#include <iostream>
#include "dht22.h"
#include <wiringPi.h>
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>

#define PORT 54000
#define CELSIUS    1
#define FAHRENHEIT 0

int main(void)
{
	dht22* pDHT22 = new dht22();
	
	if (wiringPiSetup() == -1)
		exit(EXIT_FAILURE);

    int server_fd, new_socket, valread, err;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = { 0 };


    // Creating socket file descriptor 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080 
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
        &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("192.168.1.51");
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080 
    if (bind(server_fd, (struct sockaddr*) & address,
        sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr*) & address,
        (socklen_t*)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    bool loop = true;
    bool connected = true;
    while (loop)
    {
        if (connected == true)
        {

            valread = read(new_socket, buffer, 1024);
            printf("%s\n", buffer);
            if (valread == 0)
                break;



            if (buffer[0] == 'q')
            {
                connected = false;
                shutdown(server_fd, SHUT_RDWR);
            }
            else if (buffer[0] == 'r')
            {

                err = pDHT22->readDHT22(FAHRENHEIT);
                if (err == 0) {
                    sprintf(buffer, "%.0f degF     %.0f %%", pDHT22->getTemperature(), pDHT22->getHumidity());
                    send(new_socket, buffer, strlen(buffer), 0);
                }
                else {
                    sprintf(buffer, "DHT22 read error ... try again");
                    send(new_socket, buffer, strlen(buffer), 0);
                }

            }
            else
            {
                send(new_socket, buffer, strlen(buffer), 0);
            }
        }
        else
        {
            // Creating socket file descriptor 
            if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
            {
                perror("socket failed");
                exit(EXIT_FAILURE);
            }

            // Forcefully attaching socket to the port 8080 
            if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                &opt, sizeof(opt)))
            {
                perror("setsockopt");
                exit(EXIT_FAILURE);
            }
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = inet_addr("192.168.1.51");
            address.sin_port = htons(PORT);

            // Forcefully attaching socket to the port 8080 
            if (bind(server_fd, (struct sockaddr*) & address,
                sizeof(address)) < 0)
            {
                perror("bind failed");
                exit(EXIT_FAILURE);
            }
            if (listen(server_fd, 3) < 0)
            {
                perror("listen");
                exit(EXIT_FAILURE);
            }
            if ((new_socket = accept(server_fd, (struct sockaddr*) & address,
                (socklen_t*)&addrlen)) < 0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }
            connected = true;
        }
    }
	return 0;
}