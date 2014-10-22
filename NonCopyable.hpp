#ifndef NONCOPYABLE_HPP
#define NONCOPYABLE_HPP


//禁用值语义
class NonCopyable
{
protected:
	NonCopyable() {  }
	~NonCopyable() {  }
private:
	NonCopyable(const NonCopyable &);
	void operator=(const NonCopyable &);	
};



#endif	//NONCOPYABLE_HPP