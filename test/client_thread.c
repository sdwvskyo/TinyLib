#include "sysutil.h"
#include <pthread.h>

#define ERR_EXIT(m) \
    do { \
        perror(m);\
        exit(EXIT_FAILURE);\
    }while(0);

void *thread_func(void *arg);
void do_service(int sockfd);

int main(int argc, char const *argv[])
{
	int peerfd = tcp_client(0);
    connect_host(peerfd, "192.168.0.114", 8888);

    printf("%s connected\n", get_tcp_info(peerfd));

    pthread_t tid;
    if (pthread_create(&tid, NULL, thread_func, (void *)&peerfd)) {
    	perror("pthread_create");
    	exit(EXIT_FAILURE);
    }

    char sendbuf[1024] = {0};
    while (fgets(sendbuf, sizeof sendbuf, stdin)) {
    	writen(peerfd, sendbuf, strlen(sendbuf));
    	memset(sendbuf, 0, sizeof sendbuf);
    }

    close(peerfd);
    printf("log off\n");
	return 0;
}

void *thread_func(void *arg)
{
	pthread_detach(pthread_self());
	int sockfd = *(int *)arg;
	do_service(sockfd);
	return NULL;
}

void do_service(int sockfd)
{
	char recvbuf[1024] = {0};
	while (1) {
		int nread = readline(sockfd, recvbuf, sizeof recvbuf);
	    if (nread == 0) {
	        printf("close....\n");
	        exit(EXIT_FAILURE);
	    }
	    printf("%s\n", recvbuf);
	    memset(recvbuf, 0, sizeof recvbuf);
	}
}

