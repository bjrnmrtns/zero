all: glfwtest

glfwtest: glfwtest.cpp
	g++ -ggdb -std=c++0x -Wall -Wextra -pedantic -o glfwtest glfwtest.cpp external/lib/libglfw.a external/lib64/libGLEW.a external/lib/libIL.a -lpng -Iexternal/include -lX11 -lGL
