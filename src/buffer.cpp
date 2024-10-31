#include "buffer.h"

#include <errno.h>
#include <sys/uio.h>

// 从 fd 上读取数据 Poller工作在LT模式
ssize_t Buffer::readFd(int fd, int *saveErrno)
{
    char extrabuf[65536] = {0};
    iovec vec[2];
    const size_t writable = writeableBytes();
    vec[0].iov_base = begin() + writerIndex_;
    vec[0].iov_len = writable;

    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof(extrabuf);

    const int iovcnt = (writable < sizeof(extrabuf)) ? 2 : 1;
    const ssize_t n = ::readv(fd, vec, iovcnt);

    if (n < 0)
    {
        *saveErrno = errno;
    }
    else if (n <= writable)
    {
        writerIndex_ += n;
    }
    else
    {
        writerIndex_ = buffer_.size();
        append(extrabuf, n - writable); // writerIndex_ 开始写
    }
    return n;
}

ssize_t Buffer::writeFd(int fd, int *saveErrno)
{
    // TcpConnection连接二 8:50
}