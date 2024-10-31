#pragma once

#include "poller.h"
#include "timestamp.h"

#include <vector>
#include <sys/epoll.h>

class EpollPoller : public Poller
{
public:
    explicit EpollPoller(EventLoop *loop);
    ~EpollPoller() override;

    // 重写基类Poller的抽象方法
    Timestamp poll(int timeoutMs, ChannelList *activeChannels) override;
    void updateChannel(Channel *channel) override;
    void removeChannel(Channel *channel) override;

private:
    static const int kInitEventListSize = 16;
    // 填写活跃的连接
    void fillActiveChannnels(int numEvents, ChannelList *activeChannels) const;

    // 更新channel通道
    void update(int operation, Channel *channel);
    using EventList = std::vector<epoll_event>;

    int epollfd_;
    EventList events_;
};