#include "server.h"
#include <iostream>

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <ip of the server instance>" << std::endl;
        exit(1);
    }
    std::string ip = argv[1];
    Server server(ip, "server1");
    server.run();
    return 0;
}