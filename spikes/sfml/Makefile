all: main

main.o: main.cpp GeneralException.hpp File.hpp ShaderProgram.hpp Inotify.hpp UI.hpp VertexBuffer.hpp FPSCounter.hpp
	g++ -ggdb -std=c++0x -Wall -Wextra -pedantic -c main.cpp  -Iexternal/include 

main: main.o
	g++ -ggdb -Wl,--rpath,external/lib -o main main.o external/lib64/libGLEW.a -Lexternal/lib -lX11 -lGL -lsfml-window -lRocketCore -lsfml-graphics -lsfml-system


