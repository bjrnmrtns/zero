#ifndef SHADERPROGRAM__
#define SHADERPROGRAM__

#include <string>
#include <GL/gl.h>
#include <memory>
#include <vector>

#include "File.hpp"
#include "Inotify.hpp"
#include "InputElementDescription.hpp"


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

class ShaderProgram : public Reloadable
{
private:
	std::string vsfilename;
	std::string fsfilename;
	std::unique_ptr<Shader> vs;
	std::unique_ptr<Shader> fs;
	unsigned int id;
public:
	ShaderProgram(std::string vsfilename, std::string fsfilename, const InputElementDescription description[])
	: vsfilename(vsfilename)
	, fsfilename(fsfilename)
	, vs(new Shader(vsfilename, GL_VERTEX_SHADER))
	, fs(new Shader(fsfilename, GL_FRAGMENT_SHADER))
	{
		id = glCreateProgram();
		if(id == 0) throw GeneralException("glCreateProgram of shader failed");
		for(int i = 0; description[i].elementsize; ++i)
		{
			glBindAttribLocation(id, i, description[i].name.c_str());
		}
		Link();
		Inotify::Register(vsfilename, this);
		Inotify::Register(fsfilename, this);
	}
	~ShaderProgram()
	{
		glDetachShader(id, fs->GetId());
		glDetachShader(id, vs->GetId());
		glDeleteProgram(id);
	}
	void Use()
	{
		glUseProgram(id);
	}
	void Link()
	{
		glAttachShader(id, vs->GetId());
		glAttachShader(id, fs->GetId());
		glLinkProgram(id);

		int ok = true;
		glGetShaderiv(id, GL_LINK_STATUS, &ok);
		glGetError(); //TODO: remove this because it is used to ignore the error GL_INVALID_OPERATION after glGetShaderiv
		if(!ok)
		{
			int length = 0;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
			std::unique_ptr<char> errors(new char[length]);
			glGetProgramInfoLog(id, length, 0, errors.get());
			throw GeneralException(std::string(errors.get()));
		}
	}
	void Set(const char *name, const float mat[16])
	{
		int uniform = glGetUniformLocation(id, name);
		if (uniform == -1) return;
		glUseProgram(id);
		glUniformMatrix4fv(uniform, 1, GL_FALSE, mat);
		int error = glGetError(); if (error) { printf("%d\n", error); abort(); }
	}
	bool SetTexture(const char *name, int value)
	{
		int error;
		int uniform = glGetUniformLocation(id, name);
		error = glGetError(); if (error) { printf("%d\n", error); abort(); }
		if (uniform == -1) return false;
		glUseProgram(id);
		glUniform1i(uniform, value);
		error = glGetError(); if (error) { printf("%d\n", error); abort(); }
		return true;
	}
	void reload()
	{
		unsigned int oldvsid = vs->GetId();
		unsigned int oldfsid = fs->GetId();
		vs.reset(new Shader(vsfilename, GL_VERTEX_SHADER));
		fs.reset(new Shader(fsfilename, GL_FRAGMENT_SHADER));
		glDetachShader(id, oldvsid);
		glDetachShader(id, oldfsid);
		Link();
	}
};

#endif
