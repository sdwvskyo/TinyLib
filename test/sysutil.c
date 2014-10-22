#include "sysutil.h"
#include <sys/ioctl.h>
#include <net/if.h>
#include <signal.h>

#define ERR_EXIT(m) \
    do { \
        perror(m);\
        exit(EXIT_FAILURE);\
    }while(0)

void nano_sleep(double val)
{
    struct timespec tv;
    tv.tv_sec = val;
    tv.tv_nsec = (val - tv.tv_sec) * 1000 * 1000 * 1000;

    int ret;
    do {
        ret = nanosleep(&tv, &tv);
    } while (ret == -1 && errno != EINTR);
}

ssize_t readn(int fd, void *buf, size_t count)
{
    size_t nleft = count;
    char *bufp = (char *)buf;
    while(nleft > 0) {
        size_t nread = read(fd, bufp, nleft);
        if (nread == -1) {
            if (errno == EINTR)
                continue;
            return -1;
        }
        else if (nread == 0)
            break;

        bufp += nread;
        nleft -= nread;
    } 
    return count - nleft;
}

ssize_t writen(int fd, const void *buf, size_t count)
{
    size_t nleft = count;
    char *bufp = (char *)buf;
    while(nleft > 0) {
        size_t nwrite = write(fd, bufp, nleft);
        if (nwrite <= 0) {
            if (nwrite == -1 && errno == EINTR)
                continue;
            return -1;
        }
        bufp += nwrite;
        nleft -= nwrite;
    }
    return count;
}

//预览内核缓冲区数据
ssize_t recv_peek(int sockfd, void *buf, size_t len)
{
    int nread;
    do {
        nread = recv(sockfd, buf, len, MSG_PEEK);
    } while(nread == -1 && errno == EINTR);

    return nread;
}

ssize_t readline(int fd, void *buf, size_t maxlen)
{
    size_t nleft = maxlen - 1;
    char *bufp = (char *)buf;
    ssize_t total = 0;
    ssize_t nread;

    while (nleft > 0) {
        //recv+peek为预读取
        nread = recv_peek(fd, bufp, nleft);
        if (nread <= 0)
            return nread;

        //检查此段内有无\n
        int i;
        for (i = 0; i < nread; ++i) {
            if (bufp[i] == '\n') {
                //找到\n后读取退出
                size_t nsize = i + 1;
                if (readn(fd, bufp, nsize) != nsize)
                    return -1;
                bufp += nsize;
                *bufp = '\0';
                total += nsize;
                return total;
            }
        }

        //到此步说明没有找到\n，继续
        if (readn(fd, bufp, nread) != nread)
            return -1;
        bufp += nread;
        total += nread;
        nleft -= nread;
    }
    //读取到最大长度，返回
    *bufp = '\0';
    return maxlen - 1;
}

ssize_t readLine_slow(int fd, void *buf, size_t maxlen)
{
    char c;
    size_t nleft = maxlen - 1;
    char *bufp = (char *)buf;
    while(nleft > 0 && c != '\n') {
        ssize_t nread = read(fd, &c, 1);
        if (nread == -1) {
            if (errno == EINTR)
                continue;
            return -1;
        }
        else if (nread == 0)
            break;

        *bufp++ = c;
        nleft--;
    }

    return maxlen - nleft - 1;
}

void send_int32(int sockfd, int32_t val)
{
    //先转化为网络字节序
    int32_t tmp = htonl(val);
    if (writen(sockfd, &tmp, sizeof tmp) != sizeof(int32_t))
        ERR_EXIT("send_int32");
}

int32_t recv_int32(int sockfd)
{
    int32_t tmp;
    int nread = readn(sockfd, &tmp, sizeof(int32_t));
    if (nread == 0) //EOF 对方close套接字
        return 0;
    else if (nread != sizeof(int32_t))
        ERR_EXIT("recv_int32");

    return ntohl(tmp);  //转化为主机字节序
}

//地址复用
void set_reuseaddr(int sockfd, int optval)
{
    int on = (optval != 0) ? 1 : 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
        ERR_EXIT("setsockopt SO_REUSEADDR");
}

//端口复用
void set_reuseport(int sockfd, int optval)
{
#ifdef SO_REUSEPORT
    int on = (optval != 0) ? 1 : 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on)) < 0)
        ERR_EXIT("setsockopt SO_REUSEPORT");
#else
    fprintf(stderr, "SO_REUSEPORT is not supported.\n");
#endif //SO_REUSEPORT
}

//设置nagle算法是否可用
void set_tcpnodelay(int sockfd, int optval)
{
    int on = (optval != 0) ? 1 : 0;
    if (setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on)) < 0)
        ERR_EXIT("setsockopt TCP_NODELAY");
}

//是否开始tcpAlive机制
void set_keepalive(int sockfd, int optval)
{
    int on = (optval != 0) ? 1 : 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &on, sizeof(on)) < 0)
        ERR_EXIT("setsockopt SO_KEEPALIVE");
}

static void handle_sigpipe()
{
    if (signal(SIGPIPE, SIG_IGN) == SIG_IGN)
        ERR_EXIT("signal");
}

int tcp_server(const char *host, uint16_t port)
{
    //处理PIPE信号
    handle_sigpipe();

    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1)
        ERR_EXIT("socket");

    set_reuseaddr(listenfd, 1);
    set_reuseport(listenfd, 1);
    set_keepalive(listenfd, 0);
    set_tcpnodelay(listenfd, 0);

    SAI addr;
    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (host == NULL) {
        addr.sin_addr.s_addr = INADDR_ANY;
    } 
    else if (inet_aton(host, &addr.sin_addr) == 0) {
        struct hostent *hp = gethostbyname(host);
        if (hp == NULL)
            ERR_EXIT("gethostbyname");
        addr.sin_addr = *(struct in_addr *)hp->h_addr;
    }

    if (bind(listenfd, (SA *)&addr, sizeof addr) == -1)
        ERR_EXIT("bind");

    if (listen(listenfd, SOMAXCONN) == -1)
        ERR_EXIT("listen");

    return listenfd;
}

int tcp_client(uint16_t port)
{
    int peerfd = socket(AF_INET, SOCK_STREAM, 0);
    if (peerfd == -1)
        ERR_EXIT("socket");

    if (port == 0)
        return peerfd;

    SAI addr;
    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(get_local_ip());
    if (bind(peerfd, (SA *)&addr, sizeof addr) == -1)
        ERR_EXIT("bind");
    return peerfd;
}


void connect_host(int sockfd, const char *des_host, uint16_t des_port)
{
    if (des_host == NULL) {
        fprintf(stderr, "des_host can not be NULL\n");
        exit(EXIT_FAILURE);
    }

    SAI seraddr;
    memset(&seraddr, 0, sizeof seraddr);
    seraddr.sin_family = AF_INET;
    seraddr.sin_port = htons(des_port);
    if (inet_aton(des_host, &seraddr.sin_addr) == 0) {
        struct hostent *hp = gethostbyname(des_host);
        if (hp == NULL)
            ERR_EXIT("gethostbyname");
        seraddr.sin_addr = *(struct in_addr *)hp->h_addr;
    }

    if (connect(sockfd, (SA *)&seraddr, sizeof seraddr) == -1)
        ERR_EXIT("connect");
}

//获取点分十进制的Ip字符串
const char *get_local_ip()
{
    static char ip[16];

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
        ERR_EXIT("socket");

    struct ifreq req;
    bzero(&req, sizeof(struct ifreq));
    strcpy(req.ifr_name, "eth0");
    if (ioctl(sockfd, SIOCGIFADDR, &req) == -1)
        ERR_EXIT("ioctl");

    SAI *host = (SAI *)&req.ifr_addr;
    strcpy(ip, inet_ntoa(host->sin_addr));
    close(sockfd);

    return ip;
}

SAI get_peer_addr(int peerfd)
{
    SAI addr;
    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    socklen_t len = sizeof addr;
    if (getpeername(peerfd, (SA *)&addr, &len) == -1)
        ERR_EXIT("getpeername");

    return addr;
}

SAI get_local_name(int sockfd)
{
    SAI addr;
    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    socklen_t len = sizeof addr;
    if (getsockname(sockfd, (SA *)&addr, &len) == -1)
        ERR_EXIT("getsockname");

    return addr;
}

const char *get_addr_ip(const SAI *addr)
{
    return inet_ntoa(addr->sin_addr);
}

uint16_t get_addr_port(const SAI *addr)
{
    return ntohs(addr->sin_port);
}

const char *get_tcp_info(int peerfd)
{
    static char info[1024] = {0};

    SAI localaddr = get_local_name(peerfd);
    SAI peeraddr = get_peer_addr(peerfd);
    snprintf(info, sizeof info, "%s:%d->%s:%d",
        get_addr_ip(&localaddr),
        get_addr_port(&localaddr),
        get_addr_ip(&peeraddr),
        get_addr_port(&peeraddr));

    return info;
}

void send_msg_with_len(int sockfd, const void *buf, size_t count)
{
    send_int32(sockfd, count);
    if (writen(sockfd, buf, count) != count)
        ERR_EXIT("send_msg_with_len");
}

//最后一个参数，指的是buffer的size，而不是用户要求的数目
size_t recv_msg_with_len(int sockfd, void *buf, size_t bufsize)
{
    int32_t len = recv_int32(sockfd);
    if (len == 0)
        return 0;
    else if (len > (int32_t)bufsize) {
        fprintf(stderr, "bufsize is not enough\n");
        exit(EXIT_FAILURE);
    }

    ssize_t nread = readn(sockfd, buf, len);
    if (nread == -1)
        ERR_EXIT("recv_msg_with_len");
    else if ((size_t)nread < len)
        return 0;

    return len;
}


