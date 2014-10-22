#include "PollPoller.h"
#include <assert.h>
#include <errno.h>

#define ERR_EXIT(m) \
    do { \
        perror(m);\
        exit(EXIT_FAILURE);\
    }while(0)

using namespace Tiny;

PollPoller::PollPoller(int listenfd)
	: listenfd_(listenfd),
	  maxi_(0),
	  isLooping_(false)
{
	for (size_t ix = 0; ix != 2048; ++ix)
		events_[ix].fd = -1;
	events_[0].fd = listenfd;
	events_[0].events = POLLIN;
}

PollPoller::~PollPoller()
{

}

void PollPoller::poll()
{
	int nready;
	do {
		nready = ::poll(events_, maxi_ + 1, 5000);
	} while(nready == -1 && errno == EINTR);

	if (nready == -1) {
		ERR_EXIT("poll");
    }
    else if (nready == 0) {
        printf("poll time out..\n");
    }
    else {
        //处理listenfd
        if(events_[0].revents & POLLIN) {
            int peerfd = ::accept(listenfd_, NULL, NULL);
            if(peerfd == -1)
            ERR_EXIT("accept");
            handleConnection(peerfd);
        }

        //依次处理客户
        int i;
        for(i = 1; i <= maxi_; ++i) {
            int peerfd = events_[i].fd;
            if(peerfd == -1)
                continue;
            if(events_[i].revents & POLLIN) {
                char buf[1024] = {0};
                int nread = recv(peerfd, buf, sizeof buf, MSG_PEEK);
                if(nread == -1)
                    ERR_EXIT("recv");
                else if(nread == 0)
                    handleClose(i);
                else
                    handleMessage(peerfd);
            }
        }
    }
}

void PollPoller::handleConnection(int peerfd)
{
	//1.添加至event
    //2.map中新增一条
    //3.调用函数
    int i;
    for(i = 1; i < 2048; ++i) {
        if(events_[i].fd == -1) {
            events_[i].fd = peerfd;
            events_[i].events = POLLIN;
            if(i > maxi_)
                maxi_ = i;
            break;
        }
    }
    if(i == 2048) {
        fprintf(stderr, "too many clients\n");
        exit(EXIT_FAILURE);
    }

    TcpConnectionPtr conn(new TcpConnection(peerfd));
    conn->setConnectCallback(onConnectCallback_);
    conn->setMessageCallback(onMessageCallback_);
    conn->setCloseCallback(onCloseCallback_);

    std::pair<ConnectionList::iterator, bool> ret = lists_.insert(std::make_pair(peerfd, conn));
    assert(ret.second == true);
    (void)ret;
    //onConnectionCallback_(conn);
    conn->handleConnectCallback();
}

void PollPoller::handleMessage(int peerfd)
{
	ConnectionList::iterator it = lists_.find(peerfd);
    assert(it != lists_.end());
    //onMessageCallback_(it->second);
    it->second->handleMessageCallback();
}

void PollPoller::handleClose(int i)
{
	assert(i >= 0 && i < 2048);
    int peerfd = events_[i].fd;
    assert(peerfd != -1);
    events_[i].fd = -1;
    ConnectionList::iterator it = lists_.find(peerfd);
    assert(it != lists_.end());
    //onCloseCallback_(it->second);
    it->second->handleCloseCallback();

    lists_.erase(it);
}

void PollPoller::loop()
{
    isLooping_ = true;

    while (isLooping_) {
        poll();
    }
}

void PollPoller::unloop()
{
    isLooping_ = false;
}




