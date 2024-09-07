#ifndef SERVER_TCP_SESSION_HPP
#define SERVER_TCP_SESSION_HPP

#include "TcpSession.hpp"

class ServerTcpSession : public TcpSession
{
public:
    ServerTcpSession();
    bool ListenForConnection();  // Server listens for incoming SYN
    void RespondWithSYN_ACK();   // Server sends SYN-ACK
    bool ReceiveACK();           // Server receives ACK to complete the handshake
};

#endif
