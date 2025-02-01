#include "client.h"
#include <iostream>

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <ip of server>" << std::endl;
        exit(1);
    }
    std::string ip = argv[1];
    Client client(ip);
    client.run();
    return 0;
}
