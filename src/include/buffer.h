#pragma once

#include <vector>
#include <iostream>
#include <algorithm>

class Buffer
{
public:
    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 1024;

    explicit Buffer(size_t initialSize = kInitialSize)
        : buffer_(kCheapPrepend + initialSize)
        , readerIndex_(kCheapPrepend)
        , writerIndex_(kCheapPrepend)
        {
        }
    
    size_t readableBytes() const 
    {
        return writerIndex_ - readerIndex_;
    }

    size_t writeableBytes() const
    {
        return buffer_.size() - writerIndex_;
    }

    size_t prependableBytes() const
    {
        return readerIndex_;
    }

    const char* peek() const
    {
        return begin() + readerIndex_;
    }

    void retrieve(size_t len)
    {
        if (len < readableBytes())
        {
            readerIndex_ += len;
        }
        else
        {
            retrieveAll();
        }
    }
    // 复位
    void retrieveAll()
    {
        readerIndex_ = writerIndex_ = kCheapPrepend;
    }
    // 需要len长度的内存，但此时缓存中可用空间所有加起来也不够len，整理内存碎片
    void makeSpace(size_t len)
    {
        if (writeableBytes() + prependableBytes() < len + kCheapPrepend)
        {
            buffer_.resize(writerIndex_ + len);
        }
        else
        {
            size_t readable = readableBytes();
            std::copy(begin() + readerIndex_,
                    begin() + writerIndex_,
                    begin() + kCheapPrepend);
            readerIndex_ = kCheapPrepend;
            writerIndex_ = readerIndex_ + readable;
        }
    }

    std::string retrieveAllAsString()
    {
        return retrieveAsString(readableBytes());
    }

    std::string retrieveAsString(size_t len)
    {
        std::string result(peek(), len);
        retrieve(len);
        return result;
    }

    void ensureWriteableBytes(size_t len)
    {
        if (writeableBytes() < len)
        {
            makeSpace(len); // 扩容函数
        }
    }

    void append(const char *data, size_t len)
    {
        ensureWriteableBytes(len);
        std::copy(data, data+len, beginWrite());
        writerIndex_ += len;
    }

    char* beginWrite()
    {
        return begin() + writerIndex_;
    }

    // 通过 fd 上读取数据
    ssize_t readFd(int fd, int *saveErrno);
    // 通过 fd 上写数据
    ssize_t writeFd(int fd, int *saveErrno);
private:
    char* begin()
    {
        return &*buffer_.begin(); // vector首元素的地址
    }
    const char* begin() const
    {
        return &*buffer_.begin();
    }
    std::vector<char> buffer_;
    size_t readerIndex_;
    size_t writerIndex_;
};