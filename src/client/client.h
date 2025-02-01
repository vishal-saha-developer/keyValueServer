#ifndef CLIENT_H
#define CLIENT_H
#include <string>

class Client
{
public:
    Client(std::string server_ip, std::string client_name = "client");
    ~Client();

    void run();

private:
    const std::string server_ip;
    const std::string client_name;
    const int server_port = 9000;
    int server_socket;

    void init();
    void stop();

    void handle_client_data();
    void handle_client_write();
};

#endif