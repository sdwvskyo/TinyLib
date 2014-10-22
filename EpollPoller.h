#ifndef EPOLL_POLLER_H
#define EPOLL_POLLER_H

#include <boost/noncopyable.hpp>
#include <vector>
#include <map>
#include <sys/epoll.h>
#include "TcpConnection.h"

namespace Tiny
{

class EpollPoller
{
public:
	typedef TcpConnection::TcpConnectionCallback EpollCallback;

	explicit EpollPoller(int listenfd);
	~EpollPoller();

	void loop();	//启动epoll
	void unloop();	//关闭epoll

	void setConnectCallback(const EpollCallback &cb)
	{ onConnectCallback_ = cb; }
	void setMessageCallback(const EpollCallback &cb)
	{ onMessageCallback_ = cb; }
	void setCloseCallback(const EpollCallback &cb)
	{ onCloseCallback_ == cb; }

private:
	void waitEpollFd();	//循环
	void handleConnection();	//处理accept
	void handleMessage(int peerfd);	//处理msg

	const int epollfd_;
	const int listenfd_;
	bool isLooping_; 	//是否在运行

	typedef std::vector<struct epoll_event> EventList;
	EventList events_;	//保存活跃的fd

	typedef std::map<int, TcpConnectionPtr> ConnectionList;
	ConnectionList lists_;	//实现fd到conn的映射

	EpollCallback onConnectCallback_;
    EpollCallback onMessageCallback_;
    EpollCallback onCloseCallback_;

};


}

#endif	//EPOLL_POLLER_H