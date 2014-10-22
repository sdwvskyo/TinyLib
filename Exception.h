#ifndef EXCEPTION_H_
#define EXCEPTION_H_ 

#include <exception>
#include <string>

namespace Tiny
{
class Exception : public std::exception
{
public:
	explicit Exception(const char *what);
	explicit Exception(const std::string &what);
	virtual ~Exception() throw(); 		//不抛出异常
	virtual const char * what() const throw();	//打印异常信息
	const char * stackTrace() const throw();	//打印堆栈痕迹

private:
	void fillStackTrace();
	std::string demangle(const char *symbol);

	std::string message_;
	std::string stack_;
};
}

#endif  /*EXCEPTION_H_*/
