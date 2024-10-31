#pragma once 

#include "noncopyable.h"
#include "timestamp.h"

#include <functional>
#include <memory>

class EventLoop;

/*
* Channel 通道，封装了sockfd 和 感兴趣的event， 如 EPOLLIN、EPOLLOUT事件
* 还绑定了poller返回的具体事件
*/
class Channel : private noncopyable
{
public:
    using EventCallBack = std::function<void()>;
    using ReadEventCallBack = std::function<void(Timestamp)>;

    Channel(EventLoop* loop, int fd);
    ~Channel();

    // fd 得到 poller 通知以后，处理事件
    void handleEvent(Timestamp recevieTime);

    // 设置回调函数对象
    void setReadCallback(ReadEventCallBack cb) { readCallback_ = std::move(cb); }
    void setWriteCallback(EventCallBack cb) { writeCallback_ = std::move(cb); }
    void setCloseCallback(EventCallBack cb) { closeCallback_ = std::move(cb); }
    void setErrorCallback(EventCallBack cb) { errorCallback_ = std::move(cb); }

    // 防止当Channel被手动remove掉，Channel还在执行回调操作
    void tie(const std::shared_ptr<void>&);

    int fd() const { return fd_; }
    int events() const { return events_; }
    void set_revents(int revt) { revents_ = revt; }

    // 设置 fd 相应的事件状态
    void enableReading() { events_ |= kReadEvent; update(); }
    void disableReading() { events_ &= ~kWriteEvent; update(); }
    void enableWriting() { events_ |= kWriteEvent; update(); }
    void disableWriting() { events_ &= ~kWriteEvent; update(); }
    void disableAll() { events_ = kNoneEvent; update();}

    // 返回fd当前的事件状态
    bool isNoneEvent() const { return events_ == kNoneEvent; }
    bool isWriting() const { return events_ & kWriteEvent; }
    bool isReading() const { return events_ & kReadEvent; }

    int index() { return index_; }
    void set_index(int idx) { index_ = idx; }

    EventLoop* ownerLoop() { return loop_; }
    void remove();

private:
    void update();
    void handleEventWithGuard(Timestamp receiveTime);

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop* loop_;  // 事件循环
    const int fd_;  // fd, poller监听的对象
    int events_;  // 注册fd感兴趣的事件
    int revents_;  // poller返回的具体发生的事件
    int index_;

    std::weak_ptr<void> tie_;
    bool tied_;

    // 因为channel通道里面能够获知fd最终发生的具体事件的revents,
    // 所以它负责调用具体事件的回调操作
    ReadEventCallBack readCallback_;
    EventCallBack writeCallback_;
    EventCallBack closeCallback_;
    EventCallBack errorCallback_;
};