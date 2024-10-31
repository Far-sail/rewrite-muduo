#pragma once 
#include <functional>
#include <vector>
#include <atomic>
#include <memory>
#include <mutex>

#include "noncopyable.h"
#include "timestamp.h"
#include "currentthread.h"

class Channel;
class Poller;
// 事件循环类 主要包括了两个大模块 Channel Poller (epoll 的抽象)
class EventLoop : noncopyable
{
public:
    using Functor = std::function<void()>;

    EventLoop();
    ~EventLoop();

    void loop();
    void quit();

    Timestamp pollReturnTime() const { return pollReturnTime_; }

    void runInLoop(Functor cb);
    void queueInLoop(Functor cb);

    void wakeup();

    void updateChannel(Channel *channel);
    void removeChannel(Channel *channel);
    bool hasChannel(Channel *channel);

    bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); }
private:
    void handleRead();
    void doPendingFunctors();

    using ChannelList = std::vector<Channel*>;

    std::atomic_bool looping_;
    std::atomic_bool quit_;
   

    const pid_t threadId_; // 记录当前线程的tid
    Timestamp pollReturnTime_; // poller返回发生事件的时间点
    std::unique_ptr<Poller> poller_;

    int wakeupFd_;  // 通过MainLoop获取一个新用户的channel，通过轮询算法选择一个subloop
    std::unique_ptr<Channel> wakeupChannel_;

    ChannelList activeChannels_;

    std::atomic_bool callingPendingFunctors_; // 标识当前loop是否有需要执行的操作
    std::vector<Functor> pendingFunctors_; // 存储Loop需要执行的所有回调操作
    std::mutex mutex_; // 互斥锁，用来保护上面vector容器的线程安全操作
};