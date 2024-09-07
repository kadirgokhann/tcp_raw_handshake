#include "ServerTcpSession.hpp"
#include "utils.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>

#include "ServerTcpSession.hpp"
#include "utils.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>

ServerTcpSession::ServerTcpSession() 
{
    CreateSocket();  // Ensure socket is created when the ServerTcpSession is initialized
}

bool ServerTcpSession::ListenForConnection()
{
    // Ensure the local IP is fetched
    GetLocalIp();

    struct sockaddr_in server_addr;
    // Set up server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(_local_ip.c_str());
    server_addr.sin_port = htons(8082); // Use port 8082 or your desired port

    // Bind the socket to the specified IP and port
    if (bind(_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(_sock);
        return false;
    }

    // Listen for incoming connections
    if (listen(_sock, 1) < 0) {
        perror("Listen failed");
        close(_sock);
        return false;
    }

    std::cout << "Server is listening for connections..." << std::endl;

    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Accept incoming connection
    int client_sock = accept(_sock, (struct sockaddr *)&client_addr, &client_len);
    if (client_sock < 0) {
        perror("Accept failed");
        close(_sock);
        return false;
    }

    std::cout << "Client connected!" << std::endl;
    close(_sock);  // Close the server's listening socket
    _sock = client_sock;  // Replace with the client socket to handle the connection

    return true;
}




void ServerTcpSession::RespondWithSYN_ACK()
{
    PrepareHeaders(_sendPacket);
    _sendPacket.tcp_header->th_flags = TH_SYN | TH_ACK;
    _sendPacket.tcp_header->th_ack = htonl(ntohl(_recvPacket.tcp_header->th_seq) + 1);

    if (sendto(_sock, _sendPacket.data, ntohs(_sendPacket.ip_header->ip_len), 0, 
        (struct sockaddr*)&_recvPacket.destination, sizeof(_recvPacket.destination)) < 0) 
    {
        perror("SYN-ACK sending failed");
        close(_sock);
        exit(1);
    }

    std::cout << "SYN-ACK sent" << std::endl;
}

bool ServerTcpSession::ReceiveACK()
{
    struct sockaddr_in from;
    socklen_t from_len = sizeof(from);

    ssize_t n = recvfrom(_sock, _recvPacket.data, sizeof(_recvPacket.data), 0, 
        (struct sockaddr*)&from, &from_len);
    if (n < 0) 
    {
        perror("Receiving failed");
        return false;
    }

    _recvPacket.ip_header  = (struct ip*)_recvPacket.data;
    _recvPacket.tcp_header = (struct tcphdr*)(_recvPacket.data + _recvPacket.ip_header->ip_hl * 4);

    if (!(_recvPacket.tcp_header->th_flags & TH_ACK)) 
    {
        std::cerr << "Not an ACK packet" << std::endl;
        return false;
    }

    std::cout << "ACK packet received, handshake complete" << std::endl;
    close(_sock);
    return true;
}
