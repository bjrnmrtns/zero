#include "shadercompileexception.h"

ShaderCompileException::ShaderCompileException(const std::string &errors)
	: errors(errors)
{}
ShaderCompileException::~ShaderCompileException() throw () {}
const char* ShaderCompileException::what() const throw() { return errors.c_str(); }

