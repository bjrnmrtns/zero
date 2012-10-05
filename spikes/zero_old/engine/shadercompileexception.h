#ifndef SHADERCOMPILEEXCEPTION
#define SHADERCOMPILEEXCEPTION

#include <string>
#include <exception>

class ShaderCompileException : public std::exception
{
public:
	ShaderCompileException(const std::string &errors);
	~ShaderCompileException() throw ();
	const char *what() const throw();
	bool debug;
private:
	const std::string errors;
};

#endif
