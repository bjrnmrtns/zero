all: RenderPipeline gamearchitecture RenderToTexture glfwtest

gamearchitecture: gamearchitecture.cpp
	g++ -std=c++0x -Wall -Wextra -pedantic -o gamearchitecture gamearchitecture.cpp 
RenderPipeline: RenderPipeline.cpp
	g++ -std=c++0x -Wall -Wextra -pedantic -o RenderPipeline RenderPipeline.cpp 
RenderToTexture: RenderToTexture.cpp
	g++ -std=c++0x -Wall -Wextra -pedantic -o RenderToTexture RenderToTexture.cpp 
glfwtest: glfwtest.cpp
	g++ -std=c++0x -Wall -Wextra -pedantic -o glfwtest glfwtest.cpp external/lib/libglfw.a -Iexternal/include -lX11 -lGL
