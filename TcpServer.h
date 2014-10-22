#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <boost/noncopyable.hpp>
#include "PollPoller.h"
#include "Socket.h"

namespace Tiny
{

class TcpServer : boost::noncopyable
{
public:
	typedef PollPoller::PollerCallback TcpServerCallback;

	explicit TcpServer(const InetAddress &addr);
	
	void start() { poller_.loop(); }

	void setConnectCallback(const TcpServerCallback &cb)
    { poller_.setConnectionCallback(cb); }
    void setMessageCallback(const TcpServerCallback &cb)
    { poller_.setMessageCallback(cb); }
    void setCloseCallback(const TcpServerCallback &cb)
    { poller_.setCloseCallback(cb); }


private:
	Socket sockfd_;
	PollPoller poller_;
};

}

#endif	//TCP_SERVER_H