#pragma once

#include <netinet/ip.h>
#include <Kernel/string.h>

struct Packet
{
    struct sockaddr_in  destination;
    struct ip*          ip_header;
    struct tcphdr*      tcp_header;
    char                data[4096];
    Packet()
    {
        memset(data, 0, sizeof(data));
        ip_header  = (struct ip*) data;
        tcp_header = (struct tcphdr*)(data + sizeof(struct ip));
    }
};