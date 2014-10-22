#ifndef SYSUTIL_H_
#define SYSUTIL_H_ 

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/tcp.h>


typedef struct sockaddr SA;
typedef struct sockaddr_in SAI;

//定时器
void nano_sleep(double val);

//读写函数
ssize_t readn(int fd, void *buf, size_t count);
ssize_t writen(int fd, const void *buf, size_t count);
ssize_t readline(int fd, void *buf, size_t maxlen);

void send_int32(int sockfd, int32_t val);
int32_t recv_int32(int sockfd);

//地址复用
void set_reuseaddr(int sockfd, int optval);
//端口复用
void set_reuseport(int sockfd, int optval);
//设置nagle算法是否可用
void set_tcpnodelay(int sockfd, int optval);
//是否开始tcpAlive机制
void set_keepalive(int sockfd, int optval);

//创建serversocket，返回listenfd
int tcp_server(const char *host, uint16_t port);
//创建clientsocket，返回peerfd
int tcp_client(uint16_t port);
//与server进行连接
void connect_host(int sockfd, const char *des_host, uint16_t des_port);

//获取本地IP
const char *get_local_ip();

SAI get_peer_addr(int peerfd);
SAI get_local_addr(int sockfd);
const char *get_addr_ip(const SAI *addr);
uint16_t get_addr_port(const SAI *addr);
const char *get_tcp_info(int peerfd);

void send_msg_with_len(int sockfd, const void *buf, size_t count);
size_t recv_msg_with_len(int sockfd, void *buf, size_t buflen);


#endif	//SYSUTIL_H_
