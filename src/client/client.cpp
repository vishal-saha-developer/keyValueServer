#include "client.h"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

Client::Client(std::string server_ip, std::string client_name) : server_ip(server_ip), client_name(client_name), server_socket(-1)
{
    init();
}

Client::~Client()
{
    stop();
}

void Client::init()
{
    std::cout << "Initializing client" << std::endl;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        std::cerr << "Failed to create server socket" << std::endl;
        exit(1);
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);
    server_address.sin_addr.s_addr = inet_addr(server_ip.c_str());

    if (connect(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        std::cerr << "Failed to connect to server" << std::endl;
        exit(1);
    }
}

void Client::stop()
{
    close(server_socket);
    server_socket = -1;    
}

void Client::run()
{
    std::cout << "Running client" << std::endl;
    char buffer[1024];
    while (true)
    {
        std::cout << "Enter message to send to server: ";
        std::cin.getline(buffer, sizeof(buffer));
        send(server_socket, buffer, strlen(buffer), 0);
        std::cout << "Message sent to server" << std::endl;
    }
}
