#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/glm.hpp>
#include <exception>
#include <string>

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
			char errors[length];
			glGetShaderInfoLog(id, length, 0, errors);
			throw GeneralException(std::string(errors));
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
		size_t size;
	};

	struct AttributeReference
	{
		std::string name;
		int offset;
		int count;
	};
	template <typename T>
	VertexBuffer(AttributeReference *refs, const T *vertexData, size_t count)
	: stride(sizeof(T))
	, count(count)
	, refs(refs)
	{
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glGenBuffers(1, &id);
		glBindBuffer(GL_ARRAY_BUFFER, id);
		for(int i = 0; refs[i].count; ++i)
		{
			glEnableVertexAttribArray(i);
			glVertexAttribPointer(i, refs[i].count, GL_FLOAT, GL_FALSE, stride, (void *)refs[i].offset);
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
	AttributeReference *refs;
};

static VertexBuffer::AttributeReference modelRefs[] =
{
        { "in_vertex", 0, 3  },
        { "", 0, 0  }
};


int main()
{
	if(!glfwInit()) throw new GeneralException("glfwInit failed");
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	if(!glfwOpenWindow(1024, 768, 8, 8, 8, 0, 24, 8, GLFW_WINDOW))
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

	glViewport(0, 0, 1024, 768);

	VertexShader vertexShader(vs);
	FragmentShader fragmentShader(fs);
	ShaderProgram shaderProgram(vertexShader, fragmentShader);
	shaderProgram.Use();
	static VertexBuffer::InputElementDescription elementDescription[] { {"position", sizeof(glm::vec3)} };
	float triangle[] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f};

	VertexBuffer vb(modelRefs, triangle, sizeof(triangle)/sizeof(float));
	bool running = true;
	while( running)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		vb.Draw();
/*		glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_TRIANGLES);
		glVertex3f(0.0f,-1.0f,0.0f);
		glVertex3f(0.0f,1.0f,0.0f);
		glVertex3f(1.0f,0.0f,0.0f);
		glEnd();*/
		glfwSwapBuffers();
		running = !glfwGetKey(GLFW_KEY_ESC) && 
		          glfwGetWindowParam(GLFW_OPENED);

	}
	glfwTerminate();
}
