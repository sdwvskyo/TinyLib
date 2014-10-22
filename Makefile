.PHONY:clean
CC=g++
CFLAGS=-Wall -g -std=c++11
BIN=libtiny.a
OBJS=MutexLock.o Condition.o \
	 Thread.o ThreadPool.o \
	 Timer.o TimerThread.o \
	 InetAddress.o Socket.o \
	 SocketIO.o TcpConnection.o \
	 PollPoller.o TcpServer.o \
	 Exception.o
$(BIN):$(OBJS)
	ar -crsv $@ $^
	chmod +x $@
	mkdir tiny
	cp *h tiny/
%.o:%.cpp
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -rf *.o core tiny
	rm -rf tiny/