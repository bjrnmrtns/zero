#include <GL/glew.h>
#include <GL/glfw.h>
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

int main()
{
	if(!glfwInit()) throw new GeneralException("glfwInit failed");
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
	glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	if(!glfwOpenWindow(1024, 768, 8, 8, 8, 0, 24, 8, GLFW_WINDOW))
	{
		glfwTerminate();
		throw new GeneralException("glfwOpenWindow failed");
	}
	if(glewInit() != GLEW_OK) throw new GeneralException("glewInit failed");
	bool running = true;
	while( running)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glfwSwapBuffers();
		running = !glfwGetKey(GLFW_KEY_ESC) && 
		          glfwGetWindowParam(GLFW_OPENED);
	}
	glfwTerminate();
}
