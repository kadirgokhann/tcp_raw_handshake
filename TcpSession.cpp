#include "TcpSession.hpp"
#include "utils.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>

TcpSession::TcpSession() {}

void TcpSession::CreateSocket()
{
    // Create a TCP socket for server-side listening (SOCK_STREAM)
    _sock = socket(AF_INET, SOCK_STREAM, 0);  // Changed to SOCK_STREAM for TCP connections
    if (_sock < 0) 
    {
        perror("Socket creation failed");
        exit(1);
    }
}

void TcpSession::GetLocalIp()
{
    _local_ip = utils::GetLocalIp();
    if (_local_ip.empty()) 
    {
        std::cerr << "Failed to get local IP address" << std::endl;
        exit(1);
    }
}

void TcpSession::SetDestination(const char* address, int port)
{
    _sendPacket.destination.sin_family = AF_INET;
    _sendPacket.destination.sin_port = htons(port);
    inet_pton(AF_INET, address, &_sendPacket.destination.sin_addr);
}

void TcpSession::PrepareHeaders(Packet& packet)
{
    GetLocalIp();

    // Prepare the IP header
    packet.ip_header->ip_hl     = 5;  // Header length (5 x 4 = 20 bytes)
    packet.ip_header->ip_v      = 4;  // IPv4
    packet.ip_header->ip_tos    = 0;  // Type of service
    packet.ip_header->ip_len    = htons(sizeof(struct ip) + sizeof(struct tcphdr)); // Total packet length
    packet.ip_header->ip_id     = htons(54321); // Packet ID
    packet.ip_header->ip_off    = 0;  // No fragmentation
    packet.ip_header->ip_ttl    = 255;  // Time to live
    packet.ip_header->ip_p      = IPPROTO_TCP; // TCP protocol
    packet.ip_header->ip_sum    = 0;  // Checksum initially 0
    packet.ip_header->ip_src.s_addr = inet_addr(_local_ip.c_str()); // Source IP address
    packet.ip_header->ip_dst.s_addr = packet.destination.sin_addr.s_addr; // Destination IP

    PrepareTcpHeader(packet);
    packet.ip_header->ip_sum = utils::GetChecksum((unsigned short*)packet.data, ntohs(packet.ip_header->ip_len));
}

void TcpSession::PrepareTcpHeader(Packet& packet)
{
    // Prepare the TCP header
    packet.tcp_header->th_sport = htons(12345);  // Source port
    packet.tcp_header->th_dport = packet.destination.sin_port; // Destination port
    packet.tcp_header->th_seq   = htonl(0);  // Sequence number (initial)
    packet.tcp_header->th_ack   = 0;  // No acknowledgment number for SYN
    packet.tcp_header->th_off   = 5;  // TCP header length (5 x 4 = 20 bytes)
    packet.tcp_header->th_flags = TH_SYN;  // SYN flag to initiate connection
    packet.tcp_header->th_win   = htons(5840);  // Window size
    packet.tcp_header->th_sum   = 0;  // TCP checksum
    packet.tcp_header->th_urp   = 0;  // No urgent pointer
}
