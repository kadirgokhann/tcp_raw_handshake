#include "ClientTcpSession.hpp"
#include "utils.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>

ClientTcpSession::ClientTcpSession() {}

bool ClientTcpSession::ConnectToServer(const char* server_address, int port)
{
    CreateSocket();
    SetDestination(server_address, port);
    std::cout << "Destination set" << std::endl;
    // Use connect() to establish a TCP connection with the server
    if (connect(_sock, (struct sockaddr *)&_sendPacket.destination, sizeof(_sendPacket.destination)) < 0) {
        perror("Connection to server failed");
        close(_sock);
        return false;
    }

    std::cout << "Connected to server!" << std::endl;
    return true;
}

void ClientTcpSession::SendSYN()
{
    PrepareHeaders(_sendPacket);
    _sendPacket.tcp_header->th_flags = TH_SYN;  // SYN flag

    // Use send() instead of sendto() after the socket is connected
    if (send(_sock, _sendPacket.data, ntohs(_sendPacket.ip_header->ip_len), 0) < 0) {
        perror("SYN sending failed");
        close(_sock);
        exit(1);
    }
    std::cout << "SYN packet sent" << std::endl;
}

bool ClientTcpSession::ReceiveSYN_ACK()
{
    struct sockaddr_in from;
    socklen_t from_len = sizeof(from);

    ssize_t n = recv(_sock, _recvPacket.data, sizeof(_recvPacket.data), 0);  // Use recv() for TCP
    if (n < 0) 
    {
        perror("Receiving failed");
        return false;
    }

    _recvPacket.ip_header  = (struct ip*)_recvPacket.data;
    _recvPacket.tcp_header = (struct tcphdr*)(_recvPacket.data + _recvPacket.ip_header->ip_hl * 4);

    if (!(_recvPacket.tcp_header->th_flags & TH_SYN && _recvPacket.tcp_header->th_flags & TH_ACK)) 
    {
        std::cerr << "Not a SYN-ACK packet" << std::endl;
        return false;
    }

    std::cout << "SYN-ACK received" << std::endl;
    return true;
}

void ClientTcpSession::SendACK()
{
    _sendPacket.tcp_header->th_ack = htonl(ntohl(_recvPacket.tcp_header->th_seq) + 1);
    _sendPacket.tcp_header->th_flags = TH_ACK;

    if (send(_sock, _sendPacket.data, ntohs(_sendPacket.ip_header->ip_len), 0) < 0) {
        perror("ACK sending failed");
        close(_sock);
        exit(1);
    }

    std::cout << "ACK packet sent, handshake complete" << std::endl;
    close(_sock);
}
