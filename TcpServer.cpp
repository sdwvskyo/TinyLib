#include "TcpServer.h"
#include <functional>
#include <signal.h>
#include <stdlib.h>

#define ERR_EXIT(m) \
    do { \
        perror(m);\
        exit(EXIT_FAILURE);\
    }while(0)

using namespace std::placeholders;

class IgnoreSigpipe
{
public:
	IgnoreSigpipe() {
		if (::signal(SIGPIPE, SIG_IGN) == SIG_ERR)
			ERR_EXIT("signal");
	}
};

//全局对象，系统初始化时必然处理SIGPIPE
IgnoreSigpipe initObj;  

namespace
{
int createSocketFd()
{
	int fd = ::socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1)
		ERR_EXIT("socket");
	return fd;
}

}


namespace Tiny
{

TcpServer::TcpServer(const InetAddress &addr)
	: sockfd_(createSocketFd()),
	  poller_(sockfd_.fd())
{
	sockfd_.setTcpNoDelay(false);
    sockfd_.setReusePort(true);
    sockfd_.setReuseAddr(true);
    sockfd_.setKeepAlive(false);

    sockfd_.bindAddress(addr);
    sockfd_.listen();
}

}
