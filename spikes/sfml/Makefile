all: main

main.o: main.cpp GeneralException.hpp File.hpp ShaderProgram.hpp Inotify.hpp UI.hpp VertexBuffer.hpp FPSCounter.hpp GameState.hpp models.hpp Camera.hpp
	g++ -ggdb -std=c++0x -Wall -Wextra -pedantic -c main.cpp  -isystem external/include 

main: main.o
	g++ -ggdb -Wl,--rpath,external/lib -Wl,--rpath,external/lib64 -o main main.o external/lib64/libGLEW.a -Lexternal/lib -lX11 -lGL -lsfml-window -lRocketCore -lsfml-graphics -lsfml-system


