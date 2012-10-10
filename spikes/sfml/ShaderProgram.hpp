#ifndef SHADERPROGRAM__
#define SHADERPROGRAM__

#include <string>
#include "File.hpp"
#include <GL/gl.h>
#include <memory>

class Shader
{
public:
	Shader(const std::string filename, int type)
	{
		Blob shaderdata = File::read(filename);
		const char * source = (const char*)shaderdata.buf.get();
		id = glCreateShader(type);
		glShaderSource(id, 1, &source, (int *)&shaderdata.size);
		glCompileShader(id);
		int ok = true;
		glGetShaderiv(id, GL_COMPILE_STATUS, &ok);
		if(!ok)
		{
			int length = 0;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
			std::unique_ptr<char> errors(new char[length]);
			glGetShaderInfoLog(id, length, 0, errors.get());
			throw GeneralException(std::string(errors.get()));
		}
	}
	virtual ~Shader()
	{
		glDeleteShader(id);
	}
	unsigned int GetId() { return id; }
private:
	unsigned int id;
};

class ReloadableShader
{
private:
	int type;
	std::string filename;
	std::unique_ptr<Shader> res;
public:
	ReloadableShader(const std::string filename, int type)
	: type(type)
	, filename(filename)
	, res(new Shader(filename, type))
	{
	}
	void reload()
	{
		res.reset(new Shader(filename, type));
	}
	unsigned int GetId()
	{
		return res->GetId();
	}
};



class ShaderProgram
{
	ReloadableShader vs;
	ReloadableShader fs;
public:
	ShaderProgram(std::string vsfilename, std::string fsfilename)
	: vs(vsfilename, GL_VERTEX_SHADER)
	, fs(fsfilename, GL_FRAGMENT_SHADER)
	{
	}
	void reload()
	{
		vs.reload();
		fs.reload();
	}
};

#endif
