#ifndef INET_ADDRESS_H
#define INET_ADDRESS_H

#include <string>
#include <netinet/in.h>

/*
 * InetAddress对sockaddr_in做了一些简单的包装
 */


typedef struct sockaddr SA;
typedef struct sockaddr_in SAI;

namespace Tiny
{

class InetAddress
{
public:
	explicit InetAddress(uint16_t port);
	InetAddress(const std::string &ip, uint16_t port);
	InetAddress(const struct sockaddr_in &addr);
	
	void setSockAddrInet(const struct sockaddr_in &addr)
	{ addr_ = addr; }
	const struct sockaddr_in *getSockAddrInet() const
	{ return &addr_; }

	std::string toIp() const;
	uint16_t toPort() const;

	std::string toIpPort() const;

private:
	struct sockaddr_in addr_;

};

}


#endif	//INET_ADDRESS_H




