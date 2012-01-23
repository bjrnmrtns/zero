#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/glm.hpp>
#include <exception>
#include <string>
#include <memory>

static const char *vs =
"#version 150\n"
"\n"
"in vec3 in_Position;\n"
"out vec3 pass_Position;\n"
"\n"
"void main()\n"
"{\n"
"  gl_Position = vec4(in_Position, 1.0);\n"
"  pass_Position = gl_Position.xyz;\n"
"}\n";

static const char *fs =
"#version 150\n"
"\n"
"in vec3 pass_Position;\n"
"out vec3 outColor;\n"
"\n"
"void main(void)\n"
"{\n"
"  outColor = vec3(1.0, 1.0, 0.0);\n"
"}\n";

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

class Shader
{
public:
	Shader(const char* source, int type) : type(type)
	{
		id = glCreateShader(type);
		glShaderSource(id, 1, &source, 0);
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
	int GetId() { return id; }
private:
	int id;
	int type;
};

class VertexShader : public Shader
{
public:
	VertexShader(const char* source) : Shader(source, GL_VERTEX_SHADER) {}
};

class FragmentShader : public Shader
{
public:
	FragmentShader(const char* source) : Shader(source, GL_FRAGMENT_SHADER) {}
};

class ShaderProgram
{
public:
	ShaderProgram(VertexShader &vertexShader, FragmentShader &fragmentShader)
	: vertexShader(vertexShader)
	, fragmentShader(fragmentShader)
	{
		id = glCreateProgram();
		glAttachShader(id, vertexShader.GetId());
		glAttachShader(id, fragmentShader.GetId());
		
		glBindAttribLocation(id, 0, "in_Position");
		glLinkProgram(id);
		
		int ok = true;
		glGetShaderiv(id, GL_LINK_STATUS, &ok);
		if(!ok)
		{
			int length = 0;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
			std::unique_ptr<char> errors(new char[length]);
			glGetProgramInfoLog(id, length, 0, errors.get());
			throw GeneralException(std::string(errors.get()));
		}
	}
	~ShaderProgram()
	{
		glDetachShader(id, fragmentShader.GetId());
		glDetachShader(id, vertexShader.GetId());
		glDeleteProgram(id);
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
	void Use()
	{
		glUseProgram(id);
	}
private:
	int id;
	VertexShader& vertexShader;
	FragmentShader& fragmentShader;
};

class VertexBuffer
{
public:
	struct InputElementDescription
	{
		std::string name;
		size_t numberofelements;
		size_t elementsize;
	};
	template <typename T>
	VertexBuffer(InputElementDescription *description, const T *vertexData, size_t count)
	: stride(sizeof(T))
	, count(count)
	{
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glGenBuffers(1, &id);
		glBindBuffer(GL_ARRAY_BUFFER, id);
		size_t offset = 0;
		for(int i = 0; description[i].elementsize; ++i)
		{
			glEnableVertexAttribArray(i);
			glVertexAttribPointer(i, description[i].numberofelements, GL_FLOAT, GL_FALSE, stride, (void *)offset);
			offset += description[i].numberofelements * description[i].elementsize;
		}
		glBufferData(GL_ARRAY_BUFFER, stride * count, vertexData, GL_STATIC_DRAW);
	}
	void Draw()
	{
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, count);
	}
	~VertexBuffer()
	{
		glDeleteBuffers(1, &id);
		glDeleteVertexArrays(1, &vao);
	}
private:
	unsigned int id;
	unsigned int vao;
	unsigned int stride;
	size_t count;
};

class Window_
{
public:
	Window_(size_t width, size_t height)
	{
		if(!glfwInit()) throw new GeneralException("glfwInit failed");
		glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
		glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
		glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		if(!glfwOpenWindow(width, height, 8, 8, 8, 0, 24, 8, GLFW_WINDOW))
		{
			glfwTerminate();
			throw new GeneralException("glfwOpenWindow failed");
		}
		glewExperimental = GL_TRUE;
		if(glewInit() != GLEW_OK) throw new GeneralException("glewInit failed");

		glCullFace(GL_BACK);
		glFrontFace(GL_CW);
		glEnable(GL_CULL_FACE);
		glClearDepth(1.0f);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glViewport(0, 0, width, height);
	}
	void Swap()
	{
		glfwSwapBuffers();
	}
	~Window_()
	{
		glfwTerminate();
	}
};

int main()
{
	Window_ window(1024, 768);
	VertexShader vertexShader(vs);
	FragmentShader fragmentShader(fs);
	ShaderProgram shaderProgram(vertexShader, fragmentShader);
	shaderProgram.Use();
	static VertexBuffer::InputElementDescription description[] = {
		{ "in_position", 3, sizeof(glm::vec3) },
		{ "", 0, 0 }
	};
	float triangle[] = {-1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f};
	VertexBuffer vb(description, triangle, sizeof(triangle)/sizeof(float));
	bool running = true;
	while(running)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		vb.Draw();
		window.Swap();
		running = !glfwGetKey(GLFW_KEY_ESC) && 
		          glfwGetWindowParam(GLFW_OPENED);

	}
}
