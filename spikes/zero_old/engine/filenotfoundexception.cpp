#include "filenotfoundexception.h"

FileNotFoundException::FileNotFoundException(const std::string &errors)
: errors(errors)
{
}

FileNotFoundException::~FileNotFoundException() throw () 
{
}

const char* FileNotFoundException::what() const throw() 
{ 
	return errors.c_str(); 
}

