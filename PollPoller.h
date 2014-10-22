#ifndef POLL_POLLER_H
#define POLL_POLLER_H

#include <boost/noncopyable.hpp>
#include "TcpConnection.h"
#include <map>
#include <vector>
#include <functional>
#include <poll.h>

namespace Tiny
{

class PollPoller : boost::noncopyable
{
public:
	typedef TcpConnection::TcpConnectionCallback PollerCallback;

	explicit PollPoller(int listenfd);
	~PollPoller();

	void loop();	//启动poll
	void unloop();	//关闭poll

	void setConnectionCallback(const PollerCallback &cb)
    { onConnectCallback_ = cb; }
    void setMessageCallback(const PollerCallback &cb)
    { onMessageCallback_ = cb; }
    void setCloseCallback(const PollerCallback &cb)
    { onCloseCallback_ = cb; }

private:
	void poll();
	void handleConnection(int peerfd);
	void handleMessage(int peerfd);
	void handleClose(int i);

	int listenfd_;
	int maxi_;
	bool isLooping_;
	struct pollfd events_[2048];

	typedef std::map<int, TcpConnectionPtr> ConnectionList;
	ConnectionList lists_;	//从fd到TcpConnection的映射

	PollerCallback onConnectCallback_;
	PollerCallback onMessageCallback_;
	PollerCallback onCloseCallback_;

};

}

#endif	//POLL_POLLER_H