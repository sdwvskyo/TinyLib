#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include <boost/noncopyable.hpp>
#include <memory>
#include <functional>
#include "Socket.h"
#include "SocketIO.h"

namespace Tiny
{

class TcpConnection;
typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

class TcpConnection : boost::noncopyable,
					  public std::enable_shared_from_this<TcpConnection>
{
public:
	typedef std::function<void (const TcpConnectionPtr &)> TcpConnectionCallback;

	explicit TcpConnection(int sockfd);
	~TcpConnection();

	void setConnectCallback(const TcpConnectionCallback &cb)
	{ onConnectCallback_ = cb; }
	void setMessageCallback(const TcpConnectionCallback &cb)
	{ onMessageCallback_ = cb; }
	void setCloseCallback(const TcpConnectionCallback &cb)
	{ onCloseCallback_ = cb; }

	void handleConnectCallback();
	void handleMessageCallback();
	void handleCloseCallback();

	void shutdown() { sockfd_.shutdownWrite(); isShutdownWrite_ = true; }

	ssize_t readn(char *buf, size_t count);
	ssize_t writen(const char *buf, size_t count);
	ssize_t readLine(char *usrbuf, size_t maxlen);
	std::string receive();
	void send(const std::string &s);


	const InetAddress &getLocalAddr() const { return localAddr_; }
	const InetAddress &getPeerAddr() const { return peerAddr_; }

	std::string toString() const;

private:
	Socket sockfd_;
	SocketIO sockIO_;	//该连接的读写缓冲区
	const InetAddress localAddr_;
	const InetAddress peerAddr_;
	bool isShutdownWrite_;

	TcpConnectionCallback onConnectCallback_;	//连接建立时的回调
	TcpConnectionCallback onMessageCallback_;	//消息到达
	TcpConnectionCallback onCloseCallback_;		//连接关闭

};


}


#endif	//TCP_CONNECTION_H