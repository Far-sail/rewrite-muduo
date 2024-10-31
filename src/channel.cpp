#include "channel.h"
#include "eventloop.h"
#include "logger.h"

#include <sys/epoll.h>

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;

Channel::Channel(EventLoop *loop, int fd)
    : loop_(loop), fd_(fd), events_(0), revents_(0), index_(-1), tied_(false)
{
}

Channel::~Channel()
{
}

void Channel::tie(const std::shared_ptr<void> &obj)
{
    tie_ = obj;
    tied_ = true;
}

/*
* 当改变Channel所表示的fd的events事件后，update负责在poller里面更改fd相应的事件 epoll_ctl
* EventLoop => ChannelList Poller
*/
void Channel::update()
{
    // 通过channel 所属的EventLoop, 调用poller的相应方法，注册fd的events事件
    // todo_list
    // loop_->updateChannel(this);
}

// 在EventLoop中删除当前的Channel
void Channel::remove()
{
    // todo_list
    // loop_->removeChannel(this);
}


void Channel::handleEvent(Timestamp recevieTime)
{
    if (tied_)
    {
        std::shared_ptr<void> guard = tie_.lock();
        if (guard)
        {
            handleEventWithGuard(recevieTime);
        }
    }
    else
    {
        handleEventWithGuard(recevieTime);
    }
}


void Channel::handleEventWithGuard(Timestamp receiveTime)
{
    LOG_INFO("channel handleEvent revents:%d\n", revents_);
    // 连接关闭 关闭事件，且socket非读就绪
    if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN))
    {
        if (closeCallback_)
        {
            closeCallback_();
        }
    }

    // 错误事件
    if (revents_ & EPOLLERR)
    {
        if (errorCallback_)
        {
            errorCallback_();
        }
    }

    // 可读事件
    if (revents_ & (EPOLLIN | EPOLLPRI))
    {
        if (readCallback_)
        {
            readCallback_(receiveTime);
        }
    }

    // 可写事件
    if (revents_ & EPOLLOUT)
    {
        if (writeCallback_)
        {
            writeCallback_();
        }
    }
}