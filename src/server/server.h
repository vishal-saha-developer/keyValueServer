#ifndef SERVER_H
#define SERVER_H
#include <string>

class Server
{
public:
    Server(std::string server_ip, std::string server_name = "default");
    ~Server();

    void run();

private:
    const int server_port = 9000;
    const std::string server_name;
    const std::string server_ip;

    void init();
    void stop();

    int server_listen_socket;
    int epoll_fd;

    // Client handling
    void handle_client_connection_request();
    void handle_client_disconnection(int client_socket);
    void handle_client_incoming_message(int client_socket);
};

#endif