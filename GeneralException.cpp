#ifndef GENERALEXCEPTION_
#define GENERALEXCEPTION_
#include <exception>
#include <string>

class GeneralException : public std::exception
{
public:
	GeneralException(const std::string &errors)
	: errors(errors)
	{
	}
	~GeneralException() throw ()
	{
	}
	const char *what() const throw()
	{
		return errors.c_str();
	}
private:
	const std::string errors;
};
#endif 

