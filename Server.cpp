#include "ServerTcpSession.hpp"
#include <iostream>

int main() 
{
    ServerTcpSession server;

    if (!server.ListenForConnection()) {
        std::cerr << "Failed to listen for connection!" << std::endl;
        return 1;
    }

    server.RespondWithSYN_ACK();

    if (!server.ReceiveACK()) {
        std::cerr << "Failed to complete handshake!" << std::endl;
    }

    return 0;
}
