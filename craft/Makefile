all: craft

craft.o: craft.cpp
	g++ -ggdb -std=c++0x -Wall -Wextra -pedantic -c craft.cpp  -I../external/include 

craft: craft.o
	g++ -ggdb -Wl,--rpath,../external/lib -o craft craft.o ../external/lib/libglfw.a ../external/lib64/libGLEW.a -L../external/lib -lX11 -lGL


