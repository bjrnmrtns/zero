#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/glm.hpp>
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
//	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
//	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
//	glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	if(!glfwOpenWindow(1024, 768, 8, 8, 8, 0, 24, 8, GLFW_WINDOW))
	{
		glfwTerminate();
		throw new GeneralException("glfwOpenWindow failed");
	}
	if(glewInit() != GLEW_OK) throw new GeneralException("glewInit failed");

	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
	glEnable(GL_CULL_FACE);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glViewport(0, 0, 1024, 768);

	static VertexBuffer::InputElementDescription elementDescription[] { {"position", sizeof(glm::vec3)} };
	float triangle[] = {0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f};

	VertexBuffer vb(modelRefs, triangle, sizeof(triangle)/sizeof(float));
	bool running = true;
	while( running)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_TRIANGLES);
		glVertex3f(0.0f,-1.0f,0.0f);
		glVertex3f(0.0f,1.0f,0.0f);
		glVertex3f(1.0f,0.0f,0.0f);
		glEnd();
		glfwSwapBuffers();
		running = !glfwGetKey(GLFW_KEY_ESC) && 
		          glfwGetWindowParam(GLFW_OPENED);

	}
	glfwTerminate();
}
