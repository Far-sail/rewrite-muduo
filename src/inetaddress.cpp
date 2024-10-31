#include "inetaddress.h"
#include <string.h>
#include <iostream>

// 构造函数，传入端口和ip，用于初始化成员sockaddr_in
InetAddress::InetAddress(uint16_t port, std::string ip)
{
    // 清零
    bzero(&addr_, sizeof addr_);
    // 初始化 协议族， 端口 和 ip 是网络类型
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    addr_.sin_addr.s_addr = inet_addr(ip.c_str());
}

// 获取 ip
std::string InetAddress::toIp() const
{
    char buf[64] = {0};
    ::inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof buf);
    return std::string(buf);
}

// 获取 ip:port
std::string InetAddress::toIpPort() const
{
    char buf[64] = {0};
    ::inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof buf);
    size_t end = strlen(buf); // 不计算\0
    uint16_t port = ntohs(addr_.sin_port);
    sprintf(buf+end, ":%u", port);
    return std::string(buf);
}

uint16_t InetAddress::toPort() const
{
    return ntohs(addr_.sin_port);
}

// int main()
// {
//     InetAddress addr(8000, "192.168.1.2");
//     std::cout << "ip:" << addr.toIp() << ", port:" << addr.toPort() <<
//         " ," << addr.toIpPort() << std::endl;
//     return 0;
// }