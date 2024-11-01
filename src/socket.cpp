#include "socket.h"
#include "logger.h"
#include "inetaddress.h"

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/tcp.h>

#include <string.h>

Socket::~Socket()
{
    close(sockfd_);
}

void Socket::bindAddress(const InetAddress &localAddr)
{
    if (0 != ::bind(sockfd_, (sockaddr*)localAddr.getSockAddr(), sizeof(sockaddr_in)))
    {
        LOG_FATAL("socket bind fd %d failed\n", sockfd_);
    }
}

void Socket::listen()
{
    if (0 != ::listen(sockfd_, 1024))
    {
        LOG_FATAL("listen sockfd:%d failed\n", sockfd_);
    }
}

int Socket::accept(InetAddress *peerAddr)
{
    sockaddr_in addr;
    socklen_t len;
    bzero(&addr, sizeof(addr));
    int newFd = ::accept(sockfd_, (sockaddr*)&addr, &len);
    if (newFd >= 0)
    {
        peerAddr->setSockAddr(addr);
    }
    return newFd;
}

void Socket::shutdownWrite()
{
    if (::shutdown(sockfd_, SHUT_WR) < 0)
    {
        LOG_ERROR("shutdownWrite error");
    }
}

void Socket::setTcpNoDelay(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));
}

void Socket::setReuseAddr(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}

void Socket::setReusePort(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
}

void Socket::setKeepAlive(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));
}