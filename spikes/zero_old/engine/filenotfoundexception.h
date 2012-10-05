#ifndef FILENOTFOUNDEXCEPTION_H_
#define FILENOTFOUNDEXCEPTION_H_

#include <exception>
#include <string>

class FileNotFoundException : public std::exception
{
public:
	FileNotFoundException(const std::string &errors);
	~FileNotFoundException() throw ();
	const char *what() const throw();
private:
	const std::string errors;
};

#endif

