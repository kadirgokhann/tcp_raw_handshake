#include "ClientTcpSession.hpp"
#include <iostream>

int main() 
{
    ClientTcpSession client;

    if (!client.ConnectToServer("localhost", 8082)) {
        std::cerr << "Failed to connect to server!" << std::endl;
        return 1;
    }

    client.SendSYN();

    if (client.ReceiveSYN_ACK()) {
        client.SendACK();
    } else {
        std::cerr << "Failed to complete handshake!" << std::endl;
    }

    return 0;
}
