#include <GL/glfw.h>

int main()
{
	glfwInit();
	glfwOpenWindow(300, 300, 0, 0, 0, 0, 0, 0, GLFW_WINDOW);
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
