#include "ChatServer.h"

using namespace Tiny;
using namespace std;
using namespace std::placeholders;

ChatServer::ChatServer(const Tiny::InetAddress &addr)
	: server_(addr)
{
	server_.setConnectCallback(bind(&ChatServer::onConnection, 
									this,
									_1));
	server_.setMessageCallback(bind(&ChatServer::onMessage, 
									this,
									_1));
	server_.setCloseCallback(bind(&ChatServer::onClose, 
									this,
									_1));
}


void ChatServer::onConnection(const Tiny::TcpConnectionPtr &conn)
{
	printf("%s connect.\n", conn->toString().c_str());

	if (clients_.size() >= 3) {
		conn->send("Sorry, too many clients\r\n");
		conn->shutdown();
	} 
	else {
		clients_.insert(conn);
		conn->send("Hello, welcome to Chat Server.\r\n");
	}
}

void ChatServer::onMessage(const Tiny::TcpConnectionPtr &conn)
{
	std::string s(conn->receive());
	std::string msg = conn->getPeerAddr().toIpPort() + " says: " + s;

	for (auto it = clients_.begin(); it != clients_.end(); ++it)
		if (*it != conn)
			(*it)->send(msg);
}

void ChatServer::onClose(const Tiny::TcpConnectionPtr &conn)
{
	printf("%s close\n", conn->toString().c_str());

    clients_.erase(conn);
}
