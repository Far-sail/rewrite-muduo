#include "eventloopthread.h"
#include "eventloop.h"

#include <memory>

EventLoopThread::EventLoopThread(const ThreadInitCallback &cb,
                                 const std::string &name)
                                 : loop_(nullptr)
                                 , exiting_(false)
                                 , thread_(std::bind(&EventLoopThread::threadFunc, this), name)
                                 , mutex_()
                                 , cond_()
                                 , callback_(cb)
{

}
EventLoopThread::~EventLoopThread()
{
    exiting_ = true;
    if (loop_ != nullptr)
    {
        loop_->quit();
        thread_.join();
    }
}

// 启动一个新线程
EventLoop *EventLoopThread::startLoop()
{
    thread_.start(); // 启动底层的新线程

    EventLoop *loop = nullptr;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        while (loop_ == nullptr)
        {
            cond_.wait(lock);
        }
        loop = loop_;
    }
    return loop;
}

void EventLoopThread::threadFunc()
{
    EventLoop loop; // 创建一个独立的eventloop, 和上面的线程一一对应

    if (callback_)
    {
        callback_(&loop);
    }

    {
        std::unique_lock<std::mutex> lock(mutex_);
        loop_ = &loop;
        cond_.notify_one();
    }

    loop.loop();
}