#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H

#include <tiny/TcpServer.h>
#include <set>

class ChatServer : boost::noncopyable
{
public:
	explicit ChatServer(const Tiny::InetAddress &addr);
	
	void start() { server_.start(); }
private:
	void onConnection(const Tiny::TcpConnectionPtr &conn);
	void onMessage(const Tiny::TcpConnectionPtr &conn);
	void onClose(const Tiny::TcpConnectionPtr &conn);

	Tiny::TcpServer server_;
	std::set<Tiny::TcpConnectionPtr> clients_;
};


#endif	//CHAT_SERVER_H