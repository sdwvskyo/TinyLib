.PHONY:clean
CC=g++
CFLAGS=-Wall -g -std=c++0x
BIN=test.exe
OBJS=main.o QueryServer.o TextQuery.o
LIBS=-ltiny -lpthread
$(BIN):$(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)
%.o:%.cpp
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f *.o $(BIN) core
