#include "QueryServer.h"
#include <iostream>
#include <string>
using namespace std;
using namespace Tiny;

int main(int argc, const char *argv[])
{
    QueryServer server(InetAddress(8888), "The_Holy_Bible.txt");
    server.start();
    return 0;
}
