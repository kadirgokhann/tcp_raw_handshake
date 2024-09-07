#ifndef TCP_SESSION_HPP
#define TCP_SESSION_HPP

#include <netinet/in.h>
#include <string>

#include "Packet.hpp"
#include "utils.hpp"

class TcpSession
{
public:
    TcpSession();
    virtual ~TcpSession() = default;

protected:
    int _sock;
    std::string _local_ip;
    Packet _sendPacket;
    Packet _recvPacket;

    void CreateSocket();
    void GetLocalIp();
    void SetDestination(const char* address, int port);
    void PrepareHeaders(Packet& packet);
    void PrepareTcpHeader(Packet& packet);
};

#endif
