#ifndef CLIENT_TCP_SESSION_HPP
#define CLIENT_TCP_SESSION_HPP

#include "TcpSession.hpp"

class ClientTcpSession : public TcpSession
{
public:
    ClientTcpSession();
    bool ConnectToServer(const char* server_address, int port);
    void SendSYN();          // Client sends SYN packet
    bool ReceiveSYN_ACK();   // Client waits for SYN-ACK
    void SendACK();          // Client sends ACK packet to complete handshake
};

#endif
