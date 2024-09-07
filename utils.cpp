#include "utils.hpp"

#include <ifaddrs.h>
#include <netdb.h>

namespace utils
{
    std::string GetLocalIp() 
    {
        struct ifaddrs *ifaddr, *ifa;
        char host[NI_MAXHOST];
        std::string local_ip;

        // Get a list of network interfaces
        if (getifaddrs(&ifaddr) == -1) {
            perror("getifaddrs");
            return "";
        }

        // Iterate over the list of interfaces
        for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
            if (ifa->ifa_addr == nullptr) continue;

            // We are only interested in IPv4 addresses
            if (ifa->ifa_addr->sa_family == AF_INET) {
                int s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host, NI_MAXHOST, nullptr, 0, NI_NUMERICHOST);
                if (s == 0) {
                    // Ignore the loopback interface (127.0.0.1)
                    if (std::string(host) != "127.0.0.1") {
                        local_ip = std::string(host);
                        break;
                    }
                }
            }
        }

        freeifaddrs(ifaddr);
        if (local_ip.empty()) {
            return "";
        }
        return local_ip;
    }

    unsigned short GetChecksum(void* b, int len)
    {
        unsigned short* buf = static_cast<unsigned short*>(b);
        unsigned int sum = 0;
        unsigned short result;

        for (sum = 0; len > 1; len -= 2) {
            sum += *buf++;
        }
        if (len == 1) {
            sum += *(unsigned char*)buf;
        }
        sum = (sum >> 16) + (sum & 0xFFFF);
        sum += (sum >> 16);
        result = ~sum;
        return result;
    }

}