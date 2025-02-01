#include "server.h"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
Server::Server(std::string server_ip, std::string server_name) : server_ip(server_ip), server_name(server_name)
{
    init();
}

Server::~Server()
{
    stop();
}

void Server::init()
{
    // Initialize epoll instance
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1)
    {
        std::cerr << "Failed to create epoll instance" << std::endl;
        exit(1);
    }

    // Initialize server socket
    server_listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_listen_socket == -1)
    {
        std::cerr << "Failed to create server listen socket" << std::endl;
        exit(1);
    }

    // Bind socket to server address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);
    server_address.sin_addr.s_addr = inet_addr(server_ip.c_str());

    if (bind(server_listen_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        std::cerr << "Failed to bind server listen socket" << std::endl;
        exit(1);
    }

    // Listen for incoming connections
    if (listen(server_listen_socket, 5) == -1)
    {
        std::cerr << "Failed to listen for incoming connections" << std::endl;
        exit(1);
    }

    // Add server listen socket to epoll
    struct epoll_event event;
    // EPOLLET: Edge-triggered mode
    // EPOLLIN: Data available for reading
    event.events = EPOLLIN | EPOLLET;
    event.data.fd = server_listen_socket;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_listen_socket, &event) == -1)
    {
        std::cerr << "Failed to add server listen socket to epoll" << std::endl;
        exit(1);
    }
}

void Server::stop()
{
    close(server_listen_socket);
    server_listen_socket = -1;
    close(epoll_fd);
    epoll_fd = -1;
}

void Server::run()
{
    const int MAX_EVENTS = 16;
    struct epoll_event events[MAX_EVENTS];
    while (true)
    {
        std::cout << "Waiting for events" << std::endl;
        int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (num_events == -1)
        {
            std::cerr << "Failed to wait for events" << std::endl;
            exit(1);
        }
        std::cout << "Number of events: " << num_events << std::endl;
        for (int i = 0; i < num_events; i++)
        {
            if (events[i].events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP))
            {
                std::cout << "Handling client disconnection" << std::endl;
                handle_client_disconnection(events[i].data.fd);
            }
            else if (events[i].data.fd == server_listen_socket)
            {
                std::cout << "Handling client connection request" << std::endl;
                handle_client_connection_request();
            }
            else if (events[i].events & EPOLLIN)
            {
                std::cout << "Handling client incoming message" << std::endl;
                handle_client_incoming_message(events[i].data.fd);
            }
        }
    }
}

void Server::handle_client_connection_request()
{
    int client_socket = accept(server_listen_socket, NULL, NULL);
    if (client_socket == -1)
    {
        std::cerr << "Failed to accept client connection" << std::endl;
        exit(1);
    }

    // Add client socket to epoll
    struct epoll_event event;
    // EPOLLET: Edge-triggered mode
    // EPOLLIN: Data available for reading
    // EPOLLRDHUP: Remote peer closed connection
    // EPOLLERR: Error occurred on socket
    // EPOLLHUP: Hang up occurred on socket
    event.events = EPOLLET | EPOLLIN | EPOLLRDHUP | EPOLLERR | EPOLLHUP;
    event.data.fd = client_socket;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket, &event) == -1)
    {
        std::cerr << "Failed to add client socket to epoll" << std::endl;
        exit(1);
    }
}

void Server::handle_client_disconnection(int client_socket)
{
    if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_socket, NULL) == -1)
    {
        std::cerr << "Failed to remove client socket from epoll" << std::endl;
        exit(1);
    }
    close(client_socket);
}

void Server::handle_client_incoming_message(int client_socket)
{
    char buffer[1024];
    ssize_t bytes_read = read(client_socket, buffer, sizeof(buffer));
    buffer[bytes_read] = '\0';
    if (bytes_read == -1)
    {
        std::cerr << "Failed to read client message" << std::endl;
        exit(1);
    }

    std::cout << "Client message: " << buffer << std::endl;
}