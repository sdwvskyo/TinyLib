#ifndef QUERY_SERVER_H
#define QUERY_SERVER_H

#include <tiny/TcpServer.h>
#include <tiny/ThreadPool.h>
#include "TextQuery.h"
#include <boost/noncopyable.hpp>


class QueryServer : boost::noncopyable
{
public:
	QueryServer(const Tiny::InetAddress &addr, const std::string &filename);
	
	void start();

private:
	void onConnction(const Tiny::TcpConnectionPtr &conn);
	void onMessage(const Tiny::TcpConnectionPtr &conn);
	void onClose(const Tiny::TcpConnectionPtr &conn);

	void runQuery(const std::string &word, const Tiny::TcpConnectionPtr &conn);

	Tiny::TcpServer server_;
	TextQuery query_;
	Tiny::ThreadPool pool_;
};




#endif	//QUERY_SERVER_H
