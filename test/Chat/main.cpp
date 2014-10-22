#include "ChatServer.h"

using namespace Tiny;

int main(int argc, char const *argv[])
{
	ChatServer server(InetAddress("192.168.0.114", 8888));
	server.start();
	return 0;
}