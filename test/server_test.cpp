#include <tiny/TcpServer.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

using namespace Tiny;

void onConnection(const TcpConnectionPtr &conn)
{
	printf("%s\n", conn->toString().c_str());
	conn->send("Hello, welcome to Chat Server!\n");
}

void onMessage(const TcpConnectionPtr &conn)
{
	std::string s(conn->receive());
	conn->send(s);
}

void onClose(const TcpConnectionPtr &conn)
{
	printf("%s close\n", conn->toString().c_str());
}

int main(int argc, char const *argv[])
{
	InetAddress addr("192.168.0.114", 8888);
	TcpServer server(addr);

	server.setConnectCallback(&onConnection);
	server.setMessageCallback(&onMessage);
	server.setCloseCallback(&onClose);

	server.start();

	return 0;
}
